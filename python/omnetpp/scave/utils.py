from omnetpp.scave import chart
import matplotlib as mpl
import sys


def extract_label_columns(df):

    """
    Utility function to make a reasonable guess as to which column of
    the given DataFrame is most suitable to act as a chart title and
    which ones can be used as legend labels.

    Ideally a "title column" should be one in which all lines have the same
    value, and can be reasonably used as a title. Some often used candidates
    are: `title`, `name`, and `module`.

    Label columns should be a minimal set of columns whose corresponding
    value tuples uniquely identify every line in the DataFrame. These will
    primarily be iteration variables and run attributes.

    # Returns:

    A pair of a string and a list; the first value is the name of the
    "title" column, and the second one is a list of pairs, each
    containing the index and the name of a "label" column.

    Example: `('title', [(8, 'numHosts'), (7, 'iaMean')])`
    """

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
    """
    Produces a reasonably good label text (to be used in a chart legend) for a result row from
    a DataFrame, given a list of selected columns as returned by `extract_label_columns()`.
    """

    if len(legend_cols) == 1:
        return str(row[legend_cols[0][0]])
    return ", ".join([col + "=" + str(row[i]) for i, col in legend_cols])


def make_chart_title(df, title_col, legend_cols):
    """
    Produces a reasonably good chart title text from a result DataFrame, given a selected "title"
    column, and a list of selected "legend" columns as returned by `extract_label_columns()`.
    """
    if df is None or df.empty or title_col not in df:
        return "None"

    what = str(list(df[title_col])[0]) if title_col else "Data"
    if title_col and len(df[title_col].unique()) > 1:
        what += " and other variables"
    by_what = (" (by " + ", ".join([id[1] for id in legend_cols]) + ")") if legend_cols else ""
    return what + by_what


def parse_matplotlib_rcparams(rc_content):
    rc_temp = {}
    for line_no, line in enumerate(rc_content.split("\n"), 1):
        strippedline = line.split('#', 1)[0].strip()
        if not strippedline:
            continue
        tup = strippedline.split(':', 1)
        if len(tup) != 2:
            raise RuntimeError("Illegal rc line #" + str(line_no)) + ": " + line
        key, val = tup
        key = key.strip()
        val = val.strip()
        if key in rc_temp:
            raise RuntimeError("Duplicate key " + key + " on line " + str(line_no))
        rc_temp[key] = val

    return rc_temp


def update_matplotlib_rcparams(props):
    """
    Updates `mpl.rcParams` taking suitable values from the `props` dictionary.
    """
    allowed_keys = mpl.rcParams.keys()
    filtered_props = {k:v for (k, v) in props.items() if k in allowed_keys and v}
    mpl.rcParams.update(filtered_props)


import numpy as np
from matplotlib.text import Text
import math


# Declare and register callbacks
def on_xlims_change(ax):
    display_xs = list()
    fontheight = 24
    for p in ax.get_xticks():
        xdisplay, ydisplay = ax.transData.transform_point((p, 0))
        display_xs.append(xdisplay)

    dxa = np.array(display_xs)
    dxd = dxa[1:] - dxa[:-1]
    min_dx = dxd.min() + 12

    tes = list()
    for l in ax.get_xticklabels():
        text = Text(text=l, figure=ax.figure)
        extent = text.get_window_extent()
        tes.append(extent.width)
        fontheight = extent.height

    max_tx = np.array(tes).max()

    if min_dx > max_tx:
        angle = 0
    elif min_dx < fontheight * 2:
        angle = 90
    else:
        angle = math.atan(fontheight / (min_dx - fontheight * 2)) / math.pi * 180

    angle = max(0, min(90, angle))

    plt.xticks(rotation=angle, horizontalalignment="right")
    plt.tight_layout()


def make_fancy_xticklabels(ax):
    ax.callbacks.connect('xlim_changed', on_xlims_change)


def interpolationmode_to_plot_params(drawstyle, interpolationmode, hasenum):
    style = dict()

    ds = drawstyle
    if not ds or ds == "auto":
        interp = interpolationmode if interpolationmode else 'sample-hold' if 'hasenum' else None
        if interp:
            if interp == "none":
                ds = "dots"
            elif interp == "linear":
                ds = "linear"
            elif interp == "sample-hold":
                ds = "steps-post"
            elif interp == "backward-sample-hold":
                ds = 'steps-pre'
            else:
                print("Unknown interpolationmode for", t, ":", interp, file=sys.stderr)
                ds = None
        else:
            ds = "linear"

    if ds == "dots":
        style['linestyle'] = ' '
        style['marker'] = '.'
    elif ds == "points":
        style['linestyle'] = ' '
        style['marker'] = '.'
        style['markersize'] = 1
    elif ds == "linear":
        pass  # nothing to do
    elif ds == 'steps-pre':
        style['drawstyle'] = 'steps-pre'
    elif ds == "steps-mid":
        style['drawstyle'] = 'steps-mid'
    elif ds == "steps-post":
        style['drawstyle'] = 'steps-post'
    else:
        if ds:
            print("Unknown drawstyle:", ds, file=sys.stderr)

    return style
