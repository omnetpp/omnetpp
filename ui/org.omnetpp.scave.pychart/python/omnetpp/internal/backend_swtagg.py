"""
A custom backend for matplotlib that uses the matplotlib.backends.backend_agg
module for rasterization, and communicates with the OMNeT++ IDE through a Py4J
connection for display and interaction.

This file is based on:
 - matplotlib.backends.backend_template
 - matplotlib.backends.backend_qt5
 - matplotlib.backends.backend_qt5agg
"""

__copyright__ = "Copyright 2016-2020, OpenSim Ltd"
__license__ = """
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
"""

import os
import functools

from matplotlib.figure import Figure
from matplotlib.backend_bases import FigureManagerBase, FigureCanvasBase, KeyEvent, LocationEvent, NavigationToolbar2, MouseEvent, ResizeEvent
from matplotlib.backends.backend_agg import FigureCanvasAgg

from omnetpp.internal import Gateway
from omnetpp.internal.shmutil import shm_ctor_wrapper
# from omnetpp.internal.TimeAndGuard import TimeAndGuard, for_all_methods

# autoConvert doesn't seem to work with containers returned from Python
from py4j.java_collections import MapConverter, ListConverter


print = functools.partial(print, flush=True)


def new_figure_manager(num, *args, **kwargs):
    """
    Create a new figure manager instance
    """
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

        # The rest used to be: def _init_toolbar(self): (before matplotlib 3.3)

        # self.basedir = os.path.join(matplotlib.rcParams['datapath'], 'images')

        descs = []

        for text, tooltip_text, image_file, method_name in self.toolitems:
            desc = Gateway.gateway.jvm.org.omnetpp.scave.pychart.ActionDescription(
                text, tooltip_text, image_file, method_name)
            descs.append(desc)

        Gateway.widget_provider.setPlotActions(self.canvas.num, descs)

    def _init_toolbar(self):
        # for compatibility with matplotlib < 3.3
        pass

    def dynamic_update(self):
        self.canvas.dynamic_update()

    def callback(self, action):
        if hasattr(self, action):
            getattr(self, action)()
        else:
            print("unimplemented action: " + action)
        if hasattr(self, "_active"): # matplotlib < 3.3
            Gateway.widget_provider.updateActiveAction(self.canvas.num, self._active)
        else: # matplotlib >= 3.3
            Gateway.widget_provider.updateActiveAction(self.canvas.num, (self.mode._navigate_mode or "").lower())

    def set_message(self, s):
        if self.widget is not None:
            self.widget.setMessage(s)

    # for compatibility with MPL < 3.5
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

figure_counter = 1

# @for_all_methods(TimeAndGuard())
class FigureCanvasSWT(FigureCanvasBase):

    def __init__(self, figure):
        self.draw_enabled = False
        super().__init__(figure)
        self.figure = figure

        self.widget = Gateway.widget_provider.getWidget(self.num, self)

    def enterEvent(self, x, y):
        event = LocationEvent('figure_enter_event', self, x, y)
        self.callbacks.process(event.name, event)

    def leaveEvent(self):
        event = LocationEvent('figure_leave_event', self, None, None)
        self.callbacks.process(event.name, event)

    def mousePressEvent(self, x, y, button):
        event = MouseEvent('button_press_event', self, x, y, button)
        self.callbacks.process(event.name, event)

    def mouseDoubleClickEvent(self, x, y, button):
        event = MouseEvent('button_press_event', self, x, y, button, dblclick=True)
        self.callbacks.process(event.name, event)

    def mouseWheelEvent(self, x, y, count, mod1, mod2):
        button = "up" if count >= 0 else "down"

        if mod1:
            key = "ctrl+shift" if mod2 else "control"
        else:
            key = "shift" if mod2 else None

        event = MouseEvent('scroll_event', self, x, y, button, key, step=count)
        self.callbacks.process(event.name, event)

    def mouseMoveEvent(self, x, y):
        event = MouseEvent('motion_notify_event', self, x, y)
        self.callbacks.process(event.name, event)

    def mouseReleaseEvent(self, x, y, button):
        event = MouseEvent('button_release_event', self, x, y, button)
        self.callbacks.process(event.name, event)

    def keyPressEvent(self, key):
        event = KeyEvent('key_press_event', self, key)
        self.callbacks.process(event.name, event)

    def keyReleaseEvent(self, key):
        event = KeyEvent('key_release_event', self, key)
        self.callbacks.process(event.name, event)

    def _resize(self, width, height):
        dpival = self.figure.dpi
        winch = width / dpival
        hinch = height / dpival
        self.figure.set_size_inches(winch, hinch, forward=False)
        event = ResizeEvent('resize_event', self)
        self.callbacks.process(event.name, event)

    def resizeEvent(self, width, height):
        self._resize(width, height)
        self.draw_enabled = True
        if self.widget is not None:
            self.draw_idle()

    def setCursor(self, cursor):
        self.widget.setCursorType(cursor.name)

    # for compatibility with MPL >= 3.5
    def set_cursor(self, cursor):
        return self.setCursor(cursor)

    def drawRectangle(self, rect):
        if rect is None:
            self.widget.clearRect()
        else:
            self.widget.setRect(rect)

    def performAction(self, action):
        self.toolbar.callback(action)

    def exportFigure(self, filename):
        self.print_figure(filename)

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
    motion_notify_event, key_press_event, and key_release_event.

    Attributes
    ----------
    figure : `matplotlib.figure.Figure`
        A high-level Figure instance

    """

    class Java:
        implements = ["org.omnetpp.scave.pychart.IMatplotlibFigureCanvas"]


    def __init__(self, figure, num):
        self.num = num
        self.useSharedMemory = True
        self.shm = None

        super().__init__(figure)
        self._agg_draw_pending = False

        self.draw_idle()

    def __del__(self):
        if self.shm is not None:
            self.shm.close()
            self.shm.unlink()

    def getAxisLimits(self):
        limits = list()

        for ax in self.figure.axes:
            if ax.lines or ax.collections or ax.patches: # skip "empty" axes
                limits += [*ax.get_xlim(), *ax.get_ylim()]

        # we may get `np.float64` values, and `ListConverter` can't handle those
        limits = [float(x) for x in limits]
        java_list = ListConverter().convert(limits, Gateway.gateway._gateway_client)
        return java_list

    def setAxisLimits(self, limits):
        self.figure.canvas.toolbar.push_current()
        did_change = False
        for ax in self.figure.axes:
            if len(limits) < 4:
                break
            l = limits[:4]

            ax.set_xlim(l[0], l[1])
            ax.set_ylim(l[2], l[3])
            did_change = True
            limits = limits[4:]

        if did_change:
            self.figure.canvas.toolbar.push_current()

    def blit(self, bbox=None):
        # TODO use SHM here as well!
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

    def draw(self):

        w, h = self.get_width_height()

        FigureCanvasAgg.draw(self)

        buffer = self.buffer_rgba()

        # older matplotlib versions return a bytes object, newer ones return a memoryview
        if isinstance(buffer, bytes):
            bl = len(buffer)
        else:
            # it is assumed to be a memoryview
            bl = buffer.nbytes

        if self.useSharedMemory:
            if self.shm is None or self.shm.size < bl:
                global figure_counter
                # The & 0xFFFF operations are there to make sure that name is not longer than 31 bytes,
                # because macOS has silly limitations regarding this. The fixed part is 8 characters,
                # and each of the 3 numbers are limited to 5 digits, so we should be fine.
                name = "/plot-" + str(os.getpid() & 0xFFFF) + "-" + str(self.num & 0xFFFF) + "-" + str(figure_counter & 0xFFFF)
                figure_counter += 1

                oldshm = self.shm
                name, self.shm = shm_ctor_wrapper(name, create=True, size=bl)
                self.widget.setSharedMemoryNameAndSize(name, bl)

                if oldshm is not None:
                    oldshm.close()
                    oldshm.unlink()

            # casting the buffer to a flat array if it's a memoryview, else using it as is
            bc = buffer if isinstance(buffer, bytes) else buffer.cast("B", [bl])
            # shm object may be oversized (after the widget is made smaller), hence the [:bl]
            self.shm.buf[:bl] = bc[:]

            self.widget.setPixelsShared(w, h)
        else:
            self.widget.setPixels(buffer, w, h)


FigureCanvas = FigureCanvasSWTAgg
FigureManager = FigureManagerSWT
