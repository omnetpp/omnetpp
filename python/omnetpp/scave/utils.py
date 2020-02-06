from omnetpp.scave import chart, plot
import matplotlib as mpl
import matplotlib.pyplot as plt
import sys


def extract_label_columns(df, preferred_legend_column="title"):

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
    titles.remove(preferred_legend_column)
    titles = [preferred_legend_column] + titles

    legends = titles

    blacklist = ["runID", "value", "attrvalue", "vectime", "vecvalue",
                 "binedges", "binvalues", "underflows", "overflows",
                 "count", "sumweights", "mean", "stddev", "min", "max",
                 "processid", "iterationvars", "iterationvarsf", "datetime",
                 "source", "interpolationmode", "enum", "title", "unit", 'runnumber', 'seedset']

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


def parse_rcparams(rc_content):
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


def _filter_by_key_prefix(props, prefix):
    return {k[len(prefix):] : v for (k, v) in props.items() if k.startswith(prefix) and v}

def _parse_optional_bool(value):
    if value is None:
        return None
    if value.lower() not in ["true", "false"]:
        raise ValueError("Invalid boolean property value: " + value)
    return value.lower() == "true"

def make_fancy_xticklabels(ax):

    import math
    import numpy as np
    from matplotlib.text import Text

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

    ax.callbacks.connect('xlim_changed', on_xlims_change)


def interpolationmode_to_drawstyle(interpolationmode, hasenum):
    interp = interpolationmode if interpolationmode else 'sample-hold' if hasenum else None
    if not interp:
        ds = "linear"
    elif interp == "none":
        ds = "none"
    elif interp == "linear":
        ds = "linear"
    elif interp == "sample-hold":
        ds = "steps-post"
    elif interp == "backward-sample-hold":
        ds = 'steps-pre'
    else:
        print("Unknown interpolationmode:", interp, file=sys.stderr)
        ds = None

    return ds

def _make_line_args(props, t, df):
    style = dict()

    if props["linestyle"]:
        style["linestyle"] = props["linestyle"]  # note: must precede 'drawstyle' setting

    ds = props["drawstyle"]
    if not ds or ds == "auto":
        interpolationmode = t.interpolationmode if "interpolationmode" in df else None
        hasenum = "enum" in df
        ds = interpolationmode_to_drawstyle(interpolationmode, hasenum)

    if ds == "none":
        style["linestyle"] = " "
        style["drawstyle"] = "default"  # or any valid value, doesn't matter
    else:
        style["drawstyle"] = ds if props["drawstyle"] != "linear" else "default"

    if props["linecolor"]:
        style["color"] = props["linecolor"]

    if props["linewidth"]:
        style["linewidth"] = props["linewidth"]

    if not props["marker"] or props["marker"] == "auto":
        pass
    elif props["marker"] == 'none':
        style["marker"] = ' '
    else:
        style["marker"] = props["marker"][0] # take first character only, as Matplotlib uses 1-char codes; this allows us to include a description

    if props["markersize"]:
        style["markersize"] = props["markersize"]

    return style

def _make_histline_args(props, t, df):
    style = dict()

    if props["color"]:
        style["color"] = props["color"]

    if props["linewidth"]:
        style["linewidth"] = props["linewidth"]

    return style

def set_plot_title(title, suggested_chart_name=None):
    if chart.is_native_chart():
        plot.title(title)
    else:
        plt.title(title)
    chart.set_suggested_chart_name(suggested_chart_name if suggested_chart_name is not None else title)

def plot_bars(df, props, names=None):
    p = plot if chart.is_native_chart() else plt

    ind = np.arange(len(df.index))  # the x locations for the groups
    width = 0.8 / len(df.columns)  # the width of the bars

    for i, column in enumerate(df):
        p.bar(ind - 0.4 + width*i + width * 0.5, df[column], width, label=column)

    # Add some text for labels, title and custom x-axis tick labels, etc.
    groups = props["groups"].split(",")
    series = props["series"].split(",")

    if not chart.is_native_chart():
        ax = plt.gca()
        ax.set_xticks(ind)
        ax.set_xticklabels(df.index, rotation=30, horizontalalignment="right")

    p.xlabel(df.index.names[0])
    if names:
        p.ylabel(", ".join(names))
        set_plot_title(", ".join(names) + " by " + ", ".join(groups+series))

def plot_vectors(df, props):
    p = plot if chart.is_native_chart() else plt

    column = "name" if props["legend_labels"] == "result names" else "title"
    title_col, legend_cols = extract_label_columns(df, column)

    for i, t in enumerate(df.itertuples(index=False)):
        style = _make_line_args(props, t, df)
        p.plot(t.vectime, t.vecvalue, label=make_legend_label(legend_cols, t), **style)

    title = props['title'] or make_chart_title(df, title_col, legend_cols)
    set_plot_title(title)

def plot_histograms(df, props):
    p = plot if chart.is_native_chart() else plt

    title_col, legend_cols = extract_label_columns(df)

    for i, t in enumerate(df.itertuples(index=False)):
        style = _make_histline_args(props, t, df)
        p.hist(t.binedges[:-1], t.binedges, weights=t.binvalues, label=make_legend_label(legend_cols, t), **style)

    title = props['title'] or make_chart_title(df, title_col, legend_cols)
    set_plot_title(title)

def preconfigure_plot(props):
    if chart.is_native_chart():
        #plot.set_properties(_filter_by_key_prefix(props,"plot."))  #TODO this was after plotting, was that intentional?
        supported_keys = plot.get_supported_property_keys()
        plot.set_properties({ k: v for k, v in props.items() if k in supported_keys})
        plot.set_properties(parse_rcparams(props["plot.properties"] or ""))
    else:
        if (props['plt.style']):
            plt.style.use(props['plt.style'])
        mpl.rcParams.update(_filter_by_key_prefix(props,"matplotlibrc."))
        mpl.rcParams.update(parse_rcparams(props["matplotlibrc"] or ""))


def postconfigure_plot(props):
    p = plot if chart.is_native_chart() else plt
    
    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("yaxis_title"):
        p.xlabel(props["xaxis_title"])
    if get_prop("yaxis_title"):
        p.ylabel(props["yaxis_title"])

    if get_prop("xaxis_min"):
        p.xlim(left=float(props["xaxis_min"]))
    if get_prop("xaxis_max"):
        p.xlim(right=float(props["xaxis_max"]))
    if get_prop("yaxis_min"):
        p.ylim(bottom=float(props["yaxis_min"]))
    if get_prop("yaxis_max"):
        p.ylim(top=float(props["yaxis_max"]))

    if get_prop("xaxis_log"):
        p.xscale("log" if _parse_optional_bool(props["xaxis_log"]) else "linear")
    if get_prop("yaxis_log"):
        p.yscale("log" if _parse_optional_bool(props["yaxis_log"]) else "linear")

    legend(show=_parse_optional_bool(get_prop("legend_show")),
           frameon=_parse_optional_bool(get_prop("legend_border")),
           loc=get_prop("legend_placement"))

    p.grid(_parse_optional_bool(get_prop("grid_show")),
             "major" if (get_prop("grid_density") or "").lower() == "major" else "both") # grid_density is "Major" or "All"

    if not chart.is_native_chart():
        plt.tight_layout()

# XXX: why is this here, and not in plot.py?
def legend(*args, **kwargs):
    if chart.is_native_chart():
        plot.legend(*args, **kwargs)
    else:
        _legend_mpl(*args, **kwargs)

def _legend_mpl(*args, **kwargs):
    if "show" in kwargs:
        if kwargs["show"] is not None and not kwargs["show"]:
            if plt.gca().get_legend() is not None:
                plt.gca().get_legend().remove()
            return
        del kwargs["show"]
    if "loc" in kwargs and kwargs["loc"] and kwargs["loc"].startswith("outside"):
        kwargs2 = _legend_mpl_loc_outside_args(kwargs["loc"])
        del kwargs["loc"]
        kwargs.update(kwargs2)
    plt.legend(*args, **kwargs)

def _legend_mpl_loc_outside_args(loc):
    mapping = {
        "outside top left": ("lower left", (0,1.05)),
        "outside top center": ("lower center", (0.5,1.05)),
        "outside top right": ("lower right", (1,1.05)),
        "outside bottom left": ("upper left", (0,-0.05)),
        "outside bottom center": ("upper center", (0.5,-0.05)),
        "outside bottom right": ("upper right", (1,-0.05)),
        "outside left top": ("upper right", (-0.03, 1)),
        "outside left center": ("center right", (-0.03,0.5)),
        "outside left bottom": ("lower right", (-0.03,0)),
        "outside right top": ("upper left", (1.03,1)),
        "outside right center": ("center left", (1.03,0.5)),
        "outside right bottom": ("lower left", (1.03,0)),
    }
    if loc not in mapping:
        raise ValueError("loc='{}' is not recognized/supported".format(loc))
    (anchorloc, relpos) = mapping[loc]
    return {"loc" : anchorloc, "bbox_to_anchor" : relpos}
