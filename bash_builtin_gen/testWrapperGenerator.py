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
        self.assertEqual('newtInit', v.functions[0].name)

    def testDeclVisitorReturnEmptyParam(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(void);", "aname")
        v.visit(ast)
        self.assertEqual([], v.functions[0].args)

    def testDeclVisitorReturnOneParam(self):
        v = DeclVisitor()
        ast = CParser().parse("int newtInit(int i);", "aname")
        v.visit(ast)
        self.assertEqual(1, len(v.functions[0].args))
        name, type = v.functions[0].args[0]
        self.assertEqual('i', name)

        
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
