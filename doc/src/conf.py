# -*- coding: utf-8 -*-
#
# Sphinx configuration file for OMNeT++ documentation
# 
# The following environment variables are used (case sensitive):
# what= 'omnetpp' or 'omnest' (defalult 'omnetpp')
# VERSION= the full verison number of the project
# BUILDID= the build id (date, hash of the document sources)
# 
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
import string
sys.path.insert(0, os.path.abspath('.'))
sys.path.insert(0, os.path.abspath('_extensions'))
sys.path.insert(0, os.path.abspath('_templates'))

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['sphinx.ext.ifconfig', 'omnetpp.sphinx']

# -- Project information -----------------------------------------------------

# from environment variables
what = os.environ.get('what', default='omnetpp')
version = os.environ.get('VERSION', default='internal')
buildid = os.environ.get('BUILDID', default='deadbeef')

# Sphinx config variables
project = 'OMNeT++' if what=='omnetpp' else 'OMNEST'
copyright = '1992-2021, András Varga and OpenSim Ltd.'
author = 'OpenSim Ltd'
release = version

opp_replacements = {
    "|omnet++|" : project,
    "|omnetpp|" : what,
    "|version|" : version,
    "|buildid|" : buildid,
    "|downloadsite|" : "https://omnetpp.org" if what=='omnetpp' else "https://omnest.com"
}

# -- General configuration ---------------------------------------------------

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['prolog.rst']

# Global definitions.
rst_prolog = open("prolog.rst").read()


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_material'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = [
    'omnetpp.css',
]

html_sidebars = {
    "**": ["logo-text.html", "globaltoc.html", "localtoc.html", "searchbox.html"]
}

# Material theme options (see theme.conf for more information)
html_theme_options = {

    # Set the name of the project to appear in the navigation.
    'nav_title': project,

    # Set you GA account ID to enable tracking
    'google_analytics_account': 'UA-240922-2',

    # Specify a base_url used to generate sitemap.xml. If not
    # specified, then no sitemap will be built.
    'base_url': 'https://omnetpp.org',

    # Set the color and the accent color
    'color_primary': 'blue',
    'color_accent': 'light-blue',

    # Set the repo location to get a badge with stats
    'repo_url': 'https://github.com/omnetpp/omnetpp/',
    'repo_name': 'omnetpp',

    # Visible levels of the global TOC; -1 means unlimited
    'globaltoc_depth': 2,
    # If False, expand all TOC entries
    'globaltoc_collapse': True,
    # If True, show hidden TOC entries
    'globaltoc_includehidden': False,
}

# -- Options for Eclipse help output --------------------------------------------
eclipsehelp_basedir = 'content'

# -- Options for LaTeX/PDF output -----------------------------------------------
latex_engine = 'pdflatex'

# copy the background image for the LaTeX cover page
latex_additional_files = ['cover/cover-background.pdf']

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    ('userguide/index', 'UserGuide.tex', "User Guide", author, 'manual', False),
    ('ide-developersguide/ide-developersguide', 'IDE-DevelopersGuide.tex', "IDE Developer's Guide", author, 'manual', False),
    ('ide-customization-guide/ide-customization-guide', 'IDE-CustomizationGuide.tex', "IDE Customization Guide", author, 'manual', False),
    ('installguide/index', 'InstallGuide.tex', "Installation Guide", author, 'manual', False),
    ('ide-overview/ide-overview', 'IDE-Overview.tex', "IDE Overview", author, 'manual', False),
]

latex_elements = {
# The paper size ('letterpaper' or 'a4paper').
'papersize': 'a4paper',

# The font size ('10pt', '11pt' or '12pt').
'pointsize': '10pt',

'fontpkg': r"""
\usepackage{bookman}
""",

# Additional stuff for the LaTeX preamble.
'preamble': r"""
\PassOptionsToPackage{bookmarksnumbered}{hyperref}
\usepackage{anyfontsize}
\usepackage{eso-pic}
""",

'maketitle': string.Template(open('cover/cover.tex').read())
    .substitute(project = project, version = version, buildid = buildid, copyright = copyright),

'figure_align':'H',
}

# use :numref: for references (instead of :ref:)
numfig = True
smart_quotes = False

#######################################################xxx
# code customizations
from sphinx.application import Sphinx

def setup(app: Sphinx):
    app.add_config_value('what', 'omnetpp', 'env')
