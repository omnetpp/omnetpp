import math
import numpy as np
import pandas as pd
import matplotlib.colors as mplc

import functools
print = functools.partial(print, flush=True)

import omnetpp.scave.impl_ide.swtplot as swtplot

set_property = swtplot.set_property
set_properties = swtplot.set_properties
get_supported_property_keys = swtplot.get_supported_property_keys
set_warning = swtplot.set_warning


def plot(xs, ys, key, label, drawstyle, linestyle, linewidth, color, marker, markersize):
    swtplot.assert_is_native_chart()
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

    swtplot.plot_lines(pd.DataFrame({
        "key": [key],
        "label": [label],
        "xs": [np.array(xs)],
        "ys": [np.array(ys)]
        }
    ), props)


def bar(x, height, width, key, label, color, edgecolor):
    swtplot.assert_is_native_chart()
    props = {}

    if color:
        props["Bar.Color"] = _translate_color(color)

    swtplot.plot_bars(pd.DataFrame(
        {
            "key": [key],
            "label": [str(label)],
            "values": [np.array(height)]
        }
    ), props)


# minvalue, maxvalue, underflows and overflows are NOT accepted by mpl.pyplot.hist
def hist(x, bins, density, weights, cumulative, bottom, histtype, color, key, label, linewidth,
         underflows, overflows, minvalue, maxvalue):
    swtplot.assert_is_native_chart()

    # check if we really got a precomputed histogram, using the trick documented for pyplot.hist
    if not np.array_equal(x, bins[:-1]):
        raise ValueError("Only precomputed histograms are accepted: the values in `x` must equal the values in `bins`, without the last one.")

    if weights is None or len(weights) != len(x):
        raise ValueError("The `weights` parameter must not be omitted, and it must have the same number of elements as `x`")

    props = {}

    props["Hist.Cumulative"] = str(cumulative)
    props["Hist.Density"] = str(density)

    props["Hist.Color"] = _translate_color(color)
    props["Hist.Bar"] = _translate_histtype(histtype)

    props["Bars.Baseline"] = str(bottom)

    swtplot.plot_histograms(pd.DataFrame({
        "key": [key],
        "label": [label],
        "binedges": [np.array(bins)],
        "binvalues": [np.array(weights)],
        "underflows": [underflows],
        "overflows": [overflows],
        "min": [float(np.min(bins)) if math.isnan(minvalue) else minvalue],
        "max": [float(np.max(bins)) if math.isnan(maxvalue) else maxvalue]
    }), props)

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

def xlim(left, right):
    """
    Sets the limits of the X axis.
    """
    if (left is not None):
        set_property("X.Axis.Min", str(left))
    if (right is not None):
        set_property("X.Axis.Max", str(right))

def ylim(bottom, top):
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

def xticks(ticks, labels, rotation):
    swtplot.set_group_titles(labels)
    set_property("X.Label.RotateBy", str(-int(rotation))) # matplotlib and our native widgets have different ideas about label rotation

def grid(b, which):
    # This does not work exactly like `matplotlib.pyplot.grid`.
    # There, `which` selects a target set of grid lines to turn on or off,
    # while here, it acts as a modifier if `b` is `True`.
    if which not in ["major", "both"]:
        raise ValueError("which='{}' is not supported, only 'major', and 'both'".format(which))
    if not b:
        set_property("Axes.Grid", "None")
    elif which == "major":
        set_property("Axes.Grid", "Major")
    else:
        set_property("Axes.Grid", "All")

def legend(show, frameon, loc):
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
