"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

    Assorted OMNeT++ directives, roles + eclipse help file builder.

"""

from os import path
from typing import Any, Dict
from sphinx.application import Sphinx
from sphinx.config import Config
from sphinx.highlighting import lexers
from omnetpp.sphinx import builders
from omnetpp.sphinx import video
from omnetpp.sphinx import audio
from omnetpp.sphinx import preprocessor
from omnetpp.sphinx import highlighters

def setup(app: Sphinx) -> Dict[str, Any]:
    # config keys
    app.add_config_value('opp_replacements', {}, True)

    # set up the new builders and builder config keys
    app.add_config_value('eclipsehelp_basedir', '', 'eclipsehelp')
    app.add_config_value('eclipsehelp_theme', 'alabaster', 'eclipsehelp')
    app.add_config_value('eclipsehelp_theme_options', {}, 'eclipsehelp')
    app.setup_extension('sphinx.builders.html')
    app.add_builder(builders.EclipseHelpBuilder)

    # set up directives
    app.add_directive('youtube', video.Youtube)
    app.add_directive('vimeo', video.Vimeo)
    app.add_directive('video', video.Video)
    app.add_directive('audio', audio.Audio)

    # highlighters
    app.add_lexer('ned', highlighters.NedLexer)
    app.add_lexer('msg', highlighters.MsgLexer)
    app.add_lexer('ini', highlighters.IniLexer)

    # hook up event handlers
    app.connect('source-read', preprocessor.opp_preprocess)

    return {
        'version': '6.0.0',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }

