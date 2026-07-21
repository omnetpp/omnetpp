# -*- coding: utf-8 -*-
"""
    ReST directive for embedding local, Youtube and Vimeo videos.

    There are two directives added: ``video``, ``youtube`` and ``vimeo``. The only
    argument is the video id or local URL of the video to include.

    All directives have three optional arguments: ``height``, ``width``
    and ``align``.

    Example::

        .. youtube:: anwy2MPT5RE
            :height: 315
            :width: 560
            :align: left

    :copyright: (c) 2012 by Danilo Bargen
    :license: BSD 3-clause
    :url: https://gist.github.com/dbrgn/2922648

    Modified by Martin Landa
    Modified by Rudolf Hornig
"""
from __future__ import absolute_import
from docutils import nodes
from docutils.parsers.rst import Directive, directives


def align(argument):
    """Conversion function for the "align" option."""
    return directives.choice(argument, ('left', 'center', 'right'))


class VideoBase(Directive):
    has_content = True
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = False
    option_spec = {
        'height': directives.unchanged,
        'width': directives.unchanged,
        'align': align,
    }

    def run(self):
        env = self.state.document.settings.env
        builder_name = env.app.builder.name

        self.options['video_id'] = directives.uri(self.arguments[0])
        if not self.options.get('width'):
            self.options['width'] = ''
        if not self.options.get('height'):
            self.options['height'] = ''
        if not self.options.get('align'):
            self.options['align'] = 'center'
        if not self.options.get('caption'):
            self.options['caption'] = ''.join(self.content)

        if builder_name == 'latex':
            return [nodes.raw('', self.latex % self.options, format='latex')]

        return [nodes.raw('', self.html % self.options, format='html')]


# FIXME default width/height and alignement is not working!
# FIXME video files must be copied to the destination dir too.

class Youtube(VideoBase):
    html = '<div style="text-align: %(align)s"><iframe src="http://www.youtube.com/embed/%(video_id)s" \
    width="%(width)s" height="%(height)s" frameborder="0" \
    webkitAllowFullScreen mozallowfullscreen allowfullscreen \
    class="align-%(align)s"></iframe><div style="margin-top: 5px; margin-bottom: 5px"><i>%(caption)s</i></div></div>'
    latex = '\\vskip 0.5em \\textbf{YouTube -- %(caption)s} \\newline \\url{http://www.youtube.com/embed/%(video_id)s}\\vskip 0.5em'

class Vimeo(VideoBase):
    html = '<iframe src="http://player.vimeo.com/video/%(video_id)s" \
    width="%(width)s" height="%(height)s" frameborder="0" \
    webkitAllowFullScreen mozallowfullscreen allowFullScreen \
    class="align-%(align)s"></iframe>'

class Video(VideoBase):
    html = '<video autoplay loop controls onclick="this.paused ? this.play() : this.pause();" \
    src="%(video_id)s" width="%(width)s" height="%(height)s" class="align-%(align)s"></video>'
    latex = '\\vskip 0.5em \\textbf{Video -- %(caption)s} \\vskip 0.5em'
    
class Video_noloop(VideoBase):
    html = '<video autoplay controls onclick="this.paused ? this.play() : this.pause();" \
    src="%(video_id)s" width="%(width)s" height="%(height)s" class="align-%(align)s"></video>'
    latex = '\\vskip 0.5em \\textbf{Video -- %(caption)s} \\vskip 0.5em'
