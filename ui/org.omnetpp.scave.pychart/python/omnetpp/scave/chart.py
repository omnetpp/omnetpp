import numpy as np
import pandas as pd
import math
import pickle as pl

from omnetpp.scave import results
from omnetpp.internal import Gateway

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
    # if len(legend_cols) == 1:
    #    return str(row[legend_cols[0][0]])
    return ", ".join([col + "=" + str(row[i]) for i, col in legend_cols])

def make_chart_title(df, title_col, legend_cols):
    return str(list(df[title_col])[0]) + ((" (by " + ", ".join([id[1] for id in legend_cols]) + ")") if legend_cols else "")

def _list_to_bytes(l):
    return np.array(np.array(l), dtype=np.dtype('>f8')).tobytes()


def _to_label(x):
    if isinstance(x, str):
        return x
    elif isinstance(x, tuple):
        return ", ".join(list(x))
    elif isinstance(x, list):
        return ", ".join(x)
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


def _plot_scalars_DF_2(df):
    names = df.index.get_level_values('name').tolist()
    modules = df.index.get_level_values('module').tolist()

    paths = list(map(lambda t: '.'.join(t), zip(modules, names)))

    values = df[('result', 'value')]

    _plot_scalars_lists(None, paths, values)


def plot_scalars(df_or_values, labels=None, row_label=None):
    """
    This method only does dynamic dispatching based on its first argument.
    TODO: styling in-place?
    """
    if isinstance(df_or_values, pd.DataFrame):
        df = df_or_values
        if "value" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_scalars_DF_scave(df)
        elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
            _plot_scalars_DF_2(df)
        else:
            _plot_scalars_DF_simple(df)
    else:
        _plot_scalars_lists(row_label, labels, df_or_values)


def plot_vector(label, xs, ys):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(label),
            "key": str(label),
            "xs": _list_to_bytes(xs),
            "ys": _list_to_bytes(ys)
        }
    ]))


def _plot_vectors_tuplelist(vectors):
    """ vectors is a list of (label, xs, ys) tuples """
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(v[0]),
            "key": str(v[0]),
            "xs": _list_to_bytes(v[1]),
            "ys": _list_to_bytes(v[2])
        }
        for v in vectors
    ]))


def _plot_vectors_DF_simple(df):
    xs = None
    if "time" in df:
        xs = _list_to_bytes(df["time"])
    else:
        xs = _list_to_bytes(range(len(df[df.columns[0]])))

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": column,
            "key": column,
            "xs": xs,
            "ys": _list_to_bytes(df[column])
        }
        for column in df if column != "time"
    ]))


def _plot_vectors_DF_scave(df):

    title_col, legend_cols = extract_label_columns(df)

    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": make_legend_label(legend_cols, row),
            "key": str(i),
            "xs": _list_to_bytes(row.vectime),
            "ys": _list_to_bytes(row.vecvalue)
        }
        for i, row in enumerate(df.itertuples(index=False))
    ]))

    for i, t in enumerate(df.itertuples(index=False)):
        key = str(i)
        interp = t.interpolationmode if 'interpolationmode' in df else 'sample-hold' if 'enum' in df else None
        if interp:
            if interp == "none":
                set_property("Line.Type/" + key, "Dots")
            elif interp == "linear":
                set_property("Line.Type/" + key, "Linear")
            elif interp == "sample-hold":
                set_property("Line.Type/" + key, "SampleHold")
            elif interp == "backward-sample-hold":
                set_property("Line.Type/" + key, "BackwardSampleHold")


def _plot_vectors_DF_2(df):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": row[('attr', 'title')],
            "key": "-".join(row.name) + row[('attr', 'title')],
            "xs": _list_to_bytes(row[('result', 'vectime')]),
            "ys": _list_to_bytes(row[('result', 'vecvalue')])
        }
        for index, row in df.iterrows()
    ]))

def plot_vectors(df_or_list):
    """
    TODO: styling in-place?
    """
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_vectors_DF_scave(df)
        elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
            _plot_vectors_DF_2(df)
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
    set_property("Graph.Title", title)



def _plot_histograms_DF_2(df):
    Gateway.chart_plotter.plotHistograms(pl.dumps([
        {
            "title": row[('attr', 'title')],  # row[2] + ":" + row[3],
            "key": "-".join(row.name) + row[('attr', 'title')],  # row[2] + ":" + row[3],
            "count": int(row[('result', 'count')]),
            "min": float(row[('result', 'min')]),
            "max": float(row[('result', 'max')]),
            "edges": _list_to_bytes(list(row[('result', 'binedges')]) + [float('inf')]),
            "values": _list_to_bytes(row[('result', 'binvalues')])
        }
        for index, row in df.iterrows()
    ]))


def plot_histograms(df):
    if "binedges" in df and "binvalues" in df and "module" in df and "name" in df:
        _plot_histograms_DF_scave(df)
    if "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
        _plot_histograms_DF_2(df)
    else:
        _plot_histograms_DF(df)


def set_property(key, value):
    Gateway.chart_plotter.setChartProperty(key, value)


def set_properties(*vargs, **kwargs):
    for a in vargs:
        for k, v in a.items():
            Gateway.chart_plotter.setChartProperty(k, v)  # TODO: could be optimized
    for k, v in kwargs.items():
        Gateway.chart_plotter.setChartProperty(k, v)


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
