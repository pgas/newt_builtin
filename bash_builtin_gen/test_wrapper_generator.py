import unittest
from pycparser import CParser

from . import WrapperGenerator
from .wrapper_generator import DeclVisitor, Function, without_variadic


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

    def testDeclVisitorReturnStructParam(self):
        v = DeclVisitor()
        ast = CParser().parse("""
        void f(struct bar a);
        """,
                              "aname")
        v.visit(ast)
        argtype, name = v.functions[0].args[0]
        self.assertEqual('a', name)
        self.assertEqual('struct bar', argtype)

    def testDeclVisitorReturnEllipsis(self):
        v = DeclVisitor()
        ast = CParser().parse("""
        void f(int foo, ...);
        """,
                              "aname")
        v.visit(ast)
        self.assertEqual(2, len(v.functions[0].args))
        self.assertEqual(('ellipsis', '...'), v.functions[0].args[1])


class TestFilters(unittest.TestCase):

    def testWithoutVariadic(self):
        funcs = []
        args = [('int', 'a'), ('ellipsis', '...')]
        funcs.append(Function('a', args, ''))
        args = [('int', 'a')]
        funcs.append(Function('b', args, ''))
        filtered = without_variadic(funcs)
        self.assertEqual(1, len(filtered))


class TestWrapperGenerator(unittest.TestCase):

    def testHeaderFromTemplate(self):
        gen = WrapperGenerator("int newtInit(void);",
                               "aname")
        template = """\
 {%- for func in funcs %}\
int bash_{{ func.name }}(WORD_LIST *args);\
{%- endfor %}"""
        self.assertEqual("int bash_newtInit(WORD_LIST *args);",
                         gen.render(template))

    def testArgsFromTemplate(self):
        gen = WrapperGenerator("int newtInit(int i);",
                               "aname")
        template = """{%- for func in funcs %}
{%- for  argtype, argname in func.args %}
{{ argtype }} {{ argname }};
{%- endfor %}
{%- endfor %}"""
        self.assertIn("int i;",
                      gen.render(template))

    def testFilteringVariadicFunction(self):
        gen = WrapperGenerator("""
int newtInit(int i);
int newtVariadic(int i, ...);
""",
                               "aname")
        template = """{%- for func in funcs | without_variadic %}
{%- for  argtype, argname in func.args %}{{ argtype }} {{ argname }};
{%- endfor %}
{%- endfor %}"""
        self.assertEqual("int i;",
                         gen.render(template))


if __name__ == '__main__':
    unittest.main()
