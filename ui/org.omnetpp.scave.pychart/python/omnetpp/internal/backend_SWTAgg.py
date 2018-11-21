"""
This is a fully functional do nothing backend to provide a SWTAgg to
backend writers.  It is fully functional in that you can select it as
a backend with

  import matplotlib
  matplotlib.use('SWTAgg')

and your matplotlib scripts will (should!) run without error, though
no output is produced.  This provides a nice starting point for
backend writers because you can selectively implement methods
(draw_rectangle, draw_lines, etc...) and slowly see your figure come
to life w/o having to have a full blown implementation before getting
any results.

Copy this to backend_xxx.py and replace all instances of 'SWTAgg'
with 'xxx'.  Then implement the class methods and functions below, and
add 'xxx' to the switchyard in matplotlib/backends/__init__.py and
'xxx' to the backends list in the validate_backend methon in
matplotlib/__init__.py and you're off.  You can use your backend with::

  import matplotlib
  matplotlib.use('xxx')
  from pylab import *
  plot([1,2,3])
  show()

matplotlib also supports external backends, so you can place you can
use any module in your PYTHONPATH with the syntax::

  import matplotlib
  matplotlib.use('module://my_backend')

where my_backend.py is your module name.  This syntax is also
recognized in the rc file and in the -d argument in pylab, e.g.,::

  python simple_plot.py -dmodule://my_backend

If your backend implements support for saving figures (i.e. has a print_xyz()
method) you can register it as the default handler for a given file type

  from matplotlib.backend_bases import register_backend
  register_backend('xyz', 'my_backend', 'XYZ File Format')
  ...
  plt.savefig("figure.xyz")

The files that are most relevant to backend_writers are

  matplotlib/backends/backend_your_backend.py
  matplotlib/backend_bases.py
  matplotlib/backends/__init__.py
  matplotlib/__init__.py
  matplotlib/_pylab_helpers.py

Naming Conventions

  * classes Upper or MixedUpperCase

  * varables lower or lowerUpper

  * functions lower or underscore_separated

"""

from __future__ import (absolute_import, division, print_function,
                        unicode_literals)

import six

import matplotlib
from matplotlib._pylab_helpers import Gcf
from matplotlib.backend_bases import RendererBase, GraphicsContextBase, \
     FigureManagerBase, FigureCanvasBase, NavigationToolbar2
from matplotlib.figure import Figure
from matplotlib.transforms import Bbox

from matplotlib.cbook import CallbackRegistry

from matplotlib.backends.backend_agg import FigureCanvasAgg

from matplotlib.backend_bases import cursors

import os
import sys
import time
import traceback
import numpy as np  # mostly for RLE

from omnetpp.internal import Gateway
from omnetpp.internal.TimeAndGuard import TimeAndGuard, for_all_methods

import functools
print = functools.partial(print, flush=True)

# autoConvert doesn't seem to work with containers returned from Python
from py4j.java_collections import MapConverter, ListConverter

########################################################################
#
# The following functions and classes are for pylab and implement
# window/figure managers, etc...
#
########################################################################


def draw_if_interactive():
    """
    For image backends - is not required
    For GUI backends - this should be overriden if drawing should be done in
    interactive python mode
    """

    # for manager in Gcf.get_all_fig_managers():
    #    # do something to display the GUI
    #    manager.canvas.draw()


def show():
    """
    For image backends - is not required
    For GUI backends - show() is usually the last line of a pylab script and
    tells the backend that it is time to draw.  In interactive mode, this may
    be a do nothing func.  See the GTK backend for an example of how to handle
    interactive versus batch mode
    """
    # for manager in Gcf.get_all_fig_managers():
    #    # do something to display the GUI
    #    manager.canvas.draw()
    #


def new_figure_manager(num, *args, **kwargs):
    """
    Create a new figure manager instance
    """
    # if a main-level app must be created, this (and
    # new_figure_manager_given_figure) is the usual place to
    # do it -- see backend_wx, backend_wxagg and backend_tkagg for
    # examples.  Not all GUIs require explicit instantiation of a
    # main-level app (egg backend_gtk, backend_gtkagg) for pylab
    FigureClass = kwargs.pop('FigureClass', Figure)
    thisFig = FigureClass(*args, **kwargs)
    return new_figure_manager_given_figure(num, thisFig)


def new_figure_manager_given_figure(num, figure):
    """
    Create a new figure manager instance for the given figure.
    """
    canvas = FigureCanvasSWTAgg(figure, num)
    manager = FigureManagerSWT(canvas, num)
    return manager


class NavigationToolbar2SWT(NavigationToolbar2):

    def __init__(self, canvas):
        self.canvas = canvas
        self.widget = canvas.widget
        NavigationToolbar2.__init__(self, canvas)

    def _init_toolbar(self):
        # self.basedir = os.path.join(matplotlib.rcParams['datapath'], 'images')

        descs = []

        for text, tooltip_text, image_file, method_name in self.toolitems:
            desc = Gateway.gateway.jvm.org.omnetpp.scave.pychart.ActionDescription(
                text, tooltip_text, image_file, method_name)
            descs.append(desc)

        Gateway.widget_provider.setPlotActions(self.canvas.num, descs);

    def edit_parameters(self):
        allaxes = self.canvas.figure.get_axes()

    def dynamic_update(self):
        self.canvas.dynamic_update()

    def callback(self, action):
        if hasattr(self, action):
            getattr(self, action)()
        else:
            print("unimplemented action: " + action)
        Gateway.widget_provider.updateActiveAction(self.canvas.num, self._active)

    def set_message(self, s):
        if self.widget is not None:
            self.widget.setMessage(s)

    def set_cursor(self, cursor):
        self.canvas.setCursor(cursor)

    def draw_rubberband(self, event, x0, y0, x1, y1):
        height = self.canvas.figure.bbox.height
        y1 = height - y1
        y0 = height - y0
        rect = [int(val) for val in (x0, y0, x1 - x0, y1 - y0)]
        self.canvas.drawRectangle(rect)

    def remove_rubberband(self):
        self.canvas.drawRectangle(None)


class FigureManagerSWT(FigureManagerBase):
    """
    Not implementing most of the methods, because we don't let
    matplotlib control anything about the widget (size, title, etc.).
    """

    def __init__(self, canvas, num):
        self.canvas = canvas
        self.toolbar = NavigationToolbar2SWT(canvas)
        self.canvas.toolbar = self.toolbar

        FigureManagerBase.__init__(self, canvas, num)

    def show(self):
        # TODO override with pass just to avoid the exception, or actually redraw the figure/widget?
        pass


# By Thomas Browne, Taken from http://techqa.info/programming/question/1066758/find-length-of-sequences-of-identical-values-in-a-numpy-array
def rle3(inarray):
        """ run length encoding. Partial credit to R rle function.
            Multi datatype arrays catered for including non Numpy
            returns: tuple (runlengths, startpositions, values) """
        ia = np.array(inarray)  # force numpy
        n = len(ia)
        if n == 0:
            return np.array([0, 0])
        else:
            y = np.array(ia[1:] != ia[:-1])  # pairwise unequal (string safe)
            i = np.append(np.where(y), n - 1)  # must include last element posi
            z = np.diff(np.append(-1, i))  # run lengths
            v = ia[i]  # values

            l = np.empty((z.size + v.size,), dtype=v.dtype)
            l[0::2] = z
            l[1::2] = v
            return l


# @for_all_methods(TimeAndGuard())
class FigureCanvasSWT(FigureCanvasBase):

    def __init__(self, figure):
        self.draw_enabled = False
        super().__init__(figure)
        self.figure = figure

        w, h = self.get_width_height()

        self.widget = None

        self.widget = Gateway.widget_provider.getWidget(self.num, self)

    def setWidget(self, w):
        self.widget = w

    def enterEvent(self):
        FigureCanvasBase.enter_notify_event(self)

    def leaveEvent(self):
        FigureCanvasBase.leave_notify_event(self)

    def mouseEventCoords(self, pos):
        dpi_ratio = self._dpi_ratio
        x = pos.x()
        # flip y so y=0 is bottom of canvas
        y = self.figure.bbox.height / dpi_ratio - pos.y()
        return x * dpi_ratio, y * dpi_ratio

    def mousePressEvent(self, x, y, button):
        FigureCanvasBase.button_press_event(self, x, y, button)

    def mouseDoubleClickEvent(self, x, y, button):
        FigureCanvasBase.button_press_event(self, x, y, button, dblclick=True)

    def mouseMoveEvent(self, x, y):
        FigureCanvasBase.motion_notify_event(self, x, y)

    def mouseReleaseEvent(self, x, y, button):
        FigureCanvasBase.button_release_event(self, x, y, button)

    def wheelEvent(self, event):
        x, y = self.mouseEventCoords(event)
        # from QWheelEvent::delta doc
        if event.pixelDelta().x() == 0 and event.pixelDelta().y() == 0:
            steps = event.angleDelta().y() / 120
        else:
            steps = event.pixelDelta().y()
        if steps:
            FigureCanvasBase.scroll_event(self, x, y, steps, guiEvent=event)

    def keyPressEvent(self, event):
        FigureCanvasBase.key_press_event(self, key)

    def keyReleaseEvent(self, event):
        FigureCanvasBase.key_release_event(self, key)

    def _resize(self, width, height):
        dpival = self.figure.dpi
        winch = width / dpival
        hinch = height / dpival
        self.figure.set_size_inches(winch, hinch, forward=False)
        FigureCanvasBase.resize_event(self)

    def resizeEvent(self, width, height):
        self._resize(width, height)
        self.draw_enabled = True
        if self.widget is not None:
            self.draw_idle()

    def setCursor(self, cursor):
        self.widget.setCursorType(cursor)

    def drawRectangle(self, rect):
        if rect is None:
            self.widget.clearRect()
        else:
            self.widget.setRect(rect)

    def performAction(self, action):
        self.toolbar.callback(action)

    def exportFigure(self, filename):
        try:
            self.print_figure(filename)
        except Exception as e:
            print("EXC:" + str(e))
            return str(e)
        return ""

    def getSupportedFiletypes(self):
        types = self.get_supported_filetypes_grouped()
        converted = {}
        for k, v in types.items():
            converted[k] = ListConverter().convert(v, Gateway.gateway._gateway_client)

        return MapConverter().convert(converted, Gateway.gateway._gateway_client)

    def getDefaultFiletype(self):
        return self.get_default_filetype()


class FigureCanvasSWTAgg(FigureCanvasSWT, FigureCanvasAgg):
    """
    The canvas the figure renders into.  Calls the draw and print fig
    methods, creates the renderers, etc...

    Note GUI SWTAggs will want to connect events for button presses,
    mouse movements and key presses to functions that call the base
    class methods button_press_event, button_release_event,
    motion_notify_event, key_press_event, and key_release_event.  See,
    e.g., backend_gtk.py, backend_wx.py and backend_tkagg.py

    Attributes
    ----------
    figure : `matplotlib.figure.Figure`
        A high-level Figure instance

    """

    def __init__(self, figure, num):
        self.num = num

        self.useRle = True
        self.halfResDynamic = False

        super().__init__(figure)
        self._agg_draw_pending = False

        self.draw_idle()

    class Java:
        implements = ["org.omnetpp.scave.pychart.IPyFigureCanvas"]

    def setUseRle(self, enabled):
        self.useRle = enabled

    def setHalfResDynamic(self, enabled):
        self.halfResDynamic = enabled

    def draw_idle(self):
        # TODO: make this asynchronous, and group consequent calls to just one redraw
        if (self.draw_enabled):
            self.draw()

    def blit(self, bbox=None):
        # TODO: use RLE? halfres? any other optimizations?
        if bbox is None and self.figure:
            bbox = self.figure.bbox

        l, b, r, t = bbox.extents

        l = int(l)
        w = int(r) - int(l)
        h = int(t) - int(b)
        t = int(b) + h

        reg = self.copy_from_bbox(bbox)
        pixelBuffer = reg.to_string_argb()

        self.widget.blit(pixelBuffer, l, int(self.renderer.height - t), w, h)

    def print_figure(self, *args, **kwargs):
        FigureCanvasAgg.print_figure(self, *args, **kwargs)
        self.draw()

    def dynamic_update(self):
        if self.halfResDynamic:
            width, height = self.get_width_height()
            dpi = self.figure.dpi

            # resizing the agg canvas to half size and dpi
            self.figure.dpi = dpi / 2
            self._resize(width / 2, height / 2)

            self.draw(half=True)

            # restoring the agg canvas to original size and dpi
            self.figure.dpi = dpi
            self._resize(width, height)
        else:  # full res dynamic updates
            self.draw(half=False)

    def draw(self, half=False):

        w, h = self.get_width_height()

        # print("rasterizing with Agg at " + str(w) + "x" + str(h) + " and halfRes? " + str(half) + " ...")
        # startTime = time.perf_counter()
        FigureCanvasAgg.draw(self)
        # endTime = time.perf_counter()
        # print("rasterizing with Agg took " + str((endTime - startTime) * 1000.0) + " ms")

        pixelBuffer = self.buffer_rgba()
        bytes = pixelBuffer

        # doing the rle coding in 4byte units, so all colors will be compressed
        # well, not only those where r = b = g = a (those are rare)

        # f = open('output.rgba', 'wb')
        # f.write(stringBuffer)

        if self.useRle:
            # rleStart = time.perf_counter()

            intPixels = np.frombuffer(pixelBuffer, dtype=np.uint8)

            dt = np.dtype(np.int32)
            dt = dt.newbyteorder('>')

            rleInts = rle3(intPixels.view(dtype=dt))
            # TODO: could refrain from sending it with RLE encoding if we find
            # that it got bigger (or just a tiny bit smaller)
            bytes = rleInts.view(dtype=np.int8).tobytes()

            # rleEnd = time.perf_counter()
            # print("RLE encoding took " + str((rleEnd - rleStart) * 1000) + " ms.")

        self.widget.setPixels(bytes, w, h, self.useRle, half)


FigureCanvas = FigureCanvasSWTAgg
FigureManager = FigureManagerSWT
