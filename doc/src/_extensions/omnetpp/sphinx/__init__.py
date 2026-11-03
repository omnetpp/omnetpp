"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

    Assorted OMNeT++ directives, roles + eclipse help file builder.

"""

from os import path
from typing import Any, Dict
from sphinx.application import Sphinx
from sphinx.config import Config
from omnetpp.sphinx import builders
from omnetpp.sphinx import preprocessor

def setup(app: Sphinx) -> Dict[str, Any]:
    # config keys
    app.add_config_value('opp_replacements', {}, True)

    # set up the new builders and builder config keys
    app.add_config_value('eclipsehelp_basedir', '', 'eclipsehelp')
    app.add_config_value('eclipsehelp_theme', 'alabaster', 'eclipsehelp')
    app.add_config_value('eclipsehelp_theme_options', {}, 'eclipsehelp')
    app.setup_extension('sphinx.builders.html')
    app.add_builder(builders.EclipseHelpBuilder)

    # hook up event handlers
    app.connect('source-read', preprocessor.opp_preprocess)
    app.connect('include-read', preprocessor.opp_preprocess_include)

    return {
        'version': '6.0.0',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }

