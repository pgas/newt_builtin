from pycparser import CParser, c_ast
from collections import namedtuple
from jinja2 import Template, Environment


Function = namedtuple('Function', 'name args return_type')


class DeclVisitor(c_ast.NodeVisitor):
    def __init__(self):
        self.functions = []


    def param_to_string(self, node):
        stars = ""
        while type(node) is c_ast.PtrDecl:
            stars += "*"
            node = node.type
        if node.declname:
            typename = ""
            if type(node.type) is c_ast.IdentifierType:
                typename = " ".join(node.type.names)
            elif type(node.type) is c_ast.Enum:
                typename = "enum " + node.type.name
            elif type(node.type) is c_ast.Struct:
                typename = "struct " + node.type.name
            if len(stars) > 0:
                typename += " " + stars
            return (typename, node.declname)
        else:
            return None

        
    def visit_Decl(self, node):
        if type(node.type) is c_ast.FuncDecl:
            args = []
            for param in node.type.args.params:
                if type(param) is c_ast.EllipsisParam:
                    args.append(("ellipsis", '...'))
                else:
                    param_type = self.param_to_string(param.type)
                    if param_type:
                        args.append(param_type)
            fun_typename, _name = self.param_to_string(node.type.type)
            if not fun_typename:
                fun_typename = ""
            f = Function(node.name, args, fun_typename)
            self.functions.append(f)


def without_variadic(func_list):
    return [func for func in func_list if ('ellipsis', '...') not in func.args]


def convertion_fun(type_name):
    type_name = type_name.replace(' ', "_")
    type_name = type_name.replace('*', "__ptr__")
    return "string_to_" + type_name


class WrapperGenerator(object):

    def __init__(self, text, filename):
        ast = CParser().parse(text, filename)
        self.visitor = DeclVisitor()
        self.visitor.visit(ast)
        self.env = Environment()
        self.env.filters['without_variadic'] = without_variadic
        self.env.filters['convertion_fun'] = convertion_fun

    def render(self, template_string):
        template = self.env.from_string(template_string)
        return template.render(funcs=self.visitor.functions,
                               lstrip_blocks=True)
