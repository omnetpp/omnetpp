"""
This module is the interface for displaying data using the built-in plot widgets (i.e. without matplotlib).
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE", "no") == "yes":
    from omnetpp.scave.impl_ide import plot as impl
else:
    from .impl_charttool import plot as impl

import math


def plot_bars(df):
    return impl.plot_bars(**locals())


def plot_lines(df):
    return impl.plot_lines(**locals())


def plot_histogram_data(df):
    return impl.plot_histogram_data(**locals())


# TODO: rename row_label to group_label?
def plot_scalars(df_or_values, labels=None, row_label=None):
    """
    Takes a set of scalar type results and plots them as a "native" bar chart - using the
    IDE's built-in drawing widget.

    `df_or_values` can be in one of three different formats:

    - A `pd.DataFrame` with (at least) `value`, `module` and `name` columns,
        for example as returned by `results.get_scalars()`
    - A `pd.DataFrame` containing only the numeric values. This is what you
        might get for example from `pd.pivot_table()`.
        Each line in the dataframe will be in it's own bar group, similar to
        what `df.plot.bar()` would draw by default.
        The names in both the horizontal and vertical indices are used as
        labels in the legend.
    - A simple `list` or `np.array` of numbers. They will all be in a single group.

    The optional `labels` and `row_label` parameters are only used in the third
    case, as markers for the legend; and ignored in the first two cases.

    Returns `None`.
    """
    return impl.plot_scalars(**locals())


def plot_vector(label, xs, ys, key = None, props = dict()):
    """
    Plots a single vector as a "native" line chart - using the
    IDE's built-in drawing widget.

    # Parameters

    - **label**: A string that identifies this line on the plot. It will appear on the legend, so should be unique across invocations.
    - **xs**, **ys**: Lists or `np.array`s of numbers. Containing the X and Y coordinates (like time and value) of each point on the line,
      respectively. There is no requirement on either of these to hold positive, unique or monotonous values, so this method can be used to
      draw arbitrary scatter plots as well. They must be equal in length.

    Can be called repeatedly, each invocation adds a new line to the existing ones on the plot.

    Returns `None`.
    """
    return impl.plot_vector(**locals())

def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    props = {}
    if label:
        props["Line.Name"] = label
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

    return plot_vector(label, xs, ys, key, props)

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
        'o' : None, # TODO "circle",
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

def plot_vectors(df_or_list):
    """
    Takes a set of vector type results and plots them as a "native" line chart - using the
    IDE's built-in drawing widget.

    `df_or_values` can be in one of three different formats:

    - A `pd.DataFrame` with (at least) `vectime` and `vecvalue` columns,
        for example as returned by `results.get_vectors()`
    - A `pd.DataFrame` containing only the numeric values. If there is a `"time"`
      column, that will provide the X coordinates, otherwise an artificial sequencial
      X coordinate series will be generated. Every column (other than "time") will be
      a different line.
    - A `list` of 3-tuples: `(label, x, y)`, each element of the list describes
      a line. The first element of each tuple is a string, which will be the label
      of the line, while the second and third are lists or `ndarray` of numbers.
      In every given tuple, the second and third element must be the equal length,
      but this length van differ between individual tuples.

    There is no requirement on the inputs to hold positive, unique or monotonous values,
    so this method can be used to draw arbitrary scatter plots as well.

    Returns `None`.
    """
    return impl.plot_vectors(**locals())


# TODO document, allow multiple iso columns, selecting the "value" column,
# maybe sorting by a different column than xdata
def plot_scatter(df, xdata, iso_column=None):
    """
    Pivots a DataFrame of numeric results (like scalars, itervars or statistics fields)
    and plots them on a line type chart as a scatter plot. The data points can be optionally
    separated into multiple isolines.

    # Parameters

    - **df**: A DataFrame that holds the data to be pivoted.
    - **xdata** *(string)*: The name of a column in `df` which will provide the X coordinate for the data points
    - **iso_column** *(string)*: Optional. The name of a column in `df` whose the values will be used to separate
      the data points into multiple isolines.

    Returns `None`.
    """
    return impl.plot_scatter(**locals())


def plot_histogram(label, binedges, binvalues, underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    """
    Plots a single histogram as a "native" chart - using the
    IDE's built-in drawing widget.

    # Parameters

    - **label**: A string that identifies this line on the plot. It will appear on the legend, so should be unique across invocations.
    - **binedges**, **binvalues**: Lists or `np.array`s of numbers. Containing the bin edges and the values of the histogram,
      respectively. `edges` should be one element longer than `values.
    - **underflows**, **overflows**: Optional. The sum of weights of the collected values that fell under or over the histogram bin range, respectively.
    - **minvalue**, **maxvalue**: Optional. The smallest and the highest collected value, respectively.

    Can be called repeatedly, each invocation adds a new histogram to the existing ones on the plot.

    Returns `None`.
    """
    return impl.plot_histogram(**locals())


def plot_histograms(df):
    """
    Takes a set of histogram type results and plots them as a "native" chart - using the
    IDE's built-in drawing widget.

    `df` is a DataFrame, containing (at least) `binedges` and `binvalues` columns,
    for example as returned by `results.get_histograms()`

    Returns `None`.
    """
    return impl.plot_histograms(**locals())


def set_property(key, value):
    """
    Sets one property of the chart to the given value. If there was no property with the given
    name (key) yet, it is added.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. It is not persistent and is not reflected
    in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_property(**locals())


def set_properties(props):
    """
    Updates or adds any number of properties of the chart with the values given in `props.
    Any existing property values will be overwritten, and any new keys will be inserted.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. This change is not persistent and is not
    reflected in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_properties(**locals())


def set_warning(warning):
    """
    Displays the given warning text in the plot.
    """
    impl.set_warning(**locals())

def title(str):
    """
    Sets plot title.
    """
    set_property("Plot.Title", str)

def xlabel(str):
    """
    Sets the label of the X axis.
    """
    set_property("X.Axis.Title", str)

def ylabel(str):
    """
    Sets the label of the Y axis.
    """
    set_property("Y.Axis.Title", str)

def xlim(left=None, right=None):
    """
    Sets the limits of the X axis.
    """
    if (left is not None):
        set_property("X.Axis.Min", str(left))
    if (right is not None):
        set_property("X.Axis.Max", str(right))

def ylim(left=None, right=None):
    """
    Sets the limits of the Y axis.
    """
    if (left is not None):
        set_property("Y.Axis.Min", str(left))
    if (right is not None):
        set_property("Y.Axis.Max", str(right))

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

