#!/usr/bin/env python3

import sys
import os

from omnetpp.scave import results, chart, utils, plot

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
        ("\b", "{\\textbackslash}"),
        ("~", "{\\textasciitilde}"),

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
class LaTeXRenderer(mistune.Renderer):
    def block_code(self, code, lang=None):
        # "\\begin{sloppypar}\n\\begin{flushleft}\n\\ttt{  ???
        return "\n∇begin☾filelisting☽\n" + code + "\n∇end☾filelisting☽\n"

    def block_quote(self, text):
        return text # add nice quotes?

    def block_html(self, html):
        return html # strip tags?

    def header(self, text, level, raw=None):
        global sssnum
        sssnum = sssnum + 1
        return "\n∇subsubsection☾" + text + "☽\n∇label☾sssec:num" + str(sssnum) + "☽\n\n"

    def hrule(self):
        return "\n∇noindent∇hrulefill\n\n"

    def list(self, body, ordered=True):
        if ordered:
            return "∇begin☾enumerate☽\n" + body + "\n∇end☾enumerate☽\n"
        else:
            return "∇begin☾itemize☽\n" + body + "\n∇end☾itemize☽\n"

    def list_item(self, text):
        return "  ∇item " + text + "\n"

    def paragraph(self, text):
        return "∇par " + text

    def table(self, header, body):
        return header + body

    def table_row(self, content):
        return content

    def table_cell(self, content, **flags):
        return content



    def autolink(self, link, is_email=False):
        return link # add \url

    def codespan(self, text):
        return "∇ttt☾" + text + "☽"

    def double_emphasis(self, text):
        return "∇textbf☾" + text + "☽"

    def emphasis(self, text):
        return "∇textit☾" + text + "☽"

    def image(self, src, title, alt_text):
        return "Image: " + title + " (" + alt_text + ")"

    def linebreak(self):
        return "\n"

    def newline(self):
        return "∇newline\n"

    def link(self, link, title, content):
        return link # add \href

    def strikethrough(self, text):
        return text

    def text(self, text):
        return text

    def inline_html(self, text):
        return text


renderer = LaTeXRenderer()
markdown = mistune.Markdown(renderer=renderer)

def docstring_to_latex(o):
    d = o.__doc__
    return finalize_latex(markdown.render(inspect.cleandoc(d))) if d else ""

def signature_to_latex(o):
    return finalize_latex(o.__name__ + str(inspect.signature(o)).replace("'", '"'))

def annotate_module(mod):
    modname = mod.__name__
    prefix = "omnetpp.scave."
    if modname.startswith(prefix):
        modname = modname[len(prefix):]
    print("\\section{" + modname + " module}")
    print("\\label{cha:chart-api:" + modname + "}\n")

    print(docstring_to_latex(mod))

    for k in mod.__dict__:
        o = mod.__dict__[k]
        if k and k[0] != '_' and k != "print" and inspect.isfunction(o):
            print("\\subsection{" + k.replace("_", "\\_") + "()}")
            print("\\label{cha:chart-api:" + modname + ":" + k.replace("_", "-") + "}\n")
            print("\\begin{flushleft}\n\\ttt{" + signature_to_latex(o) + "}\n\\end{flushleft}\n\n")

            print(docstring_to_latex(o))


print("""
%
% generated with extract_chart_api.py
%
""")

annotate_module(results)
annotate_module(chart)

# vectorops?