# Minimal Sphinx project the doc-builder image's pdf stage builds to PDF at image
# build time, to prove the TeX Live / ImageMagick toolchain is complete.
# SMOKE_FONTS=omnetpp uses the OMNeT++ guides' font and preamble packages (see
# doc/src/conf.py); otherwise Sphinx's defaults are used, as in INET's docs.
import os

project = 'Smoke'
extensions = ['sphinx.ext.imgconverter']
latex_engine = 'pdflatex'
latex_documents = [('index', 'smoke.tex', 'Smoke', '', 'manual', False)]
latex_elements = {'papersize': 'a4paper'}
if os.environ.get('SMOKE_FONTS') == 'omnetpp':
    latex_elements['fontpkg'] = r'\usepackage{bookman}'
    latex_elements['preamble'] = r'''
\PassOptionsToPackage{bookmarksnumbered}{hyperref}
\usepackage{anyfontsize}
\usepackage{eso-pic}
'''
