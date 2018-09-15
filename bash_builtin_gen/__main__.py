import sys
from jinja2 import Environment,  FileSystemLoader
import jinja2
from . import WrapperGenerator


def usage():
    print("""%s template source_header_file""" % __package__)


if not len(sys.argv) == 3:
    usage()
    quit(1)

template_name = sys.argv[1]
source_header_file = sys.argv[2]

env = Environment(
    loader=FileSystemLoader('src', followlinks=True)
)

try:
    template = env.get_template(template_name)
except jinja2.exceptions.TemplateNotFound:
    print("header_template not found: %s" % template_name)
    quit(2)

try:
    with open(source_header_file, "r") as f:
        w = WrapperGenerator(f.read(),
                             source_header_file)
        print(w.render(template))
except Exception as e:
    print("error generating header %s" % e)
