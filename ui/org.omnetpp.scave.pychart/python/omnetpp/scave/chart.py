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

def extract_label_columns(df):
    titles = ["title", "name", "module", "experiment", "measurement", "replication"]
    legends = ["title", "name", "module", "experiment", "measurement", "replication"]

    blacklist = ["runID", "value", "vectime", "vecvalue", "binedges", "binvalues",
                 "count", "sumweights", "mean", "stddev", "min", "max",
                 "processid", "iterationvars", "iterationvarsf", "datetime",
                 "source", "interpolationmode", "enum", "title", 'runnumber', 'seedset'
                ]

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
    for i, col in reversed(list(enumerate(df))):
        if col not in blacklist and col != title_col:
            new_len = len(df.groupby([i2 for i1, i2 in legend_cols] + [col]))
            if new_len == len(df) or (not last_len) or new_len > last_len:
                legend_cols.append((i, col))
                last_len = new_len
            if new_len == len(df):
                break
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

def _list_to_bytes(l):
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def _to_label(x):
    if isinstance(x, str):
        return x
    elif isinstance(x, tuple):
        return ", ".join(map(str, list(x)))
    elif isinstance(x, list):
        return ", ".join(map(str, x))
    else:
        return str(x)


def _plot_scalars_lists(row_label, labels, values):
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": labels if labels else [""] * len(values),
            "rowKeys": [row_label if row_label else ""],
            "values": _list_to_bytes(values)
        }
    ))


def _plot_scalars_DF_simple(df):
    # TODO: detect single-valued index/ column header levels, drop them
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": [_to_label(c) for c in list(df.columns)],
            "rowKeys": [_to_label(i) for i in list(df.index)],
            "values": _list_to_bytes(df.values.flatten('F'))
        }
    ))

def _plot_scalars_DF_scave(df):
    # TODO: pivot in the other direction (transposed)?
    # TODO: this is... wrong?
    _plot_scalars_DF_simple(pd.pivot_table(df, index="module", columns="name", values="value"))


def plot_scalars(df_or_values, labels=None, row_label=None):
    """
    This method only does dynamic dispatching based on its first argument.
    TODO: styling in-place?
    """
    if isinstance(df_or_values, pd.DataFrame):
        df = df_or_values
        if "value" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_scalars_DF_scave(df)
        else:
            _plot_scalars_DF_simple(df)
    else:
        _plot_scalars_lists(row_label, labels, df_or_values)


def _put_array_in_shm(arr, shm_objs, mmap_objs):
    global vector_data_counter
    # the only need to start with a / on POSIX, but Windows doesn't seem to mind, so just putting it there always...
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

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(label),
            "key": str(label),
            "xs": _put_array_in_shm(np.array(xs), shm_objs, mmap_objs),
            "ys": _put_array_in_shm(np.array(ys), shm_objs, mmap_objs)
        }
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def _plot_vectors_tuplelist(vectors):

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    """ vectors is a list of (label, xs, ys) tuples """
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(v[0]),
            "key": str(v[0]),
            "xs": _put_array_in_shm(np.array(v[1]), shm_objs, mmap_objs),
            "ys": _put_array_in_shm(np.array(v[2]), shm_objs, mmap_objs)
        }
        for v in vectors
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def _plot_vectors_DF_simple(df):
    xs = None
    if "time" in df:
        xs = df["time"]
    else:
        xs = range(len(df[df.columns[0]]))

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(column),
            "key": str(column),
            "xs": _put_array_in_shm(xs[~np.isnan(df[column])], shm_objs, mmap_objs),
            "ys": _put_array_in_shm(df[column].values[~np.isnan(df[column])], shm_objs, mmap_objs)
        }
        for column in df if column != "time"
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()


def _plot_vectors_DF_scave(df):

    title_col, legend_cols = extract_label_columns(df)

    # only used on posix, to unlink them later
    shm_objs = list()
    # only used on windows, to prevent gc
    mmap_objs = list()

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": make_legend_label(legend_cols, row),
            "key": str(i),
            "xs": _put_array_in_shm(row.vectime, shm_objs, mmap_objs),
            "ys": _put_array_in_shm(row.vecvalue, shm_objs, mmap_objs)
        }
        for i, row in enumerate(df.itertuples(index=False))
    ]))

    # this is a no-op on Windows
    for o in shm_objs:
        o.unlink()

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

    set_properties(properties)

def plot_vectors(df_or_list):
    """
    TODO: styling in-place?
    """
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns:
            _plot_vectors_DF_scave(df)
        else:
            _plot_vectors_DF_simple(df)
    else:
        _plot_vectors_tuplelist(df_or_list)


def _plot_points_DF(df):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(row[0]),
            "key": str(row[0]),
            "xs": _list_to_bytes([row.x]),
            "ys": _list_to_bytes([row.y])
        }
        for row in df.itertuples()
    ]))


def _plot_points_tuplelist(points):
    """ points is a list of (label, x, y) tuples """
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(point[0]),
            "key": str(point[0]),
            "xs": _list_to_bytes([point[1]]),
            "ys": _list_to_bytes([point[2]])
        }
        for point in points
    ]))

"""
# TODO: scatter charts: same as vector, just with implicit "Dots" Line Type?
def plotPoints(df_or_points):
    if isinstance(df_or_points, pd.DataFrame):
        _plotPoints_DF(df_or_points)
    else:
        _plotPoints_tuplelist(df_or_points)
"""


def plot_scatter(df, xdata, iso_column):

    names = set(df["name"].values)
    names.discard(None)
    names.discard(xdata)
    names.discard(iso_column)

    df = results.pivot_scalars(df, index=xdata, columns=iso_column, values="value")

    renaming = dict()

    if iso_column:
        for c in list(df.columns):
            renaming[c] = iso_column + "=" + str(c)

    df = df.reset_index()

    renaming[xdata] = "time"

    df = df.rename(renaming, axis="columns")

    df.dropna(inplace=True)

    df["time"] = pd.to_numeric(df["time"])

    df.sort_values(by="time", inplace=True)

    print(df)

    xs = _list_to_bytes(df["time"])

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": column,
            "key": column,
            "xs": xs,
            "ys": _list_to_bytes(df[column])
        }
        for column in df if column != "time"
    ]))

    return list(names)


def plot_histogram(label, edges, values, count=-1, lowest=math.nan, highest=math.nan):
    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "title": label,
            "key": label,
            "count": len(values) if count == -1 else count,
            "min": float(min(edges)) if lowest == math.nan else lowest,
            "max": float(max(edges)) if highest == math.nan else highest,
            "edges": _list_to_bytes(edges),
            "values": _list_to_bytes(values)
        }
    ]))


def _plot_histograms_DF(df):
    for row in df.itertuples(index=False):
        if row[1] == "histogram":

            min = float(row[10])
            max = float(row[11])
            count = int(row[6])

            edges = _list_to_bytes(list(row[12]) + [float('inf')])
            values = _list_to_bytes(row[13])

            Gateway.chart_plotter.plotHistograms(pl.dumps([
                {
                    "title": row[2] + ":" + row[3],
                    "key": row[2] + ":" + row[3],
                    "count": count,
                    "min": min,
                    "max": max,
                    "edges": edges,
                    "values": values
                }
            ]))



def _plot_histograms_DF_scave(df):
    title_col, legend_cols = extract_label_columns(df)

    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "title": make_legend_label(legend_cols, row),  # row[2] + ":" + row[3],
            "key": str(i), # "-".join([row.title, row.runID]),  # row[2] + ":" + row[3],
            "count": int(row.count),
            "min": float(row.min),
            "max": float(row.max),
            "edges": _list_to_bytes(list(row.binedges) + [float('inf')]),
            "values": _list_to_bytes(row.binvalues)
        }
        for i, row in enumerate(df.itertuples(index=False))
    ]))

    title = make_chart_title(df, title_col, legend_cols)
    if not get_property("Graph.Title"):
        set_property("Graph.Title", title)

def plot_histograms(df):
    if "binedges" in df and "binvalues" in df and "module" in df and "name" in df:
        _plot_histograms_DF_scave(df)
    else:
        _plot_histograms_DF(df)


def set_property(key, value):
    Gateway.chart_plotter.setChartProperty(key, value)

def set_properties(*vargs, **kwargs):
    props = dict()
    for a in vargs:
        if type(a) != dict:
            raise Exception("dictionary expected")
        props.update(a)
    for k, v in kwargs.items():
        props[k] = v
    Gateway.chart_plotter.setChartProperties(props)


def get_properties():
    return Gateway.properties_provider.getChartProperties()


def get_property(key):
    return Gateway.properties_provider.getChartProperties()["key"]  # TODO: could be optimized


def get_default_properties():
    return Gateway.properties_provider.getDefaultChartProperties()


def get_name():
    return Gateway.properties_provider.getChartName()

"""
def getChartContents():
    return entry_point.getChartContents()
"""
