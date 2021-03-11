# -*- coding: utf-8 -*-
"""
    ReST directive for embedding audio content.

    The directive has a single optional argument: ``align``.

    Example::

        .. audio:: myfile.wav
           :align: center

"""
from __future__ import absolute_import
from docutils import nodes
from docutils.parsers.rst import Directive, directives


def align(argument):
    """Conversion function for the "align" option."""
    return directives.choice(argument, ('left', 'center', 'right'))


class Audio(Directive):
    has_content = True
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = False
    option_spec = {
        'align': align,
    }

    def run(self):
        env = self.state.document.settings.env
        builder_name = env.app.builder.name

        self.options['audio_url'] = directives.uri(self.arguments[0])
        if not self.options.get('align'):
            self.options['align'] = 'center'
        if not self.options.get('caption'):
            self.options['caption'] = ''.join(self.content)

        if builder_name == 'latex':
            latex = '\\vskip 0.5em \\textbf{Audio -- %(caption)s} \\vskip 0.5em'
            return [nodes.raw('', latex % self.options, format='latex')]

        html = '<audio controls class="align-%(align)s" src="%(audio_url)s">Audio -- %(caption)s</audio>'
        return [nodes.raw('', html % self.options, format='html')]
