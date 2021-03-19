"""

    Build HTML documentation and EclipseHelp support files.
    (based on sphinxcontrib.devhelp extension)

    :copyright: Copyright 2007-2019 by the Sphinx team, see README.
    :license: BSD, see LICENSE for details.

"""

import re
from os import path
from typing import Any, Dict, Tuple

from docutils import nodes
from pygments.token import String
from sphinx import addnodes
from sphinx.application import Sphinx
from sphinx.builders.html import StandaloneHTMLBuilder
from sphinx.environment.adapters.indexentries import IndexEntries
from sphinx.locale import get_translation
from sphinx.util import logging
from sphinx.util.nodes import NodeMatcher
from sphinx.util.osutil import make_filename

try:
    import xml.etree.ElementTree as etree
except ImportError:
    import lxml.etree as etree  # type: ignore


if False:
    # For type annotation
    from typing import List  # NOQA


logger = logging.getLogger(__name__)
__ = get_translation(__name__, 'console')

package_dir = path.abspath(path.dirname(__file__))


class EclipseHelpBuilder(StandaloneHTMLBuilder):
    """
    Builder that also outputs toc.xml file for the Eclipse help system.
    """
    name = 'eclipsehelp'
    epilog = __('The Eclipse Help pages are in %(outdir)s.')

    # don't copy the reST source
    copysource = False
    supported_image_types = ['image/png', 'image/gif', 'image/jpeg']

    # don't add links
    add_permalinks = False
    # don't add sidebar etc.
    embedded = True

    def init(self):
        super().init()
        self.out_suffix = '.html'
        self.link_suffix = '.html'

    def get_theme_config(self) -> Tuple[str, Dict]:
        return self.config.eclipsehelp_theme, self.config.eclipsehelp_theme_options

    def handle_finish(self):
        self.build_eclipsehelp(self.outdir)

    def build_eclipsehelp(self, outdir: String):
        logger.info(__('dumping eclipse help toc.xml...'))

        base_dir = self.config.eclipsehelp_basedir
        if (base_dir != ''):
            base_dir += '/'

        root = etree.Element('toc',
                             label=self.config.html_title,
                             topic="index.html")
        tree = etree.ElementTree(root)

        tocdoc = self.env.get_and_resolve_doctree(
            self.config.master_doc, self, prune_toctrees=False)

        def write_toc(node: nodes.Node, parent: etree.Element):
            if isinstance(node, addnodes.compact_paragraph) or \
               isinstance(node, nodes.bullet_list):
                for subnode in node:
                    write_toc(subnode, parent)
            elif isinstance(node, nodes.list_item):
                item = etree.SubElement(parent, 'topic')
                for subnode in node:
                    write_toc(subnode, item)
            elif isinstance(node, nodes.reference):
                parent.attrib['label'] = node.astext()
                parent.attrib['href'] = base_dir + node['refuri']

        matcher = NodeMatcher(addnodes.compact_paragraph, toctree=Any)
        for node in tocdoc.traverse(matcher):  # type: addnodes.compact_paragraph
            write_toc(node, root)

        # Dump the XML file
        tocfile = path.join(outdir, 'toc.xml')
        with open(tocfile, 'wb') as f:
            tree.write(f, 'utf-8')
