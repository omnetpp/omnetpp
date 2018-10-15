import numpy as np
import pandas as pd
import math
import Gateway
import pickle as pl
import results


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


def _plotScalars_lists(row_label, labels, values):
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": labels if labels else [""] * len(values),
            "rowKeys": [row_label if row_label else ""],
            "values": _list_to_bytes(values)
        }
    ))


def _plotScalars_DF_simple(df):
    Gateway.chart_plotter.plotScalars(pl.dumps(
        {
            "columnKeys": [_to_label(c) for c in list(df.columns)],
            "rowKeys": [_to_label(i) for i in list(df.index)],
            "values": _list_to_bytes(df.values.flatten('F'))
        }
    ))


def _plotScalars_DF_scave(df):
    _plotScalars_DF_simple(results.pivotScalars(df))


def _plotScalars_DF_2(df):
    names = df.index.get_level_values('name').tolist()
    modules = df.index.get_level_values('module').tolist()

    paths = list(map(lambda t: '.'.join(t), zip(modules, names)))

    values = df[('result', 'value')]

    _plotScalars_lists(None, paths, values)


def plotScalars(df_or_values, labels=None, row_label=None):
    """
    This method only does dynamic dispatching based on its first argument.
    TODO: styling in-place?
    """
    if isinstance(df_or_values, pd.DataFrame):
        df = df_or_values
        if "value" in df.columns and "type" in df.columns and "module" in df.columns and "name" in df.columns:
            _plotScalars_DF_scave(df)
        elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
            _plotScalars_DF_2(df)
        else:
            _plotScalars_DF_simple(df)
    else:
        _plotScalars_lists(row_label, labels, df_or_values)


def plotVector(label, xs, ys):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(label),
            "key": str(label),
            "xs": _list_to_bytes(xs),
            "ys": _list_to_bytes(ys)
        }
    ]))


def _plotVectors_tuplelist(vectors):
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


def _plotVectors_DF_simple(df):
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


def _plotVectors_DF_scave(df):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": row.module + ":" + row.name,
            "key": row.module + ":" + row.name,
            "xs": _list_to_bytes(row.vectime),
            "ys": _list_to_bytes(row.vecvalue)
        }
        for row in df.itertuples(index=False) if row.type == "vector"
    ]))


def _plotVectors_DF_2(df):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": row[('attr', 'title')],
            "key": "-".join(row.name) + row[('attr', 'title')],
            "xs": _list_to_bytes(row[('result', 'vectime')]),
            "ys": _list_to_bytes(row[('result', 'vecvalue')])
        }
        for index, row in df.iterrows()
    ]))

    for index, row in df.iterrows():
        if ('attr', 'interpolationmode') in row:
            interp = row[('attr', 'interpolationmode')]
            key = "-".join(row.name) + row[('attr', 'title')]

            if interp == "none":
                setProperty("Line.Type/" + key, "Dots")
            elif interp == "linear":
                setProperty("Line.Type/" + key, "Linear")
            elif interp == "sample-hold":
                setProperty("Line.Type/" + key, "SampleHold")
            elif interp == "backward-sample-hold":
                setProperty("Line.Type/" + key, "BackwardSampleHold")


def plotVectors(df_or_list):
    """
    TODO: styling in-place?
    """
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns and "type" in df.columns and "module" in df.columns and "name" in df.columns:
            _plotVectors_DF_scave(df)
        elif "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
            _plotVectors_DF_2(df)
        else:
            _plotVectors_DF_simple(df)
    else:
        _plotVectors_tuplelist(df_or_list)


def _plotPoints_DF(df):
    Gateway.chart_plotter.plotVectors(pl.dumps([
        {
            "title": str(row[0]),
            "key": str(row[0]),
            "xs": _list_to_bytes([row.x]),
            "ys": _list_to_bytes([row.y])
        }
        for row in df.itertuples()
    ]))


def _plotPoints_tuplelist(points):
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


def plotScatter(df, xdata, iso_column):

    names = set(df["name"].values)
    names.discard(None)
    names.discard(xdata)
    names.discard(iso_column)

    df = results.pivotScalars(df, index=xdata, columns=iso_column, values="value")

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


def plotHistogram(label, edges, values, count=-1, lowest=math.nan, highest=math.nan):
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


def _plotHistograms_DF(df):
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


def _plotHistograms_DF_2(df):
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


def plotHistograms(df):
    if "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
        _plotHistograms_DF_2(df)
    else:
        _plotHistograms_DF(df)


def setProperty(key, value):
    Gateway.chart_plotter.setChartProperty(key, value)


def setProperties(*vargs, **kwargs):
    for a in vargs:
        for k, v in a.items():
            Gateway.chart_plotter.setChartProperty(k, v)  # TODO: could be optimized
    for k, v in kwargs.items():
        Gateway.chart_plotter.setChartProperty(k, v)


def getProperties():
    return Gateway.properties_provider.getChartProperties()


def getProperty(key):
    return Gateway.properties_provider.getChartProperties()["key"]  # TODO: could be optimized


def getDefaultProperties():
    return Gateway.properties_provider.getDefaultChartProperties()


def getName():
    return Gateway.properties_provider.getChartName()

"""
def getChartContents():
    return entry_point.getChartContents()
"""
