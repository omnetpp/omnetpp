#!/usr/bin/env python3

import sys
import os
import re

from omnetpp.scave import results, chart, utils, ideplot, vectorops, analysis, charttemplate

import mistune
import inspect

def finalize_latex(s):
    substitutes = [
        ("\\", "\b"), # temporarily
        ("{", "\\{"),
        ("}", "\\}"),
        ("_", "\\_"),
        ("$", "\\$"),
        ("%", "\\%"),
        ("&", "\\&"),
        ("#", "\\#"),
        ("^", "{\\textasciicircum}"),
        ("~", "{\\textasciitilde}"),
        ("\b", "{\\textbackslash}"),

        # this is not necessary at the moment, but might be in the future
        #(" ", " {\\allowbreak}"),

        # this is to activate the formatting inserted by us
        ("☾", "{"),
        ("☽", "}"),
        ("∇", "\\"),
    ]

    for a, b in substitutes:
        s = s.replace(a, b)

    return s


sssnum = 0
class LaTeXRenderer(mistune.renderers.html.HTMLRenderer):
    def block_code(self, code, info=None):
        # "\\begin{sloppypar}\n\\begin{flushleft}\n\\ttt{  ???
        return "\n∇begin☾filelisting☽\n" + code + "\n∇end☾filelisting☽\n"

    def block_quote(self, text):
        return text # add nice quotes?

    def block_html(self, html):
        return html # strip tags?

    def heading(self, text, level):
        global sssnum
        sssnum = sssnum + 1
        return "\n\n∇subsubsection☾" + text + "☽\n∇label☾sssec:num" + str(sssnum) + "☽\n\n"

    def list(self, text, ordered, depth, start=None):
        if ordered:
            return "\n\n∇begin☾enumerate☽\n" + text + "\n∇end☾enumerate☽\n"
        else:
            return "\n\n∇begin☾itemize☽\n" + text + "\n∇end☾itemize☽\n"

    def list_item(self, text):
        return "  ∇item " + text + "\n"

    def paragraph(self, text):
        return "\n\n∇par " + text

    def emphasis(self, text):
        return "∇textit☾" + text + "☽"

    def strong(self, text):
        return "∇textbf☾" + text + "☽"

    def codespan(self, text):
        return "∇ttt☾" + text + "☽"

    def linebreak(self):
        return "\n"

    def newline(self):
        return "∇newline\n"

    def link(self, link, text=None, title=None):
        return link # add \href

    def strikethrough(self, text):
        return text

    def text(self, text):
        return text

    def inline_html(self, text):
        return text


renderer = LaTeXRenderer()
markdown = mistune.create_markdown(renderer=renderer)

def docstring_to_latex(o):
    d = o.__doc__
    d = preformat(d)
    return finalize_latex(markdown(inspect.cleandoc(d))) if d else ""

def preformat(docstring):
    if not docstring:
        return docstring

    # This is a HACK to allow putting stuff like "\1" in docstrings, and make them both look right in the code, and also
    # end up looking the same in the output (even though the string itself contains the ASCII control characters 1, 2, etc.)
    docstring = docstring.replace("\1", "\\1").replace("\2", "\\2").replace("\3", "\\3") \
        .replace("\4", "\\4").replace("\5", "\\5").replace("\6", "\\6").replace("\7", "\\7")

    # make type string italic in places like this:
    # " - `x`, `y` (float): The coordinates"
    pattern = re.compile(r"""
        ^(\s*-\s`[^:(]*)  # prefix: starts with backquoted word, and extends until a colon or left paren
        (\(.*\))          # the type part in parens
        (\s*:)""",        # the colon
        re.MULTILINE|re.VERBOSE)
    docstring = pattern.sub(r'\1*\2*\3', docstring)  # put type part between *asterisks* (italic)
    return docstring


def signature_to_latex(o):
    signature = str(inspect.signature(o))
    signature = re.sub("=<function ([^ ]+) at [^>]+>", "=\\1", signature)
    return finalize_latex(str(signature).replace("'", '"'))

def quote(str):
    return str.replace("_", "\\_")

def tolabel(str):
    return str.replace("_", "-")

def annotate_module(mod):
    modname = mod.__name__
    print("\\subsection{Module " + modname + "}")
    print("\\label{cha:chart-api:" + modname + "}\n")

    print(docstring_to_latex(mod).strip() + "\n")

    for k in mod.__dict__:
        o = mod.__dict__[k]
        if not k or k[0] == '_':
            pass
        elif inspect.isfunction(o) and k != "print" and k != "wraps":
            print("\\subsubsection{" + quote(k) + "()}")
            print("\\label{cha:chart-api:" + modname + ":" + tolabel(k) + "}\n")
            print("\\begin{flushleft}\n\\ttt{" + finalize_latex(o.__name__) + signature_to_latex(o) + "}\n\\end{flushleft}\n")
            print(docstring_to_latex(o).strip() + "\n")
        elif inspect.isclass(o):
            classname,clazz = k,o
            print("\\subsubsection{Class " + quote(classname) + "}")
            print("\\label{cha:chart-api:" + modname + ":" + tolabel(classname) + "}\n")
            print(docstring_to_latex(clazz).strip() + "\n")
            for membername,member in inspect.getmembers(clazz):
                if not membername or (membername[0] == '_'  and membername!="__init__"):
                    pass
                elif inspect.isfunction(member):
                    #print("\\subsubsection{" + quote(classname) + "." + quote(membername) + "()}")
                    print("\\label{cha:chart-api:" + modname + ":" + tolabel(classname) + ":" + tolabel(membername) + "}\n")

                    if membername == "__init__":
                        print("\\begin{flushleft}\n\\ttt{" + quote(classname) + signature_to_latex(member) + "}\n\\end{flushleft}\n")
                    else:
                        print("\\begin{flushleft}\n\\ttt{\\small{" + quote(classname) + ".}" + finalize_latex(membername) + signature_to_latex(member) + "}\n\\end{flushleft}\n")

                    print(docstring_to_latex(member).strip() + "\n")


print("""
%
% generated with extract_chart_api.py
%
""")

annotate_module(results)
annotate_module(chart)
annotate_module(ideplot)
annotate_module(utils)
annotate_module(vectorops)
annotate_module(analysis)
annotate_module(charttemplate)
