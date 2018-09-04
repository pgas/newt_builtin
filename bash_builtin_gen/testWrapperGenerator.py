import unittest

from pycparser import CParser
from jinja2 import Template

from . import WrapperGenerator
from .wrapper_generator import DeclVisitor


class TestDeclVisitor(unittest.TestCase):

    def testDeclVisitorReturnAllNames(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(void);int Another(void);", "aname")
        v.visit(ast)
        self.assertEqual(2, len(v.functions))

    def testDeclVisitorReturnFuncName(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(void);", "aname")
        v.visit(ast)
        self.assertEqual('newtInit', v.functions[0])

class TestWrapperGenerator(unittest.TestCase):

    def testHeaderFromTemplate(self):
        gen = WrapperGenerator("int newtInit(void);",
                               "aname",
                           Template("{% for name in names %}int bash_{{ name }}(WORD_LIST *args);{% endfor %}"))
        self.assertEqual("int bash_newtInit(WORD_LIST *args);",
                    gen.render_header())
    
    
if __name__ == '__main__':
    unittest.main()
