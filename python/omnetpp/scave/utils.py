"""
A collection of utility function for data manipulation and plotting, built
on top of Pandas data frames and the `chart` and `plot` packages from `omnetpp.scave`.
Functions in this module have been written largely to the needs of the
chart templates that ship with the IDE.

There are some functions which are mandatory elements in a chart script. These are:

If you want style settings in the chart dialog to take effect:
 `preconfigure_plot()`
 `postconfigure_plot()`

If you want image/data export to work:
- `export_image_if_needed()`
- `export_data_if_needed()`

"""

import random, sys, os, string, re, math, importlib
import numpy as np
import scipy.stats as st
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
from itertools import cycle
from omnetpp.scave import chart, plot, vectorops

# color and marker cycles, with defaults in case _initialize_cycles() is not called
_marker_cycle = cycle(list("osv^<>pDd"))
_color_cycle = cycle(["C" + str(i) for i in range(10)])


def confidence_interval(alpha, data):
    """
    Returns the half-length of the confidence interval of the mean of `data`, assuming
    normal distribution, for the given confidence level `alpha`.

    Parameters:

    - `alpha` (float): Confidence level, must be in the [0..1] range.
    - `data` (array-like): An array containing the values.
    """
    return st.norm.interval(alpha, loc=0, scale=st.sem(data))[1]

def split(s, sep=','):
    """
    Split a string with the given separator (by default with comma), trim
    the surrounding whitespace from the items, and return the result as a
    list. Returns an empty list for an empty or all-whitespace input string.
    (Note that in contrast, `s.split(',')` will return an empty array,
    even for `s=''`.)
    """
    parts = s.split(sep)
    parts = [p.strip() for p in parts]
    if parts == ['']:
        parts = []
    return parts

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

    Returns:

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
                 "processid", "datetime", "datetimef", "runnumber", "seedset",
                 "iterationvars", "iterationvarsf", "iterationvarsd",
                 "source", "interpolationmode", "enum", "title", "unit",
                 "legend", "comment"]

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
    TODO legend column, comment column
    """
    comment = row.comment if hasattr(row, 'comment') else None
    comment_str = " (" + comment + ")" if type(comment) is str and comment else ""
    if hasattr(row, 'legend'):
        return row.legend + comment_str

    if len(legend_cols) == 1:
        return str(row[legend_cols[0][0]]) + comment_str
    return ", ".join([col + "=" + str(row[i]) for i, col in legend_cols]) + comment_str


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


def pick_two_columns(df):
    """
    Choose two columns from the dataframe which best partitions the rows
    of the dataframe, and returns their names as a pair. Returns (`None`, `None`)
    if no such pair was found. This method is useful for creating e.g. a bar plot.
    """
    title_col, label_cols = extract_label_columns(df)
    label_cols = [l[1] for l in label_cols]
    if len(label_cols) == 0:
        return None, None
    if len(label_cols) == 1:
        if label_cols[0] == title_col:
            return None, None
        else:
            return title_col, label_cols[0]
    if len(label_cols) >= 2:
        return label_cols[0], label_cols[1]


def assert_columns_exist(df, cols, message="Expected column missing from DataFrame"):
    """
    Ensures that the dataframe contains the given columns. If any of them are missing,
    the function raises an error with the given message.

    Parameters:

    `cols` *(list of strings)*: column names to check.
    """
    for c in cols:
        if c not in df:
            plot.set_warning(message + ": " + c)
            exit(1)


def parse_rcparams(rc_content):
    """
    Accepts a multiline string that contains rc file content in Matplotlib's
    RcParams syntax, and returns its contents as a dictionary. Parse errors
    and duplicate keys are reported via exceptions.
    """
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
    """
    Only useful for Matplotlib plots. It causes the x tick labels to be rotated
    by the minimum amount necessary so that they don't overlap. Note that the
    necessary amount of rotation typically depends on the horizontal zoom level.
    """
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


# inspired by: https://stackoverflow.com/a/11562898/635587
# and https://stackoverflow.com/q/11551049#comment77920445_11562898
def _make_scroll_navigable(figure):
    """
    Only useful for Matplotlib plots. It causes the plot to respond to mouse
    wheel events in the following way, regardless of navigation mode:
    - Without modifiers: vertical pan
    - With Shift: horizontal pan
    - With Ctrl: zoom
    """
    def zoom_fun(event):
        ax = event.inaxes

        if ax is None:
            return

        SCALING_FACTOR = 1.5
        PANNING_FACTOR = 0.1

        cur_xlim = ax.get_xlim()
        cur_ylim = ax.get_ylim()
        cur_xrange = cur_xlim[1] - cur_xlim[0]
        cur_yrange = cur_ylim[1] - cur_ylim[0]
        xdata = event.xdata
        ydata = event.ydata
        direction = np.sign(event.step)

        if event.key is None: # vertical pan
            delta = cur_yrange * direction * PANNING_FACTOR
            ax.set_ylim([cur_ylim[0] + delta, cur_ylim[1] + delta])
        elif event.key == "shift": # horizontal pan
            delta = cur_xrange * -direction * PANNING_FACTOR
            ax.set_xlim([cur_xlim[0] + delta, cur_xlim[1] + delta])
        elif event.key == "control": # zoom
            scale = math.pow(SCALING_FACTOR, direction)
            ax.set_xlim([xdata - (xdata-cur_xlim[0]) / scale,
                        xdata + (cur_xlim[1]-xdata) / scale])
            ax.set_ylim([ydata - (ydata-cur_ylim[0]) / scale,
                        ydata + (cur_ylim[1]-ydata) / scale])

        plt.draw()

    figure.canvas.mpl_connect('scroll_event', zoom_fun)


# source: https://stackoverflow.com/a/39789718/635587
def customized_box_plot(percentiles, labels=None, axes=None, redraw=True, *args, **kwargs):
    """
    Generates a customized box-and-whiskers plot based on explicitly specified
    percentile values. This method is necessary because pyplot.boxplot() insists
    on computing the stats from the raw data (which we often don't have) itself.

    The data are in the `percentiles` argument, which should be list of tuples.
    One box will be drawn for each tuple. Each tuple contains 6 elements (or 5,
    because the last one is optional):

    (*q1_start*, *q2_start*, *q3_start*, *q4_start*, *q4_end*, *fliers*)

    The first five elements have following meaning:
    - *q1_start*: y coord of bottom whisker cap
    - *q2_start*: y coord of bottom of the box
    - *q3_start*: y coord of median mark
    - *q4_start*: y coord of top of the box
    - *q4_end*: y coord of top whisker cap

    The last element, *fliers*, is a list, containing the values of the
    outlier points.

    x coords of the box-and-whiskers plots are automatic.

    Parameters:
    - `percentiles`: The list of tuples.
    - `labels`: If provided, the legend labels for the boxes.
    - `axes`: The axes object of the plot.
    - `redraw`: If False, redraw is deferred.
    - `args`, `kwargs`: Passed to `axes.boxplot()`.
    """

    if axes is None:
        axes = plt.gca()

    min_y, max_y = float('inf'), -float('inf')

    if labels is not None:
        if len(labels) != len(percentiles):
            raise ValueError("There must be as many labels as elements in the percentiles list")

    for box_no, pdata in enumerate(percentiles):
        color = next(_color_cycle)
        box_plot = axes.boxplot([-9, -4, 2, 4, 9], positions=[box_no], widths=[0.5],
            showmeans=True, meanprops=dict(marker='+', markeredgecolor=mpl.rcParams["axes.facecolor"]),
            boxprops=dict(facecolor=color), whiskerprops=dict(color=color, linewidth=2), capprops=dict(linewidth=1.5),
            patch_artist=True, *args, **kwargs)

        if labels is not None:
            # boxplot does not register its patches as legend handles, as it only wants to
            # put the labels on axis ticks.
            # And we also don't want to explicitly specify the legend contents, because the
            # legend() call is sometime later, on postconfigure_plot. Only one thing remains:
            # To make a "fake" bar for each box, and remove it - but not entirely (as that)
            # would also remove it from the Legend... (same with hiding) - so its single
            # patch is removed instead...
            bar = axes.bar([0], [0], color=color, label=labels[box_no])
            bar.patches[0].remove()

        if len(pdata) == 6:
            (q1_start, q2_start, q3_start, q4_start, q4_end, fliers) = pdata
        elif len(pdata) == 5:
            (q1_start, q2_start, q3_start, q4_start, q4_end) = pdata
            fliers = []
        else:
            raise ValueError("Percentile arrays for customized_box_plot must have either 5 or 6 values")

        fliers = np.array(fliers)

        # Lower cap
        box_plot['caps'][0].set_ydata([q1_start, q1_start])
        # xdata is determined by the width of the box plot

        # Lower whiskers
        box_plot['whiskers'][0].set_ydata([q1_start, q2_start])

        # Higher cap
        box_plot['caps'][1].set_ydata([q4_end, q4_end])

        # Higher whiskers
        box_plot['whiskers'][1].set_ydata([q4_start, q4_end])

        # Box
        path = box_plot['boxes'][0].get_path()
        path.vertices[0][1] = q2_start
        path.vertices[1][1] = q2_start
        path.vertices[2][1] = q4_start
        path.vertices[3][1] = q4_start
        path.vertices[4][1] = q2_start

        box_plot['means'][0].set_ydata([q3_start, q3_start])
        box_plot['medians'][0].set_visible(False)

        # Outliers
        if len(fliers) > 0:
            # If outliers exist
            box_plot['fliers'][0].set(xdata = [box_no] * len(fliers),
                                        ydata = fliers)

            min_y = min(q1_start, min_y, fliers.min())
            max_y = max(q4_end, max_y, fliers.max())
        else:
            min_y = min(q1_start, min_y)
            max_y = max(q4_end, max_y)

    mid_y = (min_y + max_y) / 2
    # The y axis is rescaled to fit the new box plot completely with 10%
    # of the maximum value at both ends
    axes.set_ylim([mid_y - (mid_y - min_y) * 1.25, mid_y + (max_y - mid_y) * 1.25])

    axes.set_xlim(-0.5, len(percentiles)-0.5)
    axes.set_xticks([])
    axes.set_xticklabels([])

    # If redraw is set to true, the canvas is updated.
    if redraw:
        axes.figure.canvas.draw()


def _interpolationmode_to_drawstyle(interpolationmode, hasenum):
    """
    Converts an OMNeT++-style interpolation constant ('none', 'linear',
    'sample-hold', etc.) to Matplotlib draw styles.
    """
    interp = interpolationmode if interpolationmode else 'sample-hold' if hasenum else None
    if not interp:
        ds = "default"
    elif interp == "none":
        ds = "none"
    elif interp == "linear":
        ds = "default"
    elif interp == "sample-hold":
        ds = "steps-post"
    elif interp == "backward-sample-hold":
        ds = 'steps-pre'
    else:
        print("Unknown interpolationmode:", interp, file=sys.stderr)
        ds = None

    return ds


def _make_line_args(props, t, df):
    global _marker_cycle
    style = dict()

    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("linestyle"):
        style["linestyle"] = get_prop("linestyle")  # note: must precede 'drawstyle' setting

    ds = get_prop("drawstyle")
    if not ds or ds == "auto":
        interpolationmode = t.interpolationmode if "interpolationmode" in df else None
        hasenum = "enum" in df
        ds = _interpolationmode_to_drawstyle(interpolationmode, hasenum)

    if ds == "none":
        style["linestyle"] = " "
        style["drawstyle"] = "default"  # or any valid value, doesn't matter
    else:
        style["drawstyle"] = ds if ds != "linear" else "default"

    if get_prop("linecolor"):
        style["color"] = get_prop("linecolor")
    else:
        style["color"] = next(_color_cycle)

    if get_prop("linewidth"):
        style["linewidth"] = get_prop("linewidth")

    if not get_prop("marker") or get_prop("marker") == "auto":
        style["marker"] = next(_marker_cycle)
    elif get_prop("marker") == 'none':
        style["marker"] = ' '
    else:
        style["marker"] = get_prop("marker")[0] # take first character only, as Matplotlib uses 1-char codes; this allows us to include a description

    if get_prop("markersize"):
        style["markersize"] = get_prop("markersize")

    # Special case: not letting both the lines and the markers to become
    # invisible automatically. Only if the user specifically asks for it.
    if style["marker"] == ' ' and style["linestyle"] == ' ':
        orig_ds = get_prop("drawstyle")
        if not orig_ds or orig_ds == "auto":
            style["marker"] = '.'

    return style

def _make_histline_args(props, t, df): # ??? is t and df needed at all?
    style = dict()

    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("color"):
        style["color"] = get_prop("color")
    else:
        style["color"] = next(_color_cycle)

    if get_prop("linewidth"):
        style["linewidth"] = get_prop("linewidth")

    ds = get_prop("drawstyle")
    if ds == "Solid":
        style["histtype"] = "stepfilled"
    elif ds == "Outline":
        style["histtype"] = "step"

    style["density"] = _parse_optional_bool(get_prop("normalize"))
    style["cumulative"] = _parse_optional_bool(get_prop("cumulative"))

    return style


def _make_bar_args(props, df): # ??? is df needed at all? should we also get the column name?
    style = dict()

    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("color"):
        style["color"] = get_prop("color")
    else:
        style["color"] = next(_color_cycle)

    return style


def get_names_for_title(df, props):
    """
    Produces input for the plot title. unique values from the "title" or "name" column
    TODO
    "legend_labels" => "result titles" or "result names"

    """
    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("legend_labels") == "result titles" and "title" in df:
        series = df["title"].fillna(df["name"])
    else:
        series = df["name"]

    return series.unique()


def set_plot_title(title, suggested_chart_name=None):
    """
    Sets the plot title.
    TODO also sets the suggested chart name (for saving the temp chart)
    """
    plot.title(title)
    chart.set_suggested_chart_name(suggested_chart_name if suggested_chart_name is not None else title)


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


def plot_bars(df, props, variable_name=None, errors_df=None):
    """
    Creates a bar plot from the dataframe, with styling and additional input
    coming from the properties. Each column in the dataframe defines a series.

    Column names serve as labels for the legend. Group names (displayed on the
    x axis) are taken from the row index. The names of the two indices also appear
    in the legend and as x axis label. (This is useful if column and group names
    are values of a variable, and the index name contains the name of the variable.)
    The name of the variable represented by the values can be passed in as
    the `variable_name` argument (as it is not present in the dataframe); if so,
    it will become the y axis label.

    Error bars can be drawn by providing an extra dataframe of identical
    dimensions as the main one. Error bars will protrude by the values in the
    errors dataframe both up and down (i.e. range is 2x error).

    Colors are assigned automatically. The `cycle_seed` property allows you
    to select other combinations if the default one is not suitable.

    Parameters:

    - `df`: the dataframe
    - `props` (dict): the properties
    - `variable_name` (string): The name of the variable represented by the values.
    - `errors_df`: dataframe with the errors (in y axis units)

    Notable properties that affect the plot:
    - `baseline`: The y value at which the x axis is drawn.
    - `bar_placement`: Selects the arrangement of bars: aligned, overlap, stacked, etc.
    - `xlabel_rotation`: Amount of counter-clockwise rotation of x axis labels a.k.a. group names, in degrees.
    - `title`: Plot title (autocomputed if missing).
    - `cycle_seed`: Alters the sequence in which colors are assigned to series.
    """
    p = plot if chart.is_native_chart() else plt

    def get_prop(k):
        return props[k] if k in props else None

    group_fill_ratio = 0.8
    aligned_bar_fill_ratio = 0.9
    overlap_visible_fraction = 1 / 3

    # used only by the mpl charts
    xs = np.arange(len(df.index), dtype=np.float64)  # the x locations for the groups
    width = group_fill_ratio / len(df.columns)  # the width of the bars
    bottoms = np.zeros_like(xs)
    group_increment = 0.0

    baseline = get_prop("baseline")
    if baseline:
        if chart.is_native_chart(): # is this how this should be done?
            plot.set_property("Bars.Baseline", baseline)
        else:
            bottoms += float(baseline)

    extra_args = dict()

    placement = get_prop("bar_placement")
    if placement:
        if chart.is_native_chart(): # is this how this should be done?
            plot.set_property("Bar.Placement", placement)
        else:
            extra_args["bottom"] = bottoms
            if placement == "InFront":
                width = group_fill_ratio
            elif placement == "Stacked":
                width = group_fill_ratio
            elif placement == "Aligned":
                width = group_fill_ratio / len(df.columns)
                group_increment = width
                xs -= width * (len(df.columns)-1)/2
                width *= aligned_bar_fill_ratio
            elif placement == "Overlap":
                width = group_fill_ratio / (1 + len(df.columns) * overlap_visible_fraction)
                group_increment = width * overlap_visible_fraction
                extra_parts = (1.0 / overlap_visible_fraction - 1)
                xs += width / extra_parts - (len(df.columns) + extra_parts) * width * overlap_visible_fraction / 2

    for i, column in enumerate(df):
        style = _make_bar_args(props, df)

        if not chart.is_native_chart(): # FIXME: noot pretty...
            extra_args['zorder'] = 1 - (i / len(df.columns) / 10)

        label = df.columns.name + "=" + _to_label(column) if df.columns.name else _to_label(column)
        ys = df[column].values
        p.bar(xs, ys, width, label=label, **extra_args, **style)

        if not chart.is_native_chart() and errors_df is not None:
            plt.errorbar(xs, ys + bottoms, yerr=errors_df[column], capsize=float(get_prop("cap_size") or 4), **style, linestyle="none", ecolor=mpl.rcParams["axes.edgecolor"])

        xs += group_increment
        if placement == "Stacked":
            bottoms += df[column].values

    rotation = get_prop("xlabel_rotation")
    if rotation:
        rotation = float(rotation)
    else:
        rotation = 0
    p.xticks(list(range(len(df.index))), list([_to_label(i) for i in df.index.values]), rotation=rotation)

    # Add some text for labels, title and custom x-axis tick labels, etc.
    groups = df.index.names
    series = df.columns.names

    p.xlabel(_to_label(groups))

    title = ""
    if variable_name:
        p.ylabel(variable_name)
        title = variable_name
        groups_series_str = [str(gs) for gs in groups+series]
        if groups_series_str and groups_series_str[0]:
            title += " by " + ", ".join(groups_series_str)

    set_plot_title(get_prop("title") or title)


def plot_vectors(df, props, legend_func=make_legend_label):
    """
    Creates a line plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a series.

    Colors and markers are assigned automatically. The `cycle_seed` property
    allows you to select other combinations if the default one is not suitable.

    The legend is normally computed automatically from columns which best
    differentiate among the series. There are also multiple ways to influence
    the labels: the `legend_labels` property, the `comment` and `legend` dataframe
    columns, and also the `legend_func` argument.

    Parameters:

    - `df`: the dataframe
    - `props` (dict): the properties
    - `legend_func`: the function to produce custom legend labels

    Columns of the dataframe:

    - `vectime`, `vecvalue` (Numpy `ndarray`'s of matching sizes): the x and y coordinates for the plot
    - `interpolationmode` (str, optional): this column normally comes from a result attribute, and determines how the points will be connected
    - `legend` (optional): legend label for the series; if missing, legend labels are derived from other columns
    - `name`, `title`, `module`, etc. (optional): provide input for the legend
    - `comment` (optional): will be appended to the legend labels

    Notable properties that affect the plot:

    - `title`: plot title (autocomputed if missing)
    - `legend_labels`: selects whether to prefer the `name` or the `title` column for the legend
    - `drawstyle`: Matplotlib draw style; if present, it overrides the draw style derived from `interpolationmode`.
    - `linestyle`, `linecolor`, `linewidth`, `marker`, `markersize`: styling
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    p = plot if chart.is_native_chart() else plt

    def get_prop(k):
        return props[k] if k in props else None

    column = "name" if get_prop("legend_labels") == "result names" else "title"
    title_col, legend_cols = extract_label_columns(df, column)

    for t in df.itertuples(index=False):
        style = _make_line_args(props, t, df)
        p.plot(t.vectime, t.vecvalue, label=legend_func(legend_cols, t), **style)

    title = get_prop("title") or make_chart_title(df, title_col, legend_cols)
    set_plot_title(title)

def plot_histograms(df, props, legend_func=make_legend_label):
    """
    Creates a histogram plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a histogram.

    Colors are assigned automatically.  The `cycle_seed` property allows you to
    select other combinations if the default one is not suitable.

    The legend is normally computed automatically from columns which best
    differentiate among the histograms. There also are multiple ways to influence
    the labels: the `legend_labels` property, the `comment` and `legend` dataframe
    columns, and also the `legend_func` argument.

    Parameters:

    - `df`: The dataframe.
    - `props` (dict): The properties.
    - `legend_func` (function): The function to produce custom legend labels.
       See `utils.make_legend_label()` for prototype and semantics.

    Columns of the dataframe:

    - `binedges`, `binvalues` (array-like, `len(binedges)==len(binvalues)+1`):
       The bin edges and the bin values (count or sum of weights) for the histogram.
    - `min`, `max`, `underflows`, `overflows` (float, optional): The minimum/maximum
       values, and the bin values for the underflow/overflow bins. These four columns
       must either be all present or all absent from the dataframe.
    - `legend` (string, optional): Legend label for the series. If missing,
       legend labels are derived from other columns.
    - `name`, `title`, `module`, etc. (optional): Provide input for the legend.
    - `comment` (string, optional): will be appended to the legend labels.

    Notable properties that affect the plot:

    - `normalize` (bool): If true, normalize the sum of the bin values to 1. If
      `normalize` is true (and `cumulative` is false), the probability density
       function (PDF) will be displayed.
    - `cumulative` (bool): If true, show each bin as the sum of the previous bin
       values plus itself. If both `normalize` and `cumulative` are true, that
       results in the cumulative density function (CDF) being displayed.
    - `show_overflows` (bool): If true, show the underflow/overflow bins.
    - `title`: Plot title (autocomputed if missing).
    - `legend_labels`: Selects whether to prefer the `name` or the `title` column for the legend.
    - `drawstyle`: Selects whether to fill the area below the histogram line.
    - `linestyle`, `linecolor`, `linewidth`: Styling.
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    p = plot if chart.is_native_chart() else plt

    has_overflow_columns = "min" in df and "max" in df and "underflows" in df and "overflows" in df

    if not has_overflow_columns:
        if "min" in df or "max" in df or "underflows" in df or "overflows" in df:
           raise ValueError("Either all, or none, of the following columns must be present: min, max, underflows, overflows")

    def get_prop(k):
        return props[k] if k in props else None

    title_col, legend_cols = extract_label_columns(df)

    for t in df.itertuples(index=False):
        style = _make_histline_args(props, t, df)

        # We are branching here, and call the two .hist implementations
        # differently, because the MPL API does not have support for
        # underflows/overflows, so we have to "fake" them into the real
        # data as additional cells. Drawing them separately wouldn't
        # work, because that would change the histogram when the
        # "normalized" or "cumulative" transforms are done (by MPL).
        if chart.is_native_chart():
            overflow = dict(minvalue=t.min, maxvalue=t.max, underflows=t.underflows, overflows=t.overflows) if has_overflow_columns else dict()
            p.hist(t.binedges[:-1], t.binedges, weights=t.binvalues, label=legend_func(legend_cols, t), **overflow, **style)
        else:
            edges = list(t.binedges)
            values = list(t.binvalues)

            if has_overflow_columns:
                has_underflow_bin = not np.isnan(t.min) and not np.isnan(t.underflows) and t.min < edges[0]
                has_overflow_bin = not np.isnan(t.max) and not np.isnan(t.overflows) and t.max >= edges[-1]
                if has_underflow_bin:
                    edges = [t.min] + edges
                    values = [t.underflows] + values
                if has_overflow_bin:
                    edges = edges + [t.max]
                    values = values + [t.overflows]

            p.hist(edges[:-1], edges, weights=values, label=legend_func(legend_cols, t), **style)

    show_overflows = get_prop("show_overflows")
    if show_overflows and chart.is_native_chart():
        plot.set_property("Hist.ShowOverflowCell", str(_parse_optional_bool(show_overflows)).lower())

    title = get_prop("title") or make_chart_title(df, title_col, legend_cols)
    set_plot_title(title)


def _initialize_cycles(props):
    """
    Initialize the `_marker_cycle` and `_color_cycle` global variables, taking
    the `cycle_seed` property into account.
    """
    def get_prop(k):
        return props[k] if k in props else None

    global _marker_cycle
    global _color_cycle

    seed = int(get_prop('cycle_seed') or 0)
    r = random.Random(seed)

    ml = list("osv^<>pDd")
    if seed != 0:
        random.shuffle(ml, r.random)
    _marker_cycle = cycle(ml)

    prop_cycler = plt.rcParams['axes.prop_cycle']

    if chart.is_native_chart():
        num_colors = 10
    else:
        num_colors = len(prop_cycler.by_key()['color']) if "color" in prop_cycler.keys else 1

    cl = ["C" + str(i) for i in range(num_colors)]
    if seed != 0:
        random.shuffle(cl, r.random)
    _color_cycle = cycle(cl)


def _parse_vectorop_line(line):
    """
    Parses the given vector operation line, and returns its contents as a tuple.
    Helper for `perform_vector_ops()`.
    """
    line = line.strip()
    m = re.match(r"((\w+)\s*:)?\s*(([\w.]+)\.)?(\w+)?(.*)", line)  # always matches due to last group
    _, type, _, module, name, rest = m.groups()
    if not name:
        if line and not line.startswith('#'):
            raise SyntaxError("Syntax error")
        return (None, None, None, None, None)
    if not type:
        type = "apply"
    if not type in ['apply', 'compute']:
        raise SyntaxError("Syntax error near '"+type+"': must be 'apply' or 'compute' (or omitted)")
    rest = rest.strip()
    if not rest or rest.startswith('#'):
        rest = "()"
    if not rest.startswith('('):
        raise ValueError("Parenthesized argument list expected after operation name")
    try:
        def return_args(*args, **kwargs):
            return (args,kwargs)
        args, kwargs = eval(name + " " + rest, None, {name: return_args}) # let Python do the parsing, incl. comment discarding
    except SyntaxError:
        raise SyntaxError("Syntax error in argument list")
    except Exception as e:
        raise ValueError("Error in argument list: " + str(e))
    return type, module, name, args, kwargs


def _perform_vector_op(df, line):
    """
    Performs one vector operation on the dataframe. Helper for `perform_vector_ops()`.
    """
    # parse line
    type, module, name, args, kwargs = _parse_vectorop_line(line)
    if name is None: # empty line
        return df

    # look up function
    function = None
    if not module and name in globals():  # unfortunately this doesn't work, because the chart script sees a different "globals" dict than we in this module, due it being called via exec()
        function = globals()[name]
    else:
        if not module:
            module = "omnetpp.scave.vectorops"
        mod = importlib.import_module(module)
        if not name in mod.__dict__:
            raise ValueError("Vector filter function '" + name + "' not found in module '" + module + "'")
        function = mod.__dict__[name]

    # perform operation
    if type == "apply":
        df = _apply_vector_op(df, function, *args, **kwargs)
    elif type == "compute":
        df = _compute_vector_op(df, function, *args, **kwargs)
    return df


def perform_vector_ops(df, operations : str):
    """
    Performs the given vector operations on the dataframe, and returns the
    resulting dataframe. Vector operations primarily affect the `vectime`
    and `vecvalue` columns of the dataframe, which are expected to contain
    `ndarray`'s of matching lengths.

    `operations` is a multiline string where each line denotes an operation;
    they are applied in sequence. The syntax of one operation is:

    [(`compute`|`apply`) `:` ] *opname* [ `(` *arglist* `)` ] [ `#` *comment* ]

    Blank lines and lines only containing a comment are also accepted.

    *opname* is the name of the function, optionally qualified with its package name.
    If the package name is omitted, `omnetpp.scave.vectorops` is assumed.

    `compute` and `apply` specify whether the newly computed vectors will replace
    the input row in the DataFrame (*apply*) or added as extra lines (*compute*).
    The default is *apply*.

    See the contents of the `omnetpp.scave.vectorops` package for more information.
    """
    if not operations:
        return df
    line_num = 0
    for line in operations.splitlines():
        line_num += 1
        try:
            df = _perform_vector_op(df, line)
        except Exception as e:
            context = " in Vector Operations line " + str(line_num) + " \"" + line.strip() + "\""
            raise type(e)(str(e) + context).with_traceback(sys.exc_info()[2]) # re-throw with context
    return df


def _apply_vector_op(df, operation, *args, **kwargs):
    """
    Process a vector operation with the `apply` prefix. Helper for `perform_vector_ops()`.
    """
    if operation == vectorops.aggregate:
        return vectorops.aggregate(df, *args, **kwargs)
    elif operation == vectorops.merge:
        return vectorops.merge(df)
    else:
        condition = kwargs.pop('condition', None)
        clone = df.copy()
        clone = clone.transform(lambda row: operation(row.copy(), *args, **kwargs) if not condition or condition(row) else row, axis='columns')
        return clone


def _compute_vector_op(df, operation, *args, **kwargs):
    """
    Process a vector operation with the `compute` prefix. Helper for `perform_vector_ops()`.
    """
    if operation == vectorops.aggregate:
        return df.append(vectorops.aggregate(df, *args, **kwargs), sort=False)
    elif operation == vectorops.merge:
        return df.append(vectorops.merge(df), sort=False)
    else:
        condition = kwargs.pop('condition', None)
        clone = df.copy()
        clone = clone.transform(lambda row: operation(row.copy(), *args, **kwargs) if not condition or condition(row) else row, axis='columns')
        return df.append(clone, sort=False)


def preconfigure_plot(props):
    """
    Configures the plot according to the given properties, which normally
    get their values from setting in the "Configure Chart" dialog.
    Calling this function before plotting was performed should be a standard
    part of chart scripts.

    A partial list of properties taken into account for native plots:
    - property keys understood by the plot widget, see `plot.get_supported_property_keys()`
    - properties listed in the `plot.properties` property

    And for Matplotlib plots:
    - `plt.style`
    - properties listed in the `matplotlibrc` property
    - properties prefixed with `matplotlibrc.`

    Parameters:
    - `props` (dict): the properties
    """
    def get_prop(k):
        return props[k] if k in props else None

    if chart.is_native_chart():
        supported_keys = plot.get_supported_property_keys()
        plot.set_properties({ k: v for k, v in props.items() if k in supported_keys})
        plot.set_properties(parse_rcparams(get_prop("plot.properties") or ""))
    else:
        if get_prop("plt.style"):
            plt.style.use(get_prop("plt.style"))
        mpl.rcParams.update(_filter_by_key_prefix(props,"matplotlibrc."))
        mpl.rcParams.update(parse_rcparams(get_prop("matplotlibrc") or ""))
        _make_scroll_navigable(plt.gcf())

    _initialize_cycles(props)


def postconfigure_plot(props):
    """
    Configures the plot according to the given properties, which normally
    get their values from setting in the "Configure Chart" dialog.
    Calling this function after plotting was performed should be a standard part
    of chart scripts.

    A partial list of properties taken into account:
    - `yaxis_title`, `yaxis_title`, `xaxis_min`,  `xaxis_max`, `yaxis_min`,
      `yaxis_max`, `xaxis_log`, `yaxis_log`, `legend_show`, `legend_border`,
      `legend_placement`, `grid_show`, `grid_density`

    Parameters:
    - `props` (dict): the properties
    """
    p = plot if chart.is_native_chart() else plt

    def get_prop(k):
        return props[k] if k in props else None

    if get_prop("yaxis_title"):
        p.xlabel(get_prop("xaxis_title"))
    if get_prop("yaxis_title"):
        p.ylabel(get_prop("yaxis_title"))

    if get_prop("xaxis_min"):
        p.xlim(left=float(get_prop("xaxis_min")))
    if get_prop("xaxis_max"):
        p.xlim(right=float(get_prop("xaxis_max")))
    if get_prop("yaxis_min"):
        p.ylim(bottom=float(get_prop("yaxis_min")))
    if get_prop("yaxis_max"):
        p.ylim(top=float(get_prop("yaxis_max")))

    if get_prop("xaxis_log"):
        p.xscale("log" if _parse_optional_bool(get_prop("xaxis_log")) else "linear")
    if get_prop("yaxis_log"):
        p.yscale("log" if _parse_optional_bool(get_prop("yaxis_log")) else "linear")

    plot.legend(show=_parse_optional_bool(get_prop("legend_show")),
           frameon=_parse_optional_bool(get_prop("legend_border")),
           loc=get_prop("legend_placement"))

    p.grid(_parse_optional_bool(get_prop("grid_show")),
             "major" if (get_prop("grid_density") or "").lower() == "major" else "both") # grid_density is "Major" or "All"

    if not chart.is_native_chart():
        plt.tight_layout()

def export_image_if_needed(props):
    """
    If a certain property is set, save the plot in the selected image format.
    Calling this function should be a standard part of chart scripts, as it is what
    makes the "Export image" functionality of the IDE and `opp_charttool` work.

    Note that for export, even IDE-native charts are rendered using Matplotlib.

    The properties that are taken into account:

    - `export_image` (boolean): Controls whether to perform the exporting. This is
       normally `false`, and only set to `true` by the IDE or opp_charttool when
       image export is requested.
    - `image_export_format`: The default is SVG. Accepted formats (and their names) are the ones supported by Matplotlib.
    - `image_export_folder`: The folder in which the image file is to be created.
    - `image_export_filename`: The output file name. If missing or empty, a
       sanitized version of the chart name is used.
    - `image_export_width`: Image width in inches (default: 6")
    - `image_export_height`: Image height in inches (default: 4")
    - `image_export_dpi`: DPI setting, default 96. For raster image formats, the
       image dimensions are produced as width (or height) times dpi.

    Note that these properties come from two sources to allow meaningful batch
    export. `export_image`, `image_export_format`, `image_export_folder` and
    `image_export_dpi` come from the export dialog because they are common
    to all charts, while `image_export_filename`, `image_export_width` and
    `image_export_height` come from the chart properties because they are
    specific to each chart. Note that `image_export_dpi` is used for controlling
    the resolution (for raster image formats) while letting charts maintain
    their own aspect ratio and relative sizes.

    Parameters:
    - `props` (dict): the properties
    """
    def get_prop(k):
        return props[k] if k in props else None

    if _parse_optional_bool(get_prop("export_image")):
        format = get_prop("image_export_format") or "svg"
        folder = get_prop("image_export_folder") or os.getcwd()
        filename = get_prop("image_export_filename") or _sanitize_filename(chart.get_name())
        filepath = os.path.join(folder, filename) + "." + format
        width = float(get_prop("image_export_width") or 6)
        height = float(get_prop("image_export_height") or 4)
        dpi = get_prop("image_export_dpi") or "96"

        print("exporting image to: '" + filepath + "' as " + format)

        plt.gcf().set_size_inches(width, height)
        plt.savefig(filepath, format=format, dpi=int(dpi))


def export_data_if_needed(df, props):
    """
    If a certain property is set, save the dataframe in CSV format.
    Calling this function should be a standard part of chart scripts, as it is what
    makes the "Export data" functionality of the IDE and `opp_charttool` work.

    The properties that are taken into account:

    - `export_data` (boolean): Controls whether to perform the exporting. This is
       normally `false`, and only set to `true` by the IDE or opp_charttool when
       data export is requested.
    - `data_export_folder`: The folder in which the CSV file is to be created.
    - `data_export_filename`: The output file name. If missing or empty, a
       sanitized version of the chart name is used.

    Note that these properties come from two sources to allow meaningful batch
    export. `export_data` and `image_export_folder` come from the export dialog
    because they are common to all charts, and `image_export_filename` comes
    from the chart properties because it is specific to each chart.

    Parameters:
    - `df`: the dataframe to save
    - `props` (dict): the properties
    """
    def get_prop(k):
        return props[k] if k in props else None

    def printer(arr):
        return np.array_str(arr)

    if _parse_optional_bool(get_prop("export_data")):
        format = "csv"
        folder = get_prop("data_export_folder") or os.getcwd()
        filename = get_prop("data_export_filename") or _sanitize_filename(chart.get_name())
        filepath = os.path.join(folder, filename) + "." + format

        print("exporting data to: '" + filepath + "' as " + format)

        old_opts = np.get_printoptions()
        np.set_printoptions(threshold=np.inf, linewidth=np.inf)
        np.set_string_function(printer, False)
        pd.set_option('display.max_columns', None)
        pd.set_option('display.max_colwidth', None)
        df.to_csv(filepath)
        np.set_string_function(None, False)
        np.set_printoptions(**old_opts)


def _sanitize_filename(filename):
    valid_chars = "-_.() %s%s" % (string.ascii_letters, string.digits)
    return ''.join(c for c in filename if c in valid_chars)
