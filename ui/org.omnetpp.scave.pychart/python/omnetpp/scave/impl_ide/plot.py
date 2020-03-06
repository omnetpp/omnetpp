import numpy as np
import pandas as pd
import math
import pickle as pl
import os
import platform
import matplotlib.colors as mplc

# posix_ipc is required for POSIX shm on Linux and Mac
if platform.system() in ['Linux', 'Darwin']:
    import posix_ipc
import mmap

import functools
print = functools.partial(print, flush=True)

from omnetpp.scave import results
from omnetpp.scave.utils import *
from omnetpp.internal import Gateway

import py4j
from py4j.java_collections import MapConverter

_vector_data_counter = 0

def _assert_is_native_chart():
    if not Gateway.chart_plotter:
        raise RuntimeError("This method can only be used on native charts")


def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    _assert_is_native_chart()
    props = {}
    if drawstyle:
        props["Line.DrawStyle"] = _translate_drawstyle(drawstyle)
    if linestyle:
        props["Line.Style"] = _translate_linestyle(linestyle)
    if linewidth:
        props["Line.Width"] = str(linewidth)
    if color:
        props["Line.Color"] = _translate_color(color)
    if marker:
        props["Symbols.Type"] = _translate_marker(marker)
    if markersize:
        props["Symbols.Size"] = str(markersize)

    plot_lines(pd.DataFrame({
        "key": [key],
        "label": [label],
        "xs": [np.array(xs)],
        "ys": [np.array(ys)]
        }
    ), props)


def bar(x, height, width=0.8, key=None, label=None, color=None, edgecolor=None):
    _assert_is_native_chart()
    props = {}

    if color:
        props["Bar.Color"] = _translate_color(color)

    plot_bars(pd.DataFrame(
        {
            "key": [key],
            "label": [str(label)],
            "values": [np.array(height)]
        }
    ), props)


# minvalue, maxvalue, underflows and overflows are NOT accepted by mpl.pyplot.hist
def hist(x, bins, density=False, weights=None, cumulative=False, bottom=None, histtype='stepfilled', color=None, key=None, label=None, linewidth=None,
         underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    _assert_is_native_chart()

    # check if we really got a precomputed histogram, using the trick documented for pyplot.hist
    if not np.array_equal(x, bins[:-1]):
        raise ValueError("Histogram computation is not performed.")

    props = {}

    props["Hist.Cumulative"] = str(cumulative)
    props["Hist.Density"] = str(density)

    props["Hist.Color"] = _translate_color(color)
    props["Hist.Bar"] = _translate_histtype(histtype)

    props["Bars.Baseline"] = str(bottom)

    plot_histograms(pd.DataFrame({
        "key": [key],
        "label": [label],
        "binedges": [np.array(bins)],
        "binvalues": [np.array(weights)],
        "underflows": [underflows],
        "overflows": [overflows],
        "min": [float(np.min(bins)) if math.isnan(minvalue) else minvalue],
        "max": [float(np.max(bins)) if math.isnan(maxvalue) else maxvalue]
    }), props)


def plot_lines(df, props = dict()):  # key, label, xs, ys
    _assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "xs", "ys"]):
        raise RuntimeError("Invalid DataFrame format in plot_lines")

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "key": row.key,
            "title": row.label,
            "xs": _put_array_in_shm(row.xs, shm_objs, mmap_objs),
            "ys": _put_array_in_shm(row.ys, shm_objs, mmap_objs)
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def plot_bars(df, props=dict()):
    # TODO: add check for one-layer indices? numbers-only data?
    _assert_is_native_chart()

    Gateway.chart_plotter.plotScalars(pl.dumps([
        {
            "key": row.key,
            "title": row.label,
            "values": _list_to_bytes(row.values),
        }
        for row in df.itertuples(index=False)
    ]), props)


def plot_histograms(df, props=dict()):  # key, label, binedges, binvalues, underflows, overflows, min, max
    _assert_is_native_chart()

    if sorted(list(df.columns)) != sorted(["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]):
        raise RuntimeError("Invalid DataFrame format in plot_histogram")

    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "key": row.key,
            "title": row.label,

            # this could be computed in Java as well, but just to make things simpler, we do it here
            "sumweights": float(np.sum(row.binvalues) + row.underflows + row.overflows),

            "edges": _list_to_bytes(row.binedges),
            "values": _list_to_bytes(row.binvalues),

            "underflows": float(row.underflows),
            "overflows": float(row.overflows),

            "min": float(row.min),
            "max": float(row.max),
        }
        for row in df.itertuples(index=False)
    ]), MapConverter().convert(props, Gateway.gateway._gateway_client))


def _list_to_bytes(l):
    """
    Internal. Encodes/serializes a list or `np.array` of numbers as
    8-byte floats (doubles) into an `np.array` of bytes.
    """
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def _put_array_in_shm(arr, shm_objs, mmap_objs):
    """
    Internal. Turns a `np.array` into a byte sequence, and writes it into
    a newly created (platform-specific) SHM object with the given name.
    Returns a space-separated string, holding the name and the size
    of the SHM object.
    """
    if arr.size == 0:
        return "<EMPTY> 0"

    global _vector_data_counter
    # they only need to start with a / on POSIX, but Windows doesn't seem to mind, so just putting it there always...
    name = "/vecdata-" + str(os.getpid()) + "-" + str(_vector_data_counter)
    _vector_data_counter += 1

    system = platform.system()

    if system in ['Linux', 'Darwin']:
        mem = posix_ipc.SharedMemory(name, posix_ipc.O_CREAT | posix_ipc.O_EXCL, size=arr.nbytes)

        shm_objs.append(mem)

        if system == 'Darwin':
            # for some reason we can't write to the shm fd directly on mac, only after mmap-ing it
            with mmap.mmap(mem.fd, length=mem.size) as mf:
                mf.write(arr.astype(np.dtype('>f8')).tobytes())
        else:
            with open(mem.fd, 'wb') as mf:
                arr.astype(np.dtype('>f8')).tofile(mf)

    elif system == 'Windows':
        # on Windows, the mmap module in itself provides shared memory functionality
        mm = mmap.mmap(-1, arr.nbytes, tagname=name)
        mmap_objs.append(mm)
        mm.write(arr.astype(np.dtype('>f8')).tobytes())
    else:
        raise RuntimeError("unsupported platform")

    return name + " " + str(arr.nbytes)


def _translate_drawstyle(drawstyle):
    #TODO accept *exactly* what mpl accepts
    mapping = {
        "default" : "linear",
        "steps" : "steps-post",
        "steps-pre" : "steps-pre",
        "steps-mid" : "steps-mid",
        "steps-post" : "steps-post",
    }
    if drawstyle not in mapping or mapping[drawstyle] is None:
        raise ValueError("Unrecognized drawstyle '{}'".format(drawstyle))
    return mapping[drawstyle]


def _translate_linestyle(linestyle):
    if linestyle in ["none", "solid", "dashed", "dashdot", "dotted"]:
        return linestyle
    mapping = {
        ' '  : "none",
        '-'  : "solid",
        '--' : "dashed",
        '-.' : "dashdot",
        ':'  : "dotted",
    }
    if linestyle not in mapping:
        raise ValueError("Unrecognized linestyle '{}'".format(linestyle))
    return mapping[linestyle]


def _translate_color(color):
    if color and len(color) >= 2 and color[:1] == "C" and color[1:].isdigit():
        return mplc.to_hex(color)

    mapping = {
        "b" : "blue",
        "g" : "green",
        "r" : "red",
        "c" : "cyan",
        "m" : "magenta",
        "y" : "yellow",
        "k" : "black",
        "w" : "white"
    }
    return mapping[color] if color in mapping else color


def _translate_marker(marker):
    mapping = {
        ' ' : "none",
        '.' : "dot",
        ',' : "point",
        'o' : "circle",
        'v' : "triangle_down",
        '^' : "triangle_up",
        '<' : "triangle_left",
        '>' : "triangle_right",
        '1' : "tri_down",
        '2' : "tri_up",
        '3' : "tri_left",
        '4' : "tri_right",
        '8' : "octagon",
        's' : "square",
        'p' : "pentagon",
        '*' : "star",
        'h' : None, # TODO "hexagon1",
        'H' : None, # TODO "hexagon2",
        '+' : "plus",
        'x' : "cross",
        'D' : "diamond",
        'd' : "thin_diamond",
        '|' : "vline",
        '_' : "hline"
    }
    if marker not in mapping or mapping[marker] is None:
        raise ValueError("Unrecognized marker '{}'".format(marker))
    return mapping[marker]


def _translate_histtype(histtype):
    mapping = {
        'step'       : "Outline",
        'stepfilled' : "Solid",
    }
    if histtype not in mapping:
        raise ValueError("Unrecognized histtype '{}'".format(histtype))
    return mapping[histtype]


def set_property(key, value):
    _assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperty(key, value)
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()

    if msg is not None:
        raise RuntimeError(msg)



def set_properties(props):
    _assert_is_native_chart()
    msg = None
    try:
        Gateway.chart_plotter.setProperties(dict(props))
    except py4j.protocol.Py4JJavaError as e:
        msg = e.java_exception.getMessage()


    if msg is not None:
        raise RuntimeError(msg)


def get_supported_property_keys():
    _assert_is_native_chart()
    return set(Gateway.chart_plotter.getSupportedPropertyKeys())


def set_warning(warning):
    # TODO maybe we need a common interface?
    if Gateway.widget_provider:
        Gateway.widget_provider.setWarning(warning)

    if Gateway.chart_plotter:
        Gateway.chart_plotter.setWarning(warning)

def title(label):
    """
    Sets plot title.
    """
    set_property("Plot.Title", label)

def xlabel(xlabel):
    """
    Sets the label of the X axis.
    """
    set_property("X.Axis.Title", xlabel)

def ylabel(ylabel):
    """
    Sets the label of the Y axis.
    """
    set_property("Y.Axis.Title", ylabel)

def xlim(left=None, right=None):
    """
    Sets the limits of the X axis.
    """
    if (left is not None):
        set_property("X.Axis.Min", str(left))
    if (right is not None):
        set_property("X.Axis.Max", str(right))

def ylim(bottom=None, top=None):
    """
    Sets the limits of the Y axis.
    """
    if (bottom is not None):
        set_property("Y.Axis.Min", str(bottom))
    if (top is not None):
        set_property("Y.Axis.Max", str(top))

def xscale(value):
    """
    Sets the scale of the X axis. Possible values are 'linear' and 'log'.
    """
    if value not in ["linear", "log"]:
        raise ValueError("scale='{}' is not supported, only 'linear' and 'log'".format(value))
    set_property("X.Axis.Log", "true" if value == "log" else "false")

def yscale(value):
    """
    Sets the scale of the Y axis. Possible values are 'linear' and 'log'.
    """
    if value not in ["linear", "log"]:
        raise ValueError("scale='{}' is not supported, only 'linear' and 'log'".format(value))
    set_property("Y.Axis.Log", "true" if value == "log" else "false")

def xticks(ticks=None, labels=None, rotation=0):
    Gateway.chart_plotter.setGroupTitles(labels)
    set_property("X.Label.RotateBy", str(-int(rotation))) # matplotlib and our native widgets have different ideas about label rotation

def grid(b=True, which="major"):
    if which not in ["major", "both"]:
        raise ValueError("which='{}' is not supported, only 'major', and 'both'".format(which))
    if not b:
        set_property("Axes.Grid", "None")
    elif which == "major":
        set_property("Axes.Grid", "Major")
    else:
        set_property("Axes.Grid", "All")

def legend(show=None, frameon=None, loc=None):
    if show is not None:
        set_property("Legend.Display", "true" if show else "false")

    if frameon is not None:
        set_property("Legend.Border", "true" if frameon else "false")

    if loc is not None:
        mapping = {
            "best": ("Inside", "NorthEast"),
            "upper right": ("Inside", "NorthEast"),
            "upper left": ("Inside", "NorthWest"),
            "lower left": ("Inside", "SouthWest"),
            "lower right": ("Inside", "SouthEast"),
            "right": ("Inside", "East"),
            "center left": ("Inside", "West"),
            "center right": ("Inside", "East"),
            "lower center": ("Inside", "South"),
            "upper center": ("Inside", "North"),
            #"center": unsupported
            "outside top left": ("Above", "West"),
            "outside top center": ("Above", "South"),
            "outside top right": ("Above", "East"),
            "outside bottom left": ("Below", "West"),
            "outside bottom center": ("Below", "North"),
            "outside bottom right": ("Below", "East"),
            "outside left top": ("Left", "North"),
            "outside left center": ("Left", "East"),
            "outside left bottom": ("Left", "South"),
            "outside right top": ("Right", "North"),
            "outside right center": ("Right", "West"),
            "outside right bottom": ("Right", "South")
        }
        if loc not in mapping:
            raise ValueError("loc='{}' is not recognized/supported".format(loc))

        (position, anchoring) = mapping[loc]
        set_property("Legend.Position", position)
        set_property("Legend.Anchoring", anchoring)

