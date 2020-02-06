import numpy as np
import pandas as pd
import math
import pickle as pl
import os
import platform

# posix_ipc is required for POSIX shm on Linux and Mac
if platform.system() in ['Linux', 'Darwin']:
    import posix_ipc
import mmap

import functools
from math import inf
print = functools.partial(print, flush=True)

from omnetpp.scave import results, chart, plot
from omnetpp.internal import Gateway

from omnetpp.scave.utils import *

from py4j.java_collections import MapConverter

vector_data_counter = 0

def _assert_is_native_chart():
    if not Gateway.chart_plotter:
        raise RuntimeError("This method can only be used on native charts")

def _list_to_bytes(l):
    """
    Internal. Encodes/serializes a list or `np.array` of numbers as
    8-byte floats (doubles) into an `np.array` of bytes.
    """
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


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


def hist(x, bins, density=None, weights=None, cumulative=False, bottom=None, histtype='bar', color=None, label=None, linewidth=None):
    if not np.array_equal(x, bins[:-1]):
        raise ValueError("TODO")
    plot_histogram(label, bins, weights)


def bar(x, height, width=0.8, label=None):
    return plot_bars(pd.DataFrame({label: height}))

def plot_bars(df):
    # TODO: add check for one-layer indices? numbers-only data?
    _assert_is_native_chart()
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": [_to_label(c) for c in list(df.columns)],
            "rowKeys": [_to_label(i) for i in list(df.index)],
            "values": _list_to_bytes(df.values.flatten('F'))
        }
    ))


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


def plot_histogram_data(df):  # key, label, binedges, binvalues, underflows, overflows, min, max
    _assert_is_native_chart()
    if sorted(list(df.columns)) != sorted(["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]):
        raise RuntimeError("Invalid DataFrame format in plot_histogram_data")

    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "key": row.key,
            "title": row.label,
            "sumweights": float(np.sum(row.binvalues) + row.underflows + row.overflows),  # TODO remove?

            "edges": _list_to_bytes(row.binedges),
            "values": _list_to_bytes(row.binvalues),

            "underflows": float(row.underflows),
            "overflows": float(row.overflows),

            "min": float(row.min),
            "max": float(row.max),
        }
        for row in df.itertuples(index=False)
    ]))


def _to_label(x):
    """
    Internal. Turns various types of objects into a string, so they can be used as labels.
    """
    if x is None:
        return ""
    elif isinstance(x, str):
        return x
    elif isinstance(x, tuple):
        return ", ".join(map(str, list(x)))
    elif isinstance(x, list):
        return ", ".join(map(str, x))
    else:
        return str(x)


def _plot_scalars_lists(row_label, labels, values):
    if labels and len(labels) != len(values):
        raise RuntimeError("Not the same number of labels as values!")
    rows = [row_label if row_label else ""],
    columns = labels if labels else [""] * len(values),

    df = pd.DataFrame({row_label : values}, index=labels).transpose()

    plot_bars(df)


def _plot_scalars_DF_simple(df):
    plot_bars(df)
    # TODO: detect single-valued index/ column header levels, drop them


def _plot_scalars_DF_scave(df):
    # TODO: pivot in the other direction (transposed)?
    # TODO: this is... wrong?
    _plot_scalars_DF_simple(pd.pivot_table(df, index="module", columns="name", values="value"))


# This method only does dynamic dispatching based on its first argument.
def plot_scalars(df_or_values, labels=None, row_label=None):
    _assert_is_native_chart()
    if isinstance(df_or_values, pd.DataFrame):
        df = df_or_values
        if "value" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_scalars_DF_scave(df)
        else:
            _plot_scalars_DF_simple(df)
    else:
        _plot_scalars_lists(row_label, labels, df_or_values)


def _put_array_in_shm(arr, shm_objs, mmap_objs):
    """
    Internal. Turns a `np.array` into a byte sequence, and writes it into
    a newly created (platform-specific) SHM object with the given name.
    Returns a space-separated string, holding the name and the size
    of the SHM object.
    """
    if arr.size == 0:
        return "<EMPTY> 0"

    global vector_data_counter
    # they only need to start with a / on POSIX, but Windows doesn't seem to mind, so just putting it there always...
    name = "/vecdata-" + str(os.getpid()) + "-" + str(vector_data_counter)
    vector_data_counter += 1

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

def plot_vector(label, xs, ys, key=None, props = dict()):
    _assert_is_native_chart()
    if key is None:
        key = label
    plot_lines(pd.DataFrame({
        "key": [key],
        "label": [label],
        "xs": [np.array(xs)],
        "ys": [np.array(ys)]
        }
    ), props)


def _plot_vectors_tuplelist(vectors):
    df = pd.DataFrame.from_records(vectors, columns=["label", "xs", "ys"])
    df["key"] = [str(i) for i in range(len(vectors))]
    df["xs"] = df["xs"].map(np.array)
    df["ys"] = df["ys"].map(np.array)
    plot_lines(df)


def _plot_vectors_DF_simple(df):
    xcolumn = None
    for c in ["x", "xs", "time"]:
        if c in df:
            xcolumn = c
            break
    xs = df[xcolumn] if xcolumn else np.array(range(len(df.index)))

    df2 = pd.DataFrame.from_records([(
            str(i),
            str(column),
            np.array(xs[~np.isnan(df[column])]),
            np.array(df[column].values[~np.isnan(df[column])])
        )
        for i, column in enumerate(df) if column != xcolumn
    ], columns=["key", "label", "xs", "ys"])

    plot_lines(df2)


def _plot_vectors_DF_scave(df):
    title_col, legend_cols = extract_label_columns(df)

    df2 = pd.DataFrame.from_records([(
            str(i),
            make_legend_label(legend_cols, row),
            row.vectime,
            row.vecvalue
        )
        for i, row in enumerate(df.itertuples(index=False))
        if row.vectime is not None and row.vecvalue is not None
    ], columns=["key", "label", "xs", "ys"])

    plot_lines(df2)

    properties = dict()
    for i, t in enumerate(df.itertuples(index=False)):
        key = str(i)
        interpolation = t.interpolationmode if 'interpolationmode' in df else 'sample-hold' if 'enum' in df else None
        if interpolation:
            if interpolation == "none":
                properties["Line.Type/" + key] = "Dots"
            elif interpolation == "linear":
                properties["Line.Type/" + key] = "Linear"
            elif interpolation == "sample-hold":
                properties["Line.Type/" + key] = "Sample-Hold"
            elif interpolation == "backward-sample-hold":
                properties["Line.Type/" + key] = "Backward Sample-Hold"

    set_properties(properties)


def plot_vectors(df_or_list):
    _assert_is_native_chart()
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns:
            _plot_vectors_DF_scave(df)
        else:
            _plot_vectors_DF_simple(df)
    else:
        _plot_vectors_tuplelist(df_or_list)


def plot_scatter(df, xdata, iso_column=None):
    _assert_is_native_chart()
    # names = set(df["name"].values)
    # names.discard(None)
    # names.discard(xdata)
    # names.discard(iso_column)

    # TODO: compute mean if results are vectors?
    # TODO: add option to sort by a third column, instead of the X axis

    df = pd.pivot_table(df, index=xdata, columns=iso_column, values="value")

    renaming = dict()

    if iso_column:
        for c in list(df.columns):
            renaming[c] = str(iso_column) + "=" + str(c)

    df = df.reset_index()

    df.dropna(inplace=True)

    df[xdata] = pd.to_numeric(df[xdata])

    df.sort_values(by=xdata, inplace=True)
    if df.columns.nlevels > 1:
        df.columns = [' '.join(col).strip() for col in df.columns.values]

    # todo only return df
    """
    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": column,
            "key": column,
            "xs": _put_array_in_shm(np.array(df[xdata]), shm_objs, mmap_objs),
            "ys": _put_array_in_shm(np.array(df[column]), shm_objs, mmap_objs)
        }
        for column in df if column != xdata
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()

    # TODO: set_property('Y.Axis.Title', ', '.join(names))
    set_property('X.Axis.Title', xdata)
    set_property('Plot.Title', get_name())
    """


def plot_histogram(label, binedges, binvalues, underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    _assert_is_native_chart()
    plot_histogram_data(pd.DataFrame({
        "key": [label],
        "label": [label],
        "binedges": [np.array(binedges)],
        "binvalues": [np.array(binvalues)],
        "underflows": [underflows],
        "overflows": [overflows],
        "min": [float(np.min(binedges)) if minvalue == math.nan else minvalue],
        "max": [float(np.max(binedges)) if maxvalue == math.nan else maxvalue]
    }))


def _plot_histograms_DF_scave(df):

    title_col, legend_cols = extract_label_columns(df)

    plot_histogram_data(pd.DataFrame.from_records([(
            str(i),
            make_legend_label(legend_cols, row),
            np.array(row.binedges),
            np.array(row.binvalues),
            row.underflows,
            row.overflows,
            float(min(row.binedges)) if row.min == math.nan else row.min,
            float(max(row.binedges)) if row.max == math.nan else row.max
        )
        for i, row in enumerate(df.itertuples(index=False))
    ], columns=["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]))


def plot_histograms(df):
    _assert_is_native_chart()
    if "binedges" in df.columns and "binvalues" in df.columns:
        _plot_histograms_DF_scave(df)
    else:
        pass  # TODO - add other formats as well?


def set_property(key, value):
    _assert_is_native_chart()
    Gateway.chart_plotter.setProperty(key, value)


def set_properties(props):
    _assert_is_native_chart()
    Gateway.chart_plotter.setProperties(props)


def set_warning(warning):
    # TODO maybe we need a common interface?
    if Gateway.widget_provider:
        Gateway.widget_provider.setWarning(warning)

    if Gateway.chart_plotter:
        Gateway.chart_plotter.setWarning(warning)

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

def grid(show=True, which="major"):
    if which not in ["major", "both"]:
        raise ValueError("which='{}' is not supported, only 'major', and 'both'".format(which))
    if not show:
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

