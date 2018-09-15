from pycparser import CParser, c_ast
from collections import namedtuple
from jinja2 import Template


Function = namedtuple('Function', 'name args return_type')


class DeclVisitor(c_ast.NodeVisitor):
    def __init__(self):
        self.functions = []

    def visit_Decl(self, node):
        if type(node.type) is c_ast.FuncDecl:
            args = []
            for param in node.type.args.params:
                if type(param) is c_ast.EllipsisParam:
                    args.append(("ellipsis", '...'))
                else:
                    param = param.type
                    stars = ""
                    while type(param) is c_ast.PtrDecl:
                        stars += "*"
                        param = param.type
                    if param.declname:
                        typename = ""
                        if type(param.type) is c_ast.IdentifierType:
                            typename = " ".join(param.type.names)
                        elif type(param.type) is c_ast.Enum:
                            typename = "enum " + param.type.name
                        elif type(param.type) is c_ast.Struct:
                            typename = "struct " + param.type.name
                        if len(stars) > 0:
                            typename += " " + stars
                        args.append((typename, param.declname))
            f = Function(node.name, args, '')
            self.functions.append(f)


def without_variadic(func_list):
    return [func for func in func_list if ('ellipsis', '...') not in func.args]


class WrapperGenerator(object):

    def __init__(self, text, filename):
        self.text = text
        parser = CParser()
        self.ast = parser.parse(text, filename)

    def render(self, template_string):
        template = Template(template_string)
        visitor = DeclVisitor()
        visitor.visit(self.ast)
        return template.render(funcs=visitor.functions,
                               lstrip_blocks=True)
