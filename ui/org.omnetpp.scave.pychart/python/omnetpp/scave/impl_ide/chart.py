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

from omnetpp.scave import results
from omnetpp.internal import Gateway

vector_data_counter = 0


def _list_to_bytes(l):
    """
    Internal. Encodes/serializes a list or `np.array` of numbers as
    8-byte floats (doubles) into an `np.array` of bytes.
    """
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def extract_label_columns(df):
    titles = ["title", "name", "module", "experiment", "measurement", "replication"]
    legends = ["title", "name", "module", "experiment", "measurement", "replication"]

    blacklist = ["runID", "value", "attrvalue", "vectime", "vecvalue",
                 "binedges", "binvalues", "underflows", "overflows",
                 "count", "sumweights", "mean", "stddev", "min", "max",
                 "processid", "iterationvars", "iterationvarsf", "datetime",
                 "source", "interpolationmode", "enum", "title", 'runnumber', 'seedset']

    title_col = None

    for title in titles:
        if title in df and len(df[title].unique()) == 1:
            title_col = title
            break
    if title_col == None:
        if "name" in titles:
            title_col = "name"

    legend_cols = []

    for legend in legends:
        if legend in df and len(df[legend].unique()) == len(df):
            legend_cols = [(list(df.columns.values).index(legend), legend)]
            break

    if legend_cols:
        return title_col, legend_cols

    last_len = None
    for i, col in list(enumerate(df)):
        if col not in blacklist and col != title_col:
            new_len = len(df.groupby([i2 for i1, i2 in legend_cols] + [col]))
            if new_len == len(df) or not last_len or new_len > last_len:
                legend_cols.append((i, col))
                last_len = new_len
            if new_len == len(df):
                break

    # filter out columns which only have a single value in them (this can happen in the loop above, with the first considered column)
    legend_cols = list(filter(lambda icol: len(df[icol[1]].unique()) > 1, legend_cols))

    """
    if not legend_cols:
        last_len = None
        for i, col in reversed(list(enumerate(df))):
            if col not in blacklist and col != title_col:
                new_len = len(df.groupby([i2 for i1, i2 in legend_cols] + [col]))
                if new_len > 1:
                    legend_cols.append((i, col))
    """

    # TODO: use runID (or iterationvars?) as last resort (if present)

    # at this point, ideally this should hold: len(df.groupby([i2 for i1, i2 in legend_cols])) == len(df)
    return title_col, legend_cols


def make_legend_label(legend_cols, row):
    if len(legend_cols) == 1:
        return str(row[legend_cols[0][0]])
    return ", ".join([col + "=" + str(row[i]) for i, col in legend_cols])


def make_chart_title(df, title_col, legend_cols):
    if df is None or df.empty or title_col not in df:
        return "None"

    what = str(list(df[title_col])[0]) if title_col else "Data"
    if title_col and len(df[title_col].unique()) > 1:
        what += " and other variables"
    by_what = (" (by " + ", ".join([id[1] for id in legend_cols]) + ")") if legend_cols else ""
    return what + by_what


def plot_bars(df):
    # TODO: add check for one-layer indices? numbers-only data?
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": [_to_label(c) for c in list(df.columns)],
            "rowKeys": [_to_label(i) for i in list(df.index)],
            "values": _list_to_bytes(df.values.flatten('F'))
        }
    ))

def plot_lines(df): # key, label, xs, ys
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
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()

def plot_histogram_data(df): # key, label, binedges, binvalues, underflows, overflows, min, max
    print(df)
    if sorted(list(df.columns)) != sorted(["key", "label", "binedges", "binvalues", "underflows", "overflows", "min", "max"]):
        raise RuntimeError("Invalid DataFrame format in plot_histogram_data")

    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "key": row.key,
            "title": row.label,
            "sumweights": float(np.sum(row.binvalues) + row.underflows + row.overflows), # TODO remove?

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


def plot_vector(label, xs, ys):
    plot_lines(pd.DataFrame({
        "key": [label],
        "label": [label],
        "xs": [np.array(xs)],
        "ys": [np.array(ys)]
        }
    ))


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
                properties["Line.Type/" + key] = "SampleHold"
            elif interpolation == "backward-sample-hold":
                properties["Line.Type/" + key] = "BackwardSampleHold"

    set_plot_properties(properties)


def plot_vectors(df_or_list):
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns:
            _plot_vectors_DF_scave(df)
        else:
            _plot_vectors_DF_simple(df)
    else:
        _plot_vectors_tuplelist(df_or_list)


def plot_scatter(df, xdata, iso_column=None):
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
    set_property('Graph.Title', get_name())
    """


def plot_histogram(label, binedges, binvalues, underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
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

    title = make_chart_title(df, title_col, legend_cols)
    if not get_configured_property("Graph.Title"):
        set_plot_property("Graph.Title", title)


def plot_histograms(df):
    if "binedges" in df.columns and "binvalues" in df.columns:
        _plot_histograms_DF_scave(df)
    else:
        pass  # TODO - add other formats as well?


def set_plot_property(key, value):
    Gateway.chart_plotter.setChartProperty(key, value)


def set_plot_properties(props):
    Gateway.chart_plotter.setChartProperties(props)


def get_configured_properties():
    return Gateway.properties_provider.getChartProperties()


def get_configured_property(key):
    return Gateway.properties_provider.getChartProperties()[key]  # TODO: could be optimized


# def get_default_properties():
#     return Gateway.properties_provider.getDefaultChartProperties()


def get_name():
    return Gateway.properties_provider.getChartName()

"""
def getChartContents():
    return entry_point.getChartContents()
"""
