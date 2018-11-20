import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math
from omnetpp.scave import results

properties = dict()
name = ""

def getProperties():
    return properties

def setProperties(*vargs, **kwargs):
    for a in vargs:
        properties.update(a)
    properties.update(kwargs)

def setProperty(key, value):
    properties[key] = value

def getDefaultProperties():
    return dict() # TODO

def getName():
    return name


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
    plt.bar(np.arange(len(values)), values, tick_label=labels, label=row_label)
    plt.legend()
    plt.grid()

def _plotScalars_DF_simple(df):
    column_labels = [_to_label(c) for c in list(df.columns)]

    N = len(df.index)
    M = 0
    i = 0
    for index, row in df.iterrows():
        v = row.values
        M = len(v)
        plt.bar(np.arange(M) + i * 0.8 / N, v, width=0.8/N, tick_label=column_labels, label=_to_label(index))
        i += 1

    if N and M:
        plt.xticks(np.arange(M) + 0.8 / N * ((N-1)/2))

    plt.grid()
    plt.legend()


def _plotScalars_DF_scave(df):
    _plotScalars_DF_simple(results.pivotScalars(df))


def _plotScalars_DF_2(df):
    names = df.index.get_level_values('name').tolist()
    modules = df.index.get_level_values('module').tolist()

    paths = list(map(lambda t: '.'.join(t), zip(modules, names)))

    values = df[('result', 'value')]

    _plotScalars_lists(None, paths, values)


def plotScalars(df_or_values, labels=None, row_label=None):
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
    plt.plot(xs, ys, label=_to_label(label))
    plt.legend()


def _plotVectors_tuplelist(vectors):
    for v in vectors:
        plt.plot(v[1], v[2], label=_to_label(v[0]))
    plt.legend()


def _plotVectors_DF_simple(df):
    xs = None
    if "time" in df:
        xs = list(df["time"])
    else:
        xs = list(range(len(df[df.columns[0]])))

    for column in df:
        if column != "time":
            plt.plot(xs, list(df[column]), label=_to_label(column))
    plt.legend()


def _plotVectors_DF_scave(df):
    for row in df.itertuples(index=False):
        if row.type == "vector":
            plt.plot(list(row.vectime), list(row.vecvalue), label=row.module + ':' + row.name)
    plt.legend()

def _plotVectors_DF_2(df):
    for index, row in df.iterrows():
        style = dict()
        if ('attr', 'interpolationmode') in row:
            interp = row[('attr', 'interpolationmode')]
            if interp == "none":
                style['linestyle'] = ' '
                style['marker'] = '.'
            elif interp == "linear":
                pass
                # nothing to do
            elif interp == "sample-hold":
                style['drawstyle'] = 'steps-post'
            elif interp == "backward-sample-hold":
                style['drawstyle'] = 'steps-pre'


        plt.plot(list(row[('result', 'vectime')]), list(row[('result', 'vecvalue')]), label=row[('attr', 'title')], **style)



def plotVectors(df_or_list):
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




def plotHistogram(label, edges, values, count=-1, lowest=math.nan, highest=math.nan):
    plt.hist(bins=edges, x=edges[:-1], weights=values, label=label)
    plt.legend()


def _plotHistograms_DF(df):
    for row in df.itertuples(index=False):
        if row[1] == "histogram":

            edges = list(row[12])
            values = list(row[13])

            plt.hist(bins=edges, x=edges[:-1], weights=values, label=row[2] + ":" + row[3])
    plt.legend()


def _plotHistograms_DF_2(df):
    for index, row in df.iterrows():
        edges = list(row[('result', 'binedges')])
        values = list(row[('result', 'binvalues')])
        plt.hist(bins=edges, x=edges[:-1], weights=values, label=row[('attr', 'title')])



def plotHistograms(df):
    if "experiment" in df.index.names and "measurement" in df.index.names and "replication" in df.index.names and "module" in df.index.names and "name" in df.index.names:
        _plotHistograms_DF_2(df)
    else:
        _plotHistograms_DF(df)
