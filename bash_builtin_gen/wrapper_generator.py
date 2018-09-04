
from pycparser import CParser
from pycparser import c_parser, c_ast, parse_file
from jinja2 import Environment, PackageLoader, select_autoescape

class DeclVisitor(c_ast.NodeVisitor):
    def __init__(self):
        self.functions = []
        
    def visit_Decl(self, node):
        if type(node.type) is c_ast.FuncDecl:
            self.functions.append(node.name)

class WrapperGenerator(object):

    def __init__(self, text, filename, header_template):
        self.text = text
        parser = CParser()
        self.ast = parser.parse(text, filename)
        self.header_template = header_template

    def render_header(self):
        visitor = DeclVisitor()
        visitor.visit(self.ast)
        return self.header_template.render(names=visitor.functions)

