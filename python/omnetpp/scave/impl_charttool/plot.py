import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math
from omnetpp.scave import results, plot
from omnetpp.scave import utils # for small misc. methods

properties = dict()
name = ""


def set_properties(*vargs, **kwargs):
    for a in vargs:
        properties.update(a)
    properties.update(kwargs)


def set_property(key, value):
    properties[key] = value


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
    plt.bar(np.arange(len(values)), values, tick_label=labels, label=row_label)
    plt.legend()
    plt.grid()


def _plot_scalars_DF_simple(df):
    column_labels = [_to_label(c) for c in list(df.columns)]

    N = len(df.index)
    M = 0
    i = 0
    for index, row in df.iterrows():
        v = row.values
        M = len(v)
        plt.bar(np.arange(M) + i * 0.8 / N, v, width=0.8 / N, tick_label=column_labels, label=_to_label(index))
        i += 1

    if N and M:
        plt.xticks(np.arange(M) + 0.8 / N * ((N - 1) / 2))

    plt.grid()
    plt.legend()


def _plot_scalars_DF_scave(df):
    print("patty")
    df.plot.bar(y="value")
    plt.legend()


def plot_scalars(df_or_values, labels=None, row_label=None):
    if isinstance(df_or_values, pd.DataFrame):
        df = df_or_values
        if "value" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_scalars_DF_scave(df)
        else:
            _plot_scalars_DF_simple(df)
    else:
        _plot_scalars_lists(row_label, labels, df_or_values)


def plot_vector(label, xs, ys, key=None, props=dict()):
    plt.plot(xs, ys, label=_to_label(label))
    plt.legend()

    # TODO: deal with key and props


def _plot_vectors_tuplelist(vectors):
    for v in vectors:
        plt.plot(v[1], v[2], label=_to_label(v[0]))
    plt.legend()


def _plot_vectors_DF_simple(df):
    xs = None
    if "time" in df:
        xs = list(df["time"])
    else:
        xs = list(range(len(df[df.columns[0]])))

    for column in df:
        if column != "time":
            plt.plot(xs, list(df[column]), label=_to_label(column))
    plt.legend()


def _plot_vectors_DF_scave(df):
    for row in df.itertuples(index=False):
        plt.plot(list(row.vectime), list(row.vecvalue), label=row.module + ':' + row.name)
    plt.legend()


def plot_vectors(df_or_list):
    if isinstance(df_or_list, pd.DataFrame):
        df = df_or_list
        if "vectime" in df.columns and "vecvalue" in df.columns and "module" in df.columns and "name" in df.columns:
            _plot_vectors_DF_scave(df)
        else:
            _plot_vectors_DF_simple(df)
    else:
        _plot_vectors_tuplelist(df_or_list)


def plot_histogram(label, binedges, binvalues, underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    plt.hist(bins=edges, x=edges[:-1], weights=values, label=label)
    plt.legend()


def _plot_histograms_DF_scave(df):
    for row in df.itertuples(index=False):
        edges = list(row.binedges)
        values = list(row.binvalues)

        plt.hist(bins=edges, x=edges[:-1], weights=values, label=row.module + ":" + row.name)
    plt.legend()


def _plot_histograms_DF(df):
    for row in df.itertuples(index=False):
        if row[1] == "histogram":

            edges = list(row[12])
            values = list(row[13])

            plt.hist(bins=edges, x=edges[:-1], weights=values, label=row[2] + ":" + row[3])
    plt.legend()


def plot_histograms(df):
    if "binedges" in df and "binvalues" in df and "module" in df and "name" in df:
        _plot_histograms_DF_scave(df)
    else:
        _plot_histograms_DF(df)

def get_supported_property_keys():
    return [] # TODO

def set_warning(warning):
    # TODO this might not always work, depending on whether it's called before or after plotting?
    plt.annotate(warning, xy=(20, 40), xycoords="figure pixels", color="red")

# params?
def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    params = {k:v for k, v in locals().items() if k is not None and k not in ["key", "xs", "ys"]}
    return plt.plot(xs, ys, **params)

# key?
def hist(x, bins, density=False, weights=None, cumulative=False, bottom=None, histtype='stepfilled', color=None, label=None, linewidth=None,
         underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):

    params = locals()
    params = {k:v for k, v in locals().items() if k is not None and k not in ["underflows", "overflows", "minvalue", "maxvalue", "params"]}
    print(params)
    return plt.hist(**params)

bar = plt.bar
title = plt.title
xlabel = plt.xlabel
ylabel = plt.ylabel
xlim = plt.xlim
ylim = plt.ylim
xscale = plt.xscale
yscale = plt.yscale
xticks = plt.xticks
legend = plt.xticks
grid = plt.grid

def legend(*args, **kwargs):
    if "show" in kwargs:
        if kwargs["show"] is not None and not kwargs["show"]:
            if plt.gca().get_legend() is not None:
                plt.gca().get_legend().remove()
            return
        del kwargs["show"]
    if "loc" in kwargs and kwargs["loc"] and kwargs["loc"].startswith("outside"):
        kwargs2 = utils._legend_mpl_loc_outside_args(kwargs["loc"])
        del kwargs["loc"]
        kwargs.update(kwargs2)
    plt.legend(*args, **kwargs)
