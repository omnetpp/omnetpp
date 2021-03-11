"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

    Assorted OMNeT++ directives, roles + eclipse help file builder.

"""

from os import path
from typing import Any, Dict
from sphinx.application import Sphinx
from sphinx.highlighting import lexers
from omnetpp.sphinx import builders
from omnetpp.sphinx import video
from omnetpp.sphinx import audio
from omnetpp.sphinx import preprocessor
from omnetpp.sphinx import highlighters

def setup(app: Sphinx) -> Dict[str, Any]:
    # config values
    app.add_config_value('opp_replacements', {}, True)

    # set up the new builders
    app.add_config_value('eclipsehelp_basedir', '', 'eclipsehelp')
    app.setup_extension('sphinx.builders.html')
    app.add_builder(builders.EclipseHelpBuilder)

    # set up directives
    app.add_directive('youtube', video.Youtube)
    app.add_directive('vimeo', video.Vimeo)
    app.add_directive('video', video.Video)
    app.add_directive('audio', audio.Audio)
    
    # highlighters
    lexers['ned'] = highlighters.NedLexer(startinline=True)
    lexers['msg'] = highlighters.MsgLexer(startinline=True)
    lexers['ini'] = highlighters.IniLexer(startinline=True)

    # hook up the global preporcessor
    app.connect('source-read', preprocessor.opp_preprocess)

    return {
        'version': '6.0.0',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
