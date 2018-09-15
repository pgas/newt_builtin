from pycparser import CParser, c_ast
from collections import namedtuple
import pprint

pp = pprint.PrettyPrinter(indent=4)

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


class WrapperGenerator(object):

    def __init__(self, text, filename, header_template):
        self.text = text
        parser = CParser()
        self.ast = parser.parse(text, filename)
        self.header_template = header_template

    def render_header(self):
        visitor = DeclVisitor()
        visitor.visit(self.ast)
        return self.header_template.render(funcs=visitor.functions,
                                           lstrip_blocks=True,
        )
