import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math
from omnetpp.scave import results

class NoneDict(dict):
    def __getitem__(self, key):
        return dict.get(self, key, None)

properties = NoneDict()
name = ""

def get_configured_properties():
    return properties

def set_plot_properties(*vargs, **kwargs):
    for a in vargs:
        properties.update(a)
    properties.update(kwargs)

def set_plot_property(key, value):
    properties[key] = value

#def get_default_properties():
#    return NoneDict() # TODO

def get_name():
    return name

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
        plt.bar(np.arange(M) + i * 0.8 / N, v, width=0.8/N, tick_label=column_labels, label=_to_label(index))
        i += 1

    if N and M:
        plt.xticks(np.arange(M) + 0.8 / N * ((N-1)/2))

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


def plot_vector(label, xs, ys):
    plt.plot(xs, ys, label=_to_label(label))
    plt.legend()


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

# this is needed only because opp_charttool directly imports this impl module (for technical reasons)
def copy_properties():
    set_plot_properties(get_configured_properties())
