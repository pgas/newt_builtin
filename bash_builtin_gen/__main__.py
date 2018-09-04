import sys
from jinja2 import Environment,  FileSystemLoader
import jinja2
from . import WrapperGenerator


def usage():
    print """%s header_template source_header_file""" % __package__

if not len(sys.argv) == 3:
    usage()
    quit(1)

header_template_name = sys.argv[1]
source_header_file = sys.argv[2]

env = Environment(
    loader=FileSystemLoader('src', followlinks=True)
)

try:
    header_template = env.get_template(header_template_name)
except jinja2.exceptions.TemplateNotFound:
    print "header_template not found: %s" % header_template_name
    quit(2)

try:
    with open(source_header_file, "r") as f:
        w = WrapperGenerator(f.read(),
                             source_header_file,
                             header_template)
        print w.render_header()
except:
    print "error generating header"
