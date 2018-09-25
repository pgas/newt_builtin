#!/usr/bin/env python


from jinja2 import Environment, FileSystemLoader
import sys,os

if __name__ == '__main__':
    p = sys.argv[1]
    i = p.rfind('/')
    print(p[:i])
    env = Environment(loader=FileSystemLoader(p[:i]))
    print(p[i:])
    t = env.get_template(p[i:])
    print(t.render())

    
