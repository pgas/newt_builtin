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
        name, argtype = v.functions[1].args[0]
        self.assertEqual('i', name)
        self.assertEqual('int', argtype)
        name, argtype = v.functions[1].args[1]
        self.assertEqual('newtComponent', argtype)

    def testDeclVisitorReturnPointerParam(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(char * foo, char ** bar);", "aname")
        v.visit(ast)
        name, argtype = v.functions[0].args[0]
        self.assertEqual('foo', name)
        self.assertEqual('char *', argtype)
        name, argtype = v.functions[0].args[1]
        self.assertEqual('bar', name)
        self.assertEqual('char **', argtype)


class TestWrapperGenerator(unittest.TestCase):

    def testHeaderFromTemplate(self):
        gen = WrapperGenerator("int newtInit(void);",
                               "aname",
                               Template("{% for func in funcs %}int bash_{{ func.name }}(WORD_LIST *args);{% endfor %}"))
        self.assertEqual("int bash_newtInit(WORD_LIST *args);",
                         gen.render_header())

    def testArgsFromTemplate(self):
        gen = WrapperGenerator("int newtInit(int i);",
                               "aname",
                               Template("""{%- for func in funcs %}
{%- for argname, argtype in func.args %}
{{ argtype }} {{ argname }};
{%- endfor %}
{%- endfor %}"""))
        self.assertIn("int i;",
                      gen.render_header())

if __name__ == '__main__':
    unittest.main()

    
