
from pycparser import CParser
from pycparser import c_parser, c_ast, parse_file

class DeclVisitor(c_ast.NodeVisitor):
    def __init__(self):
        self.functions = []
        
    def visit_Decl(self, node):
        if type(node.type) is c_ast.FuncDecl:
            self.functions.append(node.name)

class WrapperGenerator(object):

    def __init__(self, text, filename):
        self.text = text
        parser = CParser()
        self.ast = parser.parse(text, filename)
        

    def definitions(self):
        visitor = DeclVisitor()
        visitor.visit(self.ast)
        definitions = ["int bash_%s(WORD_LIST *args);" % f
                       for f in visitor.functions]
        return '\n'.join(definitions)
