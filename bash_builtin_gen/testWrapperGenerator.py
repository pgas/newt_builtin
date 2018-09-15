import unittest

from pycparser import CParser
from jinja2 import Template

from . import WrapperGenerator
from .wrapper_generator import DeclVisitor


class TestDeclVisitor(unittest.TestCase):
    def setUp(self):
        source = """
        typedef struct { int i; } newtComponent;
        int newtInit(void);
        int newtFoo(int i, newtComponent string);
        """
        self.ast = CParser().parse(source, "aname")

    def testDeclVisitorReturnAllNames(self):
        v = DeclVisitor()
        v.visit(self.ast)
        self.assertEqual(2, len(v.functions))

    def testDeclVisitorReturnFuncName(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(void);", "aname")
        v.visit(ast)
        self.assertEqual('newtInit', v.functions[0].name)

    def testDeclVisitorReturnEmptyParam(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(void);", "aname")
        v.visit(ast)
        self.assertEqual([], v.functions[0].args)

    def testDeclVisitorReturnTwoParam(self):
        v = DeclVisitor()
        v.visit(self.ast)
        self.assertEqual(2, len(v.functions[1].args))
        argtype, name = v.functions[1].args[0]
        self.assertEqual('i', name)
        self.assertEqual('int', argtype)
        argtype, name = v.functions[1].args[1]
        self.assertEqual('newtComponent', argtype)

    def testDeclVisitorReturnPointerParam(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(char * foo, char ** bar);",
                              "aname")
        v.visit(ast)
        argtype, name = v.functions[0].args[0]
        self.assertEqual('foo', name)
        self.assertEqual('char *', argtype)
        argtype, name = v.functions[0].args[1]
        self.assertEqual('bar', name)
        self.assertEqual('char **', argtype)


    def testDeclVisitorReturnEnumParam(self):
        v = DeclVisitor()
        ast = CParser().parse("""
        void f(enum newtFlagsSense sense);
        """,
                              "aname")
        v.visit(ast)
        argtype, name = v.functions[0].args[0]
        self.assertEqual('sense', name)
        self.assertEqual('enum newtFlagsSense', argtype)

    def testDeclVisitorReturnEllipsis(self):
        v = DeclVisitor()
        ast = CParser().parse("""
        void f(int foo, ...);
        """,
                              "aname")
        v.visit(ast)
        print(v.functions[0].args)
#        self.assertEqual(2, len(v.functions[0].args))


class TestWrapperGenerator(unittest.TestCase):

    def testHeaderFromTemplate(self):
        gen = WrapperGenerator("int newtInit(void);",
                               "aname",
                               Template("""\
 {%- for func in funcs %}\
int bash_{{ func.name }}(WORD_LIST *args);\
{%- endfor %}"""))
        self.assertEqual("int bash_newtInit(WORD_LIST *args);",
                               gen.render_header())

    def testArgsFromTemplate(self):
        gen = WrapperGenerator("int newtInit(int i);",
                               "aname",
                               Template("""{%- for func in funcs %}
{%- for  argtype, argname in func.args %}
{{ argtype }} {{ argname }};
{%- endfor %}
{%- endfor %}"""))
        self.assertIn("int i;",
                      gen.render_header())

if __name__ == '__main__':
    unittest.main()

    
