import sys
from . import WrapperGenerator


def usage():
    print("""%s  source_header_file template x""" % __package__)


if not len(sys.argv) == 3:
    usage()
    quit(1)

source_file = sys.argv[1]
template_file = sys.argv[2]

try:
    with open(source_file, "r") as source,\
         open(template_file, "r") as template:
        w = WrapperGenerator(source.read(), source_file)
        print(w.render(template.read()))
except Exception as e:
    sys.stderr.write("error generating %s: %s\n" % (template_file, e))
    quit(2)
