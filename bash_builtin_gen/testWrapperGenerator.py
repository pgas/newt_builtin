import unittest
from . import WrapperGenerator
from pycparser import CParser
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

    def testFunctionDefinitionTranslation(self):
        gen = WrapperGenerator("int newtInit(void);", "aname")
        self.assertEqual("int bash_newtInit(WORD_LIST *args);",
                    gen.definitions())
    
if __name__ == '__main__':
    unittest.main()
