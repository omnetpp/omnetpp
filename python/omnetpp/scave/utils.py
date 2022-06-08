"""
A collection of utility function for data manipulation and plotting, built
on top of Pandas data frames and the `chart` and `ideplot` packages from
`omnetpp.scave`. Functions in this module have been written largely to the
needs of the chart templates that ship with the IDE.

There are some functions which are (almost) mandatory elements in a chart script.
These are the following.

If you want style settings in the chart dialog to take effect:
- `preconfigure_plot()`
- `postconfigure_plot()`

If you want image/data export to work:
- `export_image_if_needed()`
- `export_data_if_needed()`
"""

import random, sys, os, string, re, math, importlib
import numpy as np
import scipy.stats as st
import pandas as pd
import itertools as it
import matplotlib as mpl
import matplotlib.pyplot as plt
from omnetpp.scave import chart, ideplot, vectorops

from ._version import __version__

def _check_version(module, required):
    def parse_version(v: str):
        import re
        m = re.search(R"(\d+)\.(\d+)\.(\d+).*", v)
        return int(m.group(1)), int(m.group(2)), int(m.group(3))

    def too_old(actual, required):
        major, minor, patch = parse_version(actual)
        rmajor, rminor, rpatch = parse_version(required)

        if major != rmajor:
            return major < rmajor

        if minor != rminor:
            return minor < rminor

        return patch < rpatch

    if too_old(module.__version__, required):
        print("WARNING: '" + module.__name__ + "' is too old, some analysis tool functionality may be broken. "
              "Required: " + required + ", present: " + module.__version__ + ". "
              "Try running `python3 -m pip install --user --upgrade " + module.__name__ + "` to upgrade."
              , file=sys.stderr)

_check_version(np, "1.18.0")  # Dec 22, 2019
_check_version(pd, "1.0.0")  # January 29, 2020
_check_version(mpl, "3.0.0")  # Sep 19, 2018


# color and marker cycles, with defaults in case _initialize_cycles() is not called
_marker_cycle = it.cycle(list("osv^<>pDd"))
_color_cycle = it.cycle(["C" + str(i) for i in range(10)])

# Because str.removeprefix was only added in Python 3.9
def _removeprefix(str, prefix):
    return str[len(prefix):] if str.startswith(prefix) else str

# There are many potential ways to add digit grouping to numbers.
# None of them were good enough:
#  - They all can only use "," or "_" as group separators
#  - And none of them added grouping to the fractional part
#  - Or they printed numbers with silly precision,
#    either fixed, or too low, or too high.
# So, instead, take the built-in tick labels, and add digit grouping
# to them manually, after the fact, using good ol' regices.
class _DigitGroupingFormatter(mpl.ticker.ScalarFormatter):
    def __call__(self, x, pos=None):

        SEP = '\u2009' # Unicode "Thin Space"

        result = super().__call__(x, pos)
        originalresult = result

        if "." in result:
            # For numbers with decimal separators in them:
            # First replace any digit followed by any number of three-digit-groups,
            # and a period (so, the whole part); with itself followed by a space.
            result = re.sub(r'(\d)(?=(\d{3})+\.)', r'\1' + SEP, result)
            # Then replace any remaining three-digit-groups (in the fractional part)
            # with itself followed by a space.
            result = re.sub(r'(\d{3})(?=\d)', r'\1' + SEP, result)
        else:
            # For integers, do the same thing as with the whole part of fractions,
            # but don't require the period at the end.
            result = re.sub(r'(\d)(?=(\d{3})+(?!\d))', r'\1' + SEP, result)

        assert(result.replace(SEP, "") == originalresult)

        # Add back the exponent to every value manually, as we don't want to see it
        # extracted into a common order of magnitude, which is disabled by `get_offset()`.
        # (except if it has no digits in it other than 0)
        if self.orderOfMagnitude != 0 and re.search(r'[1-9]', result):
            result += "e" + str(self.orderOfMagnitude)

        return result

    # The axis-common order of magnitude (if scientific notation is used) is also returned
    # in this, but we append it to each individual tick label instead.
    def get_offset(self):
        return ""


# Note: must be at the top, because it appears in other functions' arg list as default
def make_legend_label(legend_cols, row, props={}):
    """
    Produces a reasonably good label text (to be used in a chart legend) for a result row from
    a DataFrame. The legend label is produced as follows.

    First, a base version of the legend label is produced:

    1. If the DataFrame contains a `legend` column, its content is used
    2. Otherwise, if there is a `legend_format` property, it is used as a format string
       for producing the legend label. The format string may contain references to other
       columns of the DataFrame in the "$name" or "${name}" form.
    3. Otherwise, the legend label is concatenated from the columns listed in `legend_cols`,
       a list whose contents is usually produced using the `extract_label_columns()` function.

    Second, if there is a `legend_replacements` property, the series of regular expression
    find/replace operations described in it are performed. `legend_replacements` is expected
    to be a multi-line string, where each line contains a replacement in the customary
    "/findstring/replacement/" form. "findstring" should be a valid regex, and "replacement"
    is a string that may contain match group references ("\1", "\2", etc.). If "/" is unsuitable
    as separator, other characters may also be used; common choices are "|" and "!".
    Similar to the format string (`legend_format`), both "findstring" and "replacement"
    may contain column references in the "$name" or "${name}" form. (Note that "findstring"
    may still end in "$" to match the end of the string, it won't collide with column references.)

    Possible errors:

    - References to nonexistent columns in `legend_format` or `legend_replacements` (`KeyError`)
    - Malformed regex in the "findstring" parts of `legend_replacements` (`re.error`)
    - Invalid group reference in the "replacement" parts of `legend_replacements` (`re.error`)

    Parameters:

    - `row` (named tuple): The row from the dataframe.
    - `props` (dict): The properties that control how the legend is produced
    - `legend_cols` (list of strings): The names of columns chosen for the legend.
    """

    def get_prop(k):
        return props[k] if k in props else None

    def substitute_columns(format, row, where):
        try:
            return string.Template(format).substitute(row._asdict())
        except KeyError as ex:
            raise chart.ChartScriptError("Unknown column reference ${} {}".format(ex.args[0], where)) from ex
        except ValueError as ex:
            raise chart.ChartScriptError("Error {}: {}".format(where, ex.args[0])) from ex

    legend_isautomatic = get_prop('legend_automatic')
    legend_format = get_prop('legend_format')

    if hasattr(row, 'legend'):
        legend = row.legend
    elif legend_format and legend_isautomatic != "true":
        legend = substitute_columns(legend_format, row, "in legend format string")
    else:
        def fmt(col):
            if col in ["name", "title"]:
                prefix = ""
            elif col in ["module", "moduledisplaypath"]:
                prefix = " in "
            else:
                prefix = ", " + col + "="
            return prefix + str(getattr(row, col))
        legend = "".join([fmt(col) for col in legend_cols])
        legend = _removeprefix(_removeprefix(legend, ", "), " in ")

    legend_replacements = get_prop('legend_replacements')
    if legend_replacements:
        lines = [li.strip() for li in legend_replacements.split('\n') if li.strip()]
        for line in lines:
            if line.startswith("#"):
                continue
            m = re.search(r"^(.)(.*)\1(.*)\1$", line)
            if not m:
                raise chart.ChartScriptError("Invalid line in 'legend_replacements', '/foo/bar/' syntax expected: '" + line + "'")
            else:
                findstr = m.group(2)
                findstr = re.sub(r'\$$', '$$', findstr) # if ends with "$" (regex end-of-line), make it "$$" so that string.Template won't mistake it for invalid variable reference
                findstr = substitute_columns(findstr, row, "in legend label replacement "+line)
                replacement = substitute_columns(m.group(3), row, "in legend label replacement "+line)
                try:
                    legend = re.sub(findstr, replacement, legend)
                except re.error as ex:
                    raise chart.ChartScriptError("Regex error in legend replacement '{}': {}".format(line, str(ex))) from ex

    return legend


def plot_bars(df, errors_df=None, meta_df=None, props={}):
    """
    Creates a bar plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a series.

    Group names (displayed on the x axis) are taken from the column index.

    The name of the variable represented by the values can be passed in as
    the `variable_name` argument (as it is not present in the dataframe); if so,
    it will become the y axis label.

    Error bars can be drawn by providing an extra dataframe of identical
    dimensions as the main one. Error bars will protrude by the values in the
    errors dataframe both up and down (i.e. range is 2x error).

    To make the legend labels customizable, an extra dataframe can be provided,
    which contains any columns of metadata for each series.

    Colors are assigned automatically. The `cycle_seed` property allows you
    to select other combinations if the default one is not suitable.

    Parameters:

    - `df`: the dataframe
    - `props` (dict): the properties
    - `variable_name` (string): The name of the variable represented by the values.
    - `errors_df`: dataframe with the errors (in y axis units)
    - `meta_df`: dataframe with the metadata about each series

    Notable properties that affect the plot:
    - `baseline`: The y value at which the x axis is drawn.
    - `bar_placement`: Selects the arrangement of bars: aligned, overlap, stacked, etc.
    - `xlabel_rotation`: Amount of counter-clockwise rotation of x axis labels a.k.a. group names, in degrees.
    - `title`: Plot title (autocomputed if missing).
    - `cycle_seed`: Alters the sequence in which colors are assigned to series.
    """
    p = ideplot if ideplot.is_native_plot() else plt

    def get_prop(k):
        return props[k] if k in props else None

    group_fill_ratio = 0.8
    aligned_bar_fill_ratio = 0.9
    overlap_visible_fraction = 1 / 3

    # used only by the mpl charts
    xs = np.arange(len(df.columns), dtype=np.float64)  # the x locations for the groups
    width = group_fill_ratio / len(df.index)  # the width of the bars
    bottoms = np.zeros_like(xs)
    stacks = np.zeros_like(xs)
    group_increment = 0.0

    baseline = get_prop("baseline")
    if baseline:
        if ideplot.is_native_plot(): # is this how this should be done?
            ideplot.set_property("Bars.Baseline", baseline)
        else:
            bottoms += float(baseline)

    extra_args = dict()

    placement = get_prop("bar_placement") or "Aligned"
    if placement:
        if ideplot.is_native_plot(): # is this how this should be done?
            ideplot.set_property("Bar.Placement", placement)
        else:
            if placement == "InFront":
                width = group_fill_ratio
            elif placement == "Stacked":
                width = group_fill_ratio
                bottoms *= 0 # doesn't make sense
            elif placement == "Aligned":
                width = group_fill_ratio / len(df.index)
                group_increment = width
                xs -= width * (len(df.index)-1)/2
                width *= aligned_bar_fill_ratio
            elif placement == "Overlap":
                width = group_fill_ratio / (1 + len(df.index) * overlap_visible_fraction)
                group_increment = width * overlap_visible_fraction
                extra_parts = (1.0 / overlap_visible_fraction - 1)
                xs += width / extra_parts - (len(df.index) + extra_parts) * width * overlap_visible_fraction / 2

    df.sort_index(axis="columns", inplace=True)
    df.sort_index(axis="index", inplace=True)

    if errors_df is not None:
        errors_df.sort_index(axis="columns", inplace=True)
        errors_df.sort_index(axis="index", inplace=True)

        assert(df.columns.equals(errors_df.columns))
        assert(df.index.equals(errors_df.index))

    title_cols, legend_cols = extract_label_columns(meta_df.reset_index(), props)

    for i, ((index, row), meta_row) in enumerate(zip(df.iterrows(), meta_df.reset_index().itertuples(index=False))):
        style = _make_bar_args(props, df)

        if not ideplot.is_native_plot(): # FIXME: noot pretty...
            extra_args['zorder'] = 1 - (i / len(df.index) / 10)
            extra_args['bottom'] = bottoms + stacks

        label = make_legend_label(legend_cols, meta_row, props)
        ys = row.values
        p.bar(xs, ys-bottoms, width, label=label, **extra_args, **style)

        if not ideplot.is_native_plot() and errors_df is not None and not errors_df.iloc[i].isna().all():
            plt.errorbar(xs, ys + stacks, yerr=errors_df.iloc[i], capsize=float(get_prop("cap_size") or 4), **style, linestyle="none", ecolor=mpl.rcParams["axes.edgecolor"])

        xs += group_increment
        if placement == "Stacked":
            stacks += row.values

    rotation = get_prop("xlabel_rotation")
    if rotation:
        rotation = float(rotation)
    else:
        rotation = 0
    p.xticks(list(range(len(df.columns))), list([_to_label(i) for i in df.columns.values]), rotation=rotation)

    # Add some text for labels, title and custom x-axis tick labels, etc.
    groups = df.columns.names

    p.xlabel(_to_label(groups))

    title = get_prop("title")
    if meta_df is not None:
        meta_df = meta_df.reset_index()
        if get_prop("legend_prefer_result_titles") == "true" and "title" in meta_df:
            series = meta_df["title"]
        else:
            series = meta_df["name"]

        title_names = series.unique()
        ylabel = title_names[0]
        if len(title_names) > 1:
            ylabel += ", etc."
        p.ylabel(ylabel)

        if title is None:
            title = make_chart_title(meta_df, title_cols)

    if title is not None:
        set_plot_title(title)



def plot_vectors(df, props, legend_func=make_legend_label):
    """
    Creates a line plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a series.

    Colors and markers are assigned automatically. The `cycle_seed` property
    allows you to select other combinations if the default one is not suitable.

    A function to produce the legend labels can be passed in. By default,
    `make_legend_label()` is used, which offers many ways to influence the
    legend via dataframe columns and chart properties. In the absence of
    more specified settings, the legend is normally computed from columns which best
    differentiate among the vectors.

    Parameters:

    - `df`: the dataframe
    - `props` (dict): the properties
    - `legend_func`: the function to produce custom legend labels

    Columns of the dataframe:

    - `vectime`, `vecvalue` (Numpy `ndarray`'s of matching sizes): the x and y coordinates for the plot
    - `interpolationmode` (str, optional): this column normally comes from a result attribute, and determines how the points will be connected
    - `legend` (optional): legend label for the series; if missing, legend labels are derived from other columns
    - `name`, `title`, `module`, etc. (optional): provide input for the legend

    Notable properties that affect the plot:

    - `title`: plot title (autocomputed if missing)
    - `drawstyle`: Matplotlib draw style; if present, it overrides the draw style derived from `interpolationmode`.
    - `linestyle`, `linecolor`, `linewidth`, `marker`, `markersize`: styling
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    p = ideplot if chart.is_native_chart() else plt

    def get_prop(k):
        return props[k] if k in props else None

    title_cols, legend_cols = extract_label_columns(df, props)

    df.sort_values(by=legend_cols, inplace=True)
    for t in df.itertuples(index=False):
        style = _make_line_args(props, t, df)
        p.plot(t.vectime, t.vecvalue, label=legend_func(legend_cols, t, props), **style)

    title = get_prop("title") or make_chart_title(df, title_cols)
    set_plot_title(title)

    p.ylabel(make_chart_title(df, ["title"]))


def plot_vectors_separate(df, props, legend_func=make_legend_label):
    """
    This is very similar to `plot_vectors`, with identical usage.
    The only difference is in the end result, where each vector will
    be plotted in its own separate set of axes (coordinate system),
    arranged vertically, with a shared X axis during navigation.
    """
    def get_prop(k):
        return props[k] if k in props else None

    title_cols, legend_cols = extract_label_columns(df, props)

    df.sort_values(by=legend_cols, inplace=True)

    ax = None
    for i, t in enumerate(df.itertuples(index=False)):
        style = _make_line_args(props, t, df)
        ax = plt.subplot(df.shape[0], 1, i+1, sharex=ax)

        if i != df.shape[0]-1:
            plt.setp(ax.get_xticklabels(), visible=False)
            ax.xaxis.get_label().set_visible(False)

        plt.plot(t.vectime, t.vecvalue, label=legend_func(legend_cols, t, props), **style)

    plt.subplot(df.shape[0], 1, 1)

    title = get_prop("title") or make_chart_title(df, title_cols)
    set_plot_title(title)


def plot_histograms(df, props, legend_func=make_legend_label):
    """
    Creates a histogram plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a histogram.

    Colors are assigned automatically.  The `cycle_seed` property allows you to
    select other combinations if the default one is not suitable.

    A function to produce the legend labels can be passed in. By default,
    `make_legend_label()` is used, which offers many ways to influence the
    legend via dataframe columns and chart properties. In the absence of
    more specified settings, the legend is normally computed from columns which best
    differentiate among the histograms.

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

    Notable properties that affect the plot:

    - `normalize` (bool): If true, normalize the sum of the bin values to 1. If
      `normalize` is true (and `cumulative` is false), the probability density
       function (PDF) will be displayed.
    - `cumulative` (bool): If true, show each bin as the sum of the previous bin
       values plus itself. If both `normalize` and `cumulative` are true, that
       results in the cumulative density function (CDF) being displayed.
    - `show_overflows` (bool): If true, show the underflow/overflow bins.
    - `title`: Plot title (autocomputed if missing).
    - `drawstyle`: Selects whether to fill the area below the histogram line.
    - `linestyle`, `linecolor`, `linewidth`: Styling.
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    p = ideplot if chart.is_native_chart() else plt

    has_overflow_columns = "min" in df and "max" in df and "underflows" in df and "overflows" in df

    if not has_overflow_columns:
        if "min" in df or "max" in df or "underflows" in df or "overflows" in df:
           raise ValueError("Either all, or none, of the following columns must be present: min, max, underflows, overflows")

    def get_prop(k):
        return props[k] if k in props else None

    title_cols, legend_cols = extract_label_columns(df, props)

    df.sort_values(by=legend_cols, inplace=True)
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
            p.hist(t.binedges[:-1], t.binedges, weights=t.binvalues, label=legend_func(legend_cols, t, props), **overflow, **style)
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

            p.hist(edges[:-1], edges, weights=values, label=legend_func(legend_cols, t, props), **style)

    show_overflows = get_prop("show_overflows")
    if show_overflows and chart.is_native_chart():
        ideplot.set_property("Hist.ShowOverflowCell", str(_parse_optional_bool(show_overflows)).lower())

    title = get_prop("title") or make_chart_title(df, title_cols)
    set_plot_title(title)


def plot_lines(df, props, legend_func=make_legend_label):
    """
    Creates a line plot from the dataframe, with styling and additional input
    coming from the properties. Each row in the dataframe defines a line.

    Colors are assigned automatically.  The `cycle_seed` property allows you to
    select other combinations if the default one is not suitable.

    A function to produce the legend labels can be passed in. By default,
    `make_legend_label()` is used, which offers many ways to influence the
    legend via dataframe columns and chart properties. In the absence of
    more specified settings, the legend is normally computed from columns which best
    differentiate among the lines.

    Parameters:

    - `df`: The dataframe.
    - `props` (dict): The properties.
    - `legend_func` (function): The function to produce custom legend labels.
       See `utils.make_legend_label()` for prototype and semantics.

    Columns of the dataframe:

    - `x`, `y` (array-like, `len(x)==len(y)`): The X and Y coordinates of the points.
    - `error` (array-like, `len(x)==len(y)`, optional):
       The half lengths of the error bars for each point.
    - `legend` (string, optional): Legend label for the series. If missing,
       legend labels are derived from other columns.
    - `name`, `title`, `module`, etc. (optional): Provide input for the legend.

    Notable properties that affect the plot:

    - `title`: Plot title (autocomputed if missing).
    - `linewidth`: Line width.
    - `marker`: Marker style.
    - `linestyle`, `linecolor`, `linewidth`: Styling.
    - `error_style`: If `error` is present, controls how the error is shown.
       Accepted values: "Error bars", "Error band"
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    p = ideplot if chart.is_native_chart() else plt

    def get_prop(k):
        return props[k] if k in props else None

    title_cols, legend_cols = extract_label_columns(df, props)

    df.sort_values(by=legend_cols, inplace=True)
    for t in df.itertuples(index=False):
        style = _make_line_args(props, t, df)

        if len(t.x) < 2 and style["marker"] == ' ':
            style["marker"] = '.'

        p.plot(t.x, t.y, label=legend_func(legend_cols, t, props), **style)

        if hasattr(t, "error") and not ideplot.is_native_plot():
            style["linewidth"] = float(style["linewidth"])
            style["linestyle"] = "none"

            if props["error_style"] == "Error bars":
                plt.errorbar(t.x, t.y, yerr=t.error, capsize=float(props["cap_size"]), **style)
            elif props["error_style"] == "Error band":
                plt.fill_between(t.x, t.y-t.error, t.y+t.error, alpha=float(props["band_alpha"]))

    title = get_prop("title") or make_chart_title(df, title_cols)
    set_plot_title(title)


def plot_boxwhiskers(df, props, legend_func=make_legend_label):
    """
    Creates a box and whiskers plot from the dataframe, with styling and additional
    input coming from the properties. Each row in the dataframe defines one set
    of a box and two whiskers.

    Colors are assigned automatically.  The `cycle_seed` property allows you to
    select other combinations if the default one is not suitable.

    A function to produce the legend labels can be passed in. By default,
    `make_legend_label()` is used, which offers many ways to influence the
    legend via dataframe columns and chart properties. In the absence of
    more specified settings, the legend is normally computed from columns which best
    differentiate among the boxes.

    Parameters:

    - `df`: The dataframe.
    - `props` (dict): The properties.
    - `legend_func` (function): The function to produce custom legend labels.
       See `utils.make_legend_label()` for prototype and semantics.

    Columns of the dataframe:

    - `min`, `max`, `mean`, `stddev`, `count` (float): The minimum/maximum
       values, mean, standard deviation, and sample count of the data.
    - `legend` (string, optional): Legend label for the series. If missing,
       legend labels are derived from other columns.
    - `name`, `title`, `module`, etc. (optional): Provide input for the legend.

    Notable properties that affect the plot:

    - `title`: Plot title (autocomputed if missing).
    - `cycle_seed`: Alters the sequence in which colors and markers are assigned to series.
    """
    title_cols, legend_cols = extract_label_columns(df, props)
    df.sort_values(by=legend_cols, axis='index', inplace=True)

    # This is how much of the standard deviation will give the 25th and 75th
    # percentiles, assuming normal distribution.
    # >>> math.sqrt(2) * scipy.special.erfinv(0.5)
    coeff = 0.6744897501960817

    boxes = [(r.min, r.mean - r.stddev * coeff, r.mean, r.mean + r.stddev * coeff, r.max)
            for r in df.itertuples(index=False) if r.count > 0]
    labels = [legend_func(legend_cols, r, props) for r in df.itertuples(index=False) if r.count > 0]
    customized_box_plot(boxes, labels)

    title = make_chart_title(df, title_cols)
    if "title" in props and props["title"]:
        title = props["title"]
    set_plot_title(title)


# source: https://stackoverflow.com/a/39789718/635587
def customized_box_plot(percentiles, labels=None, axes=None, redraw=True, *args, **kwargs):
    """
    Generates a customized box-and-whiskers plot based on explicitly specified
    percentile values. This method is necessary because `pyplot.boxplot()` insists
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
            boxprops=dict(facecolor=color), whiskerprops=dict(zorder=5, linewidth=1, solid_capstyle="butt"), capprops=dict(zorder=6, linewidth=2, color=color, solid_capstyle="butt"),
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


def preconfigure_plot(props):
    """
    Configures the plot according to the given properties, which normally
    get their values from setting in the "Configure Chart" dialog.
    Calling this function before plotting was performed should be a standard
    part of chart scripts.

    A partial list of properties taken into account for native plots:
    - property keys understood by the plot widget, see `ideplot.get_supported_property_keys()`

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
        supported_keys = ideplot.get_supported_property_keys()
        ideplot.set_properties({ k: v for k, v in props.items() if k in supported_keys})
    else:
        if get_prop("plt.style"):
            plt.style.use(get_prop("plt.style"))
        mpl.rcParams.update(_filter_by_key_prefix(props,"matplotlibrc."))
        mpl.rcParams.update(parse_rcparams(get_prop("matplotlibrc") or ""))
        _make_scroll_navigable(plt.gcf())

    _initialize_cycles(props)


def _legend_loc_outside_args(loc):
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
    - properties listed in the `plot.properties` property

    Parameters:
    - `props` (dict): the properties
    """
    p = ideplot if ideplot.is_native_plot() else plt

    def get_prop(k):
        return props[k] if k in props else None

    def setup():
        if get_prop("xaxis_title"):
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

        ideplot.grid(_parse_optional_bool(get_prop("grid_show")),
            "major" if (get_prop("grid_density") or "").lower() == "major" else "both") # grid_density is "Major" or "All"

    if ideplot.is_native_plot():
        setup()

        ideplot.legend(show=_parse_optional_bool(get_prop("legend_show")),
           frameon=_parse_optional_bool(get_prop("legend_border")),
           loc=get_prop("legend_placement"))

        ideplot.set_properties(parse_rcparams(get_prop("plot.properties") or ""))
    else:
        for ax in p.gcf().axes:
            plt.sca(ax)
            setup()

            if _parse_optional_bool(get_prop("legend_show")):

                loc = get_prop("legend_placement")
                args = { "loc": loc}
                if loc and loc.startswith("outside"):
                    args.update(_legend_loc_outside_args(loc))
                args["frameon"] =_parse_optional_bool(get_prop("legend_border"))
                plt.legend(**args)
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
    - `image_export_filename`: The output file name. If it has no extension,
       one will be added based on the format. If missing or empty, a sanitized
       version of the chart name is used.
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
        filepath = get_image_export_filepath(props)
        format = get_prop("image_export_format") or "svg"
        width = float(get_prop("image_export_width") or 6)
        height = float(get_prop("image_export_height") or 4)
        dpi = float(get_prop("image_export_dpi") or "96")

        print("exporting image to: '" + filepath + "' as " + format)

        os.makedirs(os.path.dirname(filepath), exist_ok=True)

        plt.gcf().set_size_inches(width, height)
        plt.gcf().set_dpi(dpi)

        # in case of Matplotlib-emulated native widgets, make it look more similar to the IDE
        if chart.is_native_chart():
            ax = plt.gca()

            # add digit grouping to the axis tick labels
            ax.xaxis.set_minor_formatter(_DigitGroupingFormatter())
            ax.xaxis.set_major_formatter(_DigitGroupingFormatter())
            ax.yaxis.set_minor_formatter(_DigitGroupingFormatter())
            ax.yaxis.set_major_formatter(_DigitGroupingFormatter())

            # only use scientific notation for negative exponents
            ax.ticklabel_format(scilimits=(0, 1000), useOffset=False, useLocale=False)

            # start with a tight value bounding box
            ax.margins(0)
            ax.autoscale(True)

            # apply the same auto-range logic as in the native widgets of the IDE
            min_x, max_x = ax.get_xlim()
            width = max_x - min_x # not computing twice, would cause asymmetry
            min_x = 0 if min_x >= 0 else min_x - width/80
            max_x = 0 if max_x <= 0 else max_x + width/80

            if width == 0:
                if max_x > 0:
                    max_x *= 1.25
                if min_x < 0:
                    min_x *= 1.25
                if min_x == max_x == 0:
                    min_x = -1
                    max_x = 1

            if get_prop("xaxis_min"):
                min_x = float(get_prop("xaxis_min"))
            if get_prop("xaxis_max"):
                max_x = float(get_prop("xaxis_max"))

            ax.set_xlim(left=min_x, right=max_x)


            min_y, max_y = ax.get_ylim()
            height = max_y - min_y # not computing twice, would cause asymmetry
            min_y = 0 if min_y >= 0 else min_y - height/3
            max_y = 0 if max_y <= 0 else max_y + height/3

            if height == 0:
                if max_y > 0:
                    max_y *= 1.25
                if min_y < 0:
                    min_y *= 1.25
                if min_y == max_y:
                    min_y = -1
                    max_y = 1

            if get_prop("yaxis_min"):
                min_y = float(get_prop("yaxis_min"))
            if get_prop("yaxis_max"):
                max_y = float(get_prop("yaxis_max"))

            ax.set_ylim(bottom=min_y, top=max_y)

            # reducing margins all around
            plt.tight_layout()

        def _make_image_metadata():
            # Include versions of various software components into the metadata,
            # to facilitate resolving support reqests later. We use "Creator" as key,
            # as it seems to be suitable with all image formats. Some formats ignore it
            # and PNG would prefer to call it "Software", but it doesn't cause an exception
            # to be thrown from savefig(). JPG and TIF display a DeprecationWarning though:
            # "savefig() got unexpected argument 'metadata' which is no longer supported
            # as of 3.3 and will become an error two minor releases later"
            import platform
            creatorInfo = f"omnetpp.scave {__version__}; Matplotlib {mpl.__version__} w/ backend {mpl.get_backend()}; Pandas {pd.__version__}; Python {platform.python_version()}; {platform.platform()}"
            return { "Creator" : creatorInfo }

        # Explicitly select a non-interactive backend appropriate for the export format,
        # and keep auto-selection for any other formats not listed here.
        backend = _get_mpl_backend_for_image_format(format)

        # save image
        retry = True
        try:
            plt.savefig(filepath, format=format, dpi=dpi, metadata=_make_image_metadata(), backend=backend)
            retry = False
        except:
            pass
        if retry:
            # try again without metadata, in case that caused the error
            # oh yes, and we cannot put this into the "except" block, because we'd get
            # an ugly error message ("During handling of the above exception, another
            # exception occurred") if this fails too, e.g. due to innvalid "format" parameter
            plt.savefig(filepath, format=format, dpi=dpi, backend=backend)

def get_image_export_filepath(props):
    """
    Returns the file path for the image to export based on the
    `image_export_format`, `image_export_folder` and
    `image_export_filename` properties given in `props`.
    If a relative filename is returned, it is relative to the
    working directory when the image export takes place.
    """
    def get_prop(k):
        return props[k] if k in props else None
    format = get_prop("image_export_format") or "svg"
    folder = get_prop("image_export_folder") or "."
    filename = get_prop("image_export_filename") or _sanitize_filename(chart.get_name())
    if not re.match(string=filename, pattern=r".+\.\w{3,4}$"):
        filename = filename + "." + format
    return os.path.join(folder, filename)

def _format_to_extension(format):
    format = format.lower()

    format_to_extension = {
        "markdown": "md",
        "latex": "tex",
        "excel": "xlsx",
        "xls": "xlsx",
        "bigquery": "gbq",
        "stata": "dta",
        "hdf": "h5",
        "feather": "ftr",
        "pickle": "pkl"
    }

    if format in format_to_extension.keys():
        format = format_to_extension[format]

    return format


def _get_mpl_backend_for_image_format(format):
    return {
        "png":  "AGG",
        "jpg":  "AGG",
        "jpeg": "AGG",
        "tif":  "AGG",
        "tiff": "AGG",
        "raw":  "AGG",

        "svg":  "SVG",
        "svgz": "SVG",

        "ps" :  "PS",
        "eps":  "PS",

        "pdf":  "PDF",

        "pgf":  "PGF",
    }.get(format.lower(), None)


def _export_df_as(df, format, filepath, **kwargs):
    extension = _format_to_extension(format)

    if extension == "pkl":
        df.to_pickle(filepath, **kwargs)
    elif extension == "csv":
        df.to_csv(filepath, **kwargs)
    elif extension == "h5":
        if "key" not in kwargs.keys():
            kwargs["key"] = "results"
        df.to_hdf(filepath, **kwargs)
    #elif extension == "sql":
    #    df.to_sql(filepath, **kwargs) # needs a connection
    elif extension == "xlsx":
        df.to_excel(filepath, **kwargs)
    elif extension == "json":
        df.to_json(filepath, **kwargs)
    elif extension == "html":
        df.to_html(filepath, **kwargs)
    #elif extension == "ftr":
    #    df.reset_index().to_feather(filepath, **kwargs) # column names must be strings
    elif extension == "tex":
        df.to_latex(filepath, **kwargs)
    elif extension == "dta":
        df.to_stata(filepath, **kwargs)
    #elif extension == "gbq":
    #    df.to_gbq(filepath, **kwargs) # needs authentication
    elif extension == "md":
        df.to_markdown(filepath, **kwargs),
    else:
        raise ValueError("Unknown export data format: " + format)

def export_data_if_needed(df, props, **kwargs):
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

    try:
        chart.set_observed_column_names(list(df.columns.values))
    except Exception as e:
        print("Warning: error in chart.set_observed_column_names():", e, file=sys.stderr)

    def get_prop(k):
        return props[k] if k in props else None

    def printer(arr):
        return np.array_str(arr)

    if _parse_optional_bool(get_prop("export_data")):
        format = get_prop("data_export_format") or "csv"
        filepath = get_data_export_filepath(props)

        print("exporting data to: '" + filepath + "' as " + format)

        os.makedirs(os.path.dirname(filepath), exist_ok=True)

        old_opts = np.get_printoptions()
        np.set_printoptions(threshold=np.inf, linewidth=np.inf)
        np.set_string_function(printer, False)
        pd.set_option('display.max_columns', None)
        pd.set_option('display.max_colwidth', None)
        _export_df_as(df, format, filepath, **kwargs)
        np.set_string_function(None, False)
        np.set_printoptions(**old_opts)

def get_data_export_filepath(props):
    """
    Returns the file path for the data to export based on the
    `data_export_format`, `data_export_folder` and
    `data_export_filename` properties given in `props`.
    If a relative filename is returned, it is relative to the
    working directory when the data export takes place.
    """
    def get_prop(k):
        return props[k] if k in props else None
    format = get_prop("data_export_format") or "csv"
    extension = _format_to_extension(format)
    folder = get_prop("data_export_folder") or "."
    filename = get_prop("data_export_filename") or _sanitize_filename(chart.get_name())
    if not re.match(string=filename, pattern=r".+\.\w{3,4}$"):
        filename = filename + "." + extension
    return os.path.join(folder, filename)


def _sanitize_filename(filename):
    valid_chars = "-_=.() %s%s" % (string.ascii_letters, string.digits) # TODO may be too strict, e.g. '#' or accented letters should be allowed
    return ''.join(c for c in filename if c in valid_chars)


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
    _marker_cycle = it.cycle(ml)

    prop_cycler = plt.rcParams['axes.prop_cycle']

    if chart.is_native_chart():
        num_colors = 10
    else:
        num_colors = len(prop_cycler.by_key()['color']) if "color" in prop_cycler.keys else 1

    cl = ["C" + str(i) for i in range(num_colors)]
    if seed != 0:
        random.shuffle(cl, r.random)
    _color_cycle = it.cycle(cl)


def histogram_bin_edges(values, bins=None, range=None, weights=None):
    """
    An improved version of numpy.histogram_bin_edges.
    This will only return integer edges for input arrays consisting entirely of integers
    (unless the `bins` are explicitly given otherwise).
    In addition, the rightmost edge will always be strictly greater than the maximum of `values`
    (unless explicitly given otherwise in `range`).
    """
    if bins is not None and type(bins) != int:
        if range is None:
            min_value = values.min()
            max_value = values.max()
            value_range = max_value - min_value
            range = (min_value, max_value + value_range * 0.01)
        return np.histogram_bin_edges(values, bins, range, weights)

    all_integers = not np.mod(values, 1).any()
    min_value, max_value = range if range is not None else (values.min(), values.max())
    value_range = max_value - min_value

    if value_range == 0:
        edges = [min_value, min_value + 1]
    else:
        if all_integers:
            bin_size = max(1, round(value_range / (bins or 10)))
            # the +1 is to make sure that max_value will be in the returned edge list,
            # and the +bin_size is to add another whole bin because
            # numpy.histogram defines the last bin as closed from the right
            edges = np.arange(min_value, max_value + 1 + bin_size, bin_size)
        else:
            edges = np.histogram_bin_edges(values, bins or 'auto', range or (min_value, max_value + value_range * 0.01), weights)

    return edges


def confidence_interval(alpha, data):
    """
    Returns the half-length of the confidence interval of the mean of `data`, assuming
    normal distribution, for the given confidence level `alpha`.

    Parameters:

    - `alpha` (float): Confidence level, must be in the [0..1] range.
    - `data` (array-like): An array containing the values.
    """
    return math.nan if len(data) <= 1 else 0 if len(set(data)) <= 1 else st.norm.interval(alpha, loc=0, scale=st.sem(data))[1]


def pivot_for_barchart(df, groups, series, confidence_level=None):
    """
    Turns a DataFrame containing scalar results (in the format returned
    by `results.get_scalars()`) into a 3-tuple of a value, an error, and
    a metadata DataFrame, which can then be passed to `utils.plot_bars()`.
    The error dataframe is None if no confidence level is given.

    Parameters:

    - `df` (pandas.DataFrame): The dataframe to pivot.
    - `groups` (list): A list of column names, the values in which will
       be used as names for the bar groups.
    - `series` (list): A list of column names, the values in which will
       be used as names for the bar series.
    - `confidence_level` (float, optional):
       The confidence level to use when computing the sizes of the error bars.

    Returns:

    -  A triplet of DataFrames containing the pivoted data: (values, errors, metadata)
    """
    for c in groups + series:
        df[c] = pd.to_numeric(df[c], errors="ignore")

    df.sort_values(by=groups+series, axis='index', inplace=True)

    def aggfunc(values):
        if values.empty:
            return None
        elif np.issubdtype(values.dtype, np.number):
            return values.mean()
        else:
            uniq = values.unique()
            if len(uniq) == 1:
                return uniq[0]
            else:
                return str(uniq[0]) + ", etc."

    metadf = pd.pivot_table(df, index=series, aggfunc=aggfunc, dropna=False)
    del metadf["value"]

    if confidence_level is None:
        df = pd.pivot_table(df, index=series, columns=groups, values='value', dropna=False)
        return (df, None, metadf)
    else:
        def conf_intv(values):
            return confidence_interval(confidence_level, values)

        pivoted = pd.pivot_table(df, index=series, columns=groups, values="value", aggfunc=[np.mean, conf_intv], dropna=False)
        valuedf = pivoted["mean"]
        errorsdf = pivoted["conf_intv"]
        if errorsdf.isna().values.all():
            errorsdf = None
        return (valuedf, errorsdf, metadf)


def pivot_for_scatterchart(df, xaxis_itervar, group_by, confidence_level=None):
    """
    Turns a DataFrame containing scalar results (in the format returned
    by `results.get_scalars()`) into a DataFrame which can then be
    passed to `utils.plot_lines()`.

    Parameters:

    - `df` (pandas.DataFrame): The dataframe to pivot.
    - `xaxis_itervar` (string): The name of the iteration variable whose
       values are to be used as X coordinates.
    - `group_by` (list): A list of column names, the values in which will
       be used to group the scalars into lines.
    - `confidence_level` (float, optional):
       The confidence level to use when computing the sizes of the error bars.

    Returns:

    -  A DataFrame containing the pivoted data, with these columns:
       `name`, `x`, `y`, and optionally `error` - if `confidence_level` is given.
    """

    for gb in group_by:
        df[gb] = pd.to_numeric(df[gb], errors="ignore")

    df[xaxis_itervar] = pd.to_numeric(df[xaxis_itervar], errors="ignore")

    newdf = pd.DataFrame()
    if confidence_level is None:
        df = pd.pivot_table(df, values="value", columns=group_by, index=xaxis_itervar)
        errors_df = None
    else:
        def conf_intv(values):
            return confidence_interval(confidence_level, values)
        pivoted = pd.pivot_table(df, values="value", columns=group_by, index=xaxis_itervar if xaxis_itervar else "name", aggfunc=[np.mean, conf_intv], dropna=False)

        df = pivoted["mean"]
        errors_df = pivoted["conf_intv"]

        if errors_df.isna().values.all():
            errors_df = None

    try:
        xs = pd.to_numeric(df.index.values)
        ideplot.xlabel(xaxis_itervar)
    except:
        xs = np.zeros_like(df.index.values)

    for c in df:
        t = c if type(c) == tuple else (c,)
        name = ", ".join([str(a) + "=" + str(b) for a, b in zip(df.columns.names, t) if a is not None])

        to_append = pd.DataFrame.from_dict({"name": [name], "x": [np.array(xs)], "y": [np.array(df[c].values)]})
        if errors_df is not None:
            to_append["error"] = [np.array(errors_df[c].values)]
        newdf = newdf.append(to_append)

    return newdf


def get_confidence_level(props):
    """
    Returns the confidence level from the `confidence_level` property,
    converted to a `float`. Also accepts "none" (returns `None` in this case),
    and percentage values (e.g. "95%").
    """
    if "confidence_level" not in props:
        return None
    s = props["confidence_level"]
    if s == "none":
        return None
    return float(s[:-1])/100 if s.endswith("%") else float(s)


def perform_vector_ops(df, operations: str):
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
            raise chart.ChartScriptError(str(e) + context) from e # re-throw with context
    return df


def _perform_vector_op(df, line: str):
    """
    Performs one vector operation on the dataframe. Helper for `perform_vector_ops()`.
    """
    # parse line
    type, module, name, args, kwargs, arglist_str = _parse_vectorop_line(line)
    if name is None: # empty line
        return df

    # look up operation
    function = vectorops.lookup_operation(module, name)
    if function is None:
        errmsg = "Vector filter function '" + name + "' not found"
        if module is not None:
            errmsg += " in module '" + module + "'"
        raise ValueError(errmsg)

    # perform operation
    if type == "apply":
        df = _apply_vector_op(df, name + arglist_str, function, *args, **kwargs)
    elif type == "compute":
        df = _compute_vector_op(df, name + arglist_str, function, *args, **kwargs)
    return df


def _parse_vectorop_line(line: str):
    """
    Parses the given vector operation line, and returns its contents as a tuple.
    Helper for `perform_vector_ops()`.
    """
    line = line.strip()
    m = re.match(r"""(?x) # allow whitespace and comments in regex
        ((\w+)\s*:)?      # optional "apply:" or "compute:"
        \s*               # optional whitespace
        (([\w.]+)\.)?     # optional qualifier (module prefix) for function name
        (\w+)?            # function name (optional so we can allow blank and comment-only lines)
        (.*)              # the rest: arglist in parens, comment (both optional)
        """, line)        # note: this regex always matches, and always matches the whole string due to last group
    _, type, _, module, name, rest = m.groups()
    if not name:
        if line and not line.startswith('#'):
            raise SyntaxError("Syntax error")
        return (None, None, None, None, None, None)
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
    arglist_str = re.sub(r"\)\s*#.*", ")", rest) if args or kwargs else ""
    return type, module, name, args, kwargs, arglist_str


def _apply_vector_op(df, op_str, operation, *args, **kwargs):
    """
    Process a vector operation with the `apply` prefix. Helper for `perform_vector_ops()`.
    """
    if operation == vectorops.aggregate:
        return vectorops.aggregate(df, *args, **kwargs)
    elif operation == vectorops.merge:
        return vectorops.merge(df)
    else:
        condition = kwargs.pop('condition', None)

        def process(row):
            row["vectime"].flags.writeable = False
            row["vecvalue"].flags.writeable = False
            row = operation(row, *args, **kwargs)
            row["name"] = row["name"] + ":" + op_str
            if not row["vectime"].flags.owndata:
                row["vectime"] = row["vectime"].copy()
            row["vectime"].flags.writeable = True
            if not row["vecvalue"].flags.owndata:
                row["vecvalue"] = row["vecvalue"].copy()
            row["vecvalue"].flags.writeable = True
            return row

        # Using iteration instead of df.transform because that catches the ChartScriptErrors
        # raised from the vector operation functions internally, which we don't want.
        clone = pd.DataFrame()
        for _, row in df.iterrows():
            if condition is None or condition(row):
                clone = clone.append(process(row), ignore_index=True)
        return clone


def _compute_vector_op(df, op_str, operation, *args, **kwargs):
    """
    Process a vector operation with the `compute` prefix. Helper for `perform_vector_ops()`.
    """
    return df.append(_apply_vector_op(df, op_str, operation, *args, **kwargs), sort=False)


def set_plot_title(title, suggested_chart_name=None):
    """
    Sets the plot title. It also sets the suggested chart name (the name that
    the IDE offers when adding a temporary chart to the Analysis file.)
    """
    ideplot.title(title)
    chart.set_suggested_chart_name(suggested_chart_name if suggested_chart_name is not None else title)


def fill_missing_titles(df):
    """
    Utility function to fill missing values in the `title` and `moduledisplaypath`
    columns from the `name` and `module` columns. (Note that `title` and `moduledisplaypath`
    normally come from result attributes of the same name.)
    """
    if "title" in df and "name" in df:
        df["title"].fillna(df["name"], inplace=True)
    if "moduledisplaypath" in df and "module" in df:
        df["moduledisplaypath"].fillna(df["module"], inplace=True)


def extract_label_columns(df, props):
    """
    Utility function to make a reasonable guess as to which column of
    the given DataFrame is most suitable to act as a chart title and
    which ones can be used as legend labels.

    Ideally a "title column" should be one in which all lines have the same
    value, and can be reasonably used as a title. This is often the `title`
    or `name` column.

    Label columns should be a minimal set of columns whose corresponding
    value tuples uniquely identify every line in the DataFrame. These will
    primarily be iteration variables and run attributes.

    Returns:

    A pair of a string and a list; the first value is the name of the
    "title" column, and the second one is a list of pairs, each
    containing the index and the name of a "label" column.

    Example: `('title', [(8, 'numHosts'), (7, 'iaMean')])`
    """

    def get_prop(k):
        return props[k] if k in props else None

    fill_missing_titles(df)

    prefer_titles = get_prop('legend_prefer_result_titles') == 'true'
    prefer_displaypaths = get_prop('legend_prefer_module_display_paths') == 'true'

    name_column = "title" if ((prefer_titles or "name" not in df) and "title" in df) else "name"
    module_column = "moduledisplaypath" if ((prefer_displaypaths or "module" not in df) and "moduledisplaypath" in df) else "module"

    title_cols = []
    legend_cols = []

    if name_column in df:
        if df[name_column].nunique() == 1:
            title_cols.append(name_column)
        else:
            legend_cols.append(name_column)

    if module_column in df:
        if df[module_column].nunique() == 1:
            title_cols.append(module_column)
        else:
            legend_cols.append(module_column)

    if len(df) == 1:
        title_cols = [c for c in [name_column, module_column] if c in df]
        legend_cols = title_cols
        return title_cols, legend_cols

    if not title_cols:
        title_col_candidates = ["experiment", "measurement", "replication"]

        # for title, choose the first column that has has identical values in all rows
        for col in title_col_candidates:
            if col in df and col not in title_cols and df[col].nunique() == 1 and df[col].values[0]:
                title_cols.append(col)
                break
        if not title_cols and name_column in df:
            title_cols = [name_column]

    blacklist = set(["name", "title", "module", "moduledisplaypath", # these will be used anyway
                 "runID", "value", "attrvalue", "vectime", "vecvalue",
                 "binedges", "binvalues", "underflows", "overflows",
                 "count", "sumweights", "mean", "stddev", "min", "max",
                 "processid", "datetime", "datetimef", "runnumber", "seedset",
                 "iterationvars", "iterationvarsf", "iterationvarsd", "repetition",
                 "source", "recordingmode", "interpolationmode", "enum", "unit"])

    # if unsuccessful, try to pick from all columns, except a few that we don't like
    legend_col_candidates = [col for col in list(df.columns.values) if col not in blacklist and col not in title_cols]

    # last resort columns
    if "datetime" in df:
        legend_col_candidates.append("datetime")
    if "runID" in df:
        legend_col_candidates.append("runID")

    # pick columns that improve the partitioning of rows, until each row can be identified with them
    last_len = None
    for col in legend_col_candidates:
        try:
            new_len = len(df.groupby(legend_cols + [col]))
            if new_len == len(df) or not last_len or new_len > last_len:
                legend_cols.append(col)
                last_len = new_len
            if new_len == len(df):
                break
        except Exception:
            pass

    # filter out columns that only have a single value in them
    # (this can happen in the loop above, with the first considered column)
    legend_cols = list(filter(lambda icol: df[icol].nunique() > 1, legend_cols))

    # at this point, ideally this should hold: len(df.groupby(legend_cols)) == len(df)
    return title_cols, legend_cols

def make_chart_title(df, title_cols):
    """
    Produces a reasonably good chart title text from a result DataFrame,
    given a selected list of "title" columns.
    """
    if df is None or df.empty:
        return "None"

    def fmt(col):
        if col in ["name", "title"]:
            prefix = ""
        elif col in ["module", "moduledisplaypath"]:
            prefix = " in "
        else:
            prefix = ", " + col + "="

        val = str(df[col].values[0])
        if df[col].nunique() > 1:
            val += ", etc."

        return prefix + val

    title = "".join([fmt(col) for col in title_cols if col in df])
    title = _removeprefix(_removeprefix(title, ", "), " in ")

    return title


def select_best_partitioning_column_pair(df, props=None):  #TODO remove default for props in final release
    """
    Choose two columns from the dataframe which best partitions the rows
    of the dataframe, and returns their names as a pair. Returns (`None`, `None`)
    if no such pair was found. This method is useful for creating e.g. a bar plot.
    """
    if props is None:
        print("select_best_partitioning_column_pair(): Missing props argument! Update chart script, or code will break on next release!", file=sys.stderr)
        props = {}
    title_cols, label_cols = extract_label_columns(df, props)
    if len(label_cols) == 0:
        return None, None
    if len(label_cols) == 1:
        if len(title_cols) == 0:
            return None, None
        elif label_cols[0] == title_cols[0]:
            return None, None
        else:
            return title_cols[0], label_cols[0]
    if len(label_cols) >= 2:
        return label_cols[1], label_cols[0]


def select_groups_series(df, props):
    """
    Extracts the column names to be used for groups and series from the `df` DataFrame,
    for pivoting. The columns whose names are to be used as group names are given in
    the "groups" property in `props`, as a comma-separated list.
    The names for the series are selected similarly, based on the "series" property.
    There should be no overlap between these two lists.

    If both "groups" and "series" are given (non-empty), they are simply returned
    as lists after some sanity checks.
    If both of them are empty, a reasonable guess is made for which columns should
    be used, and (["module"], ["name"]) is used as a fallback.

    The data in `df` should be in the format as returned by `result.get_scalars()`,
    and the result can be used directly by `utils.pivot_for_barchart()`.

    Returns:
        - (group_names, series_names): A pair of lists of strings containing the
          selected names for the groups and the series, respectively.
    """
    groups = split(props["groups"])
    series = split(props["series"])

    if not groups and not series:
        print("The Groups and Series options were not set in the dialog, inferring them from the data.")
        g, s = ("module", "name") if len(df) == 1 else select_best_partitioning_column_pair(df, props)
        groups, series = [g], [s]

    if not groups or not groups[0] or not series or not series[0]:
        raise chart.ChartScriptError("Please set both the Groups and Series properties in the dialog - or neither, for automatic setup.")

    common = list(set(groups) & set(series))
    if common:
        raise chart.ChartScriptError("Overlap between Group and Series columns: " + ", ".join(common))

    assert_columns_exist(df, groups + series, "No such iteration variable or run attribute")

    return groups, series


def select_xaxis_and_groupby(df, props):
    """
    Extracts an iteration variable name and the column names to be used for grouping from
    the `df` DataFrame, for pivoting. The columns whose names are to be used as group
    names are given in the "group_by" property in `props`, as a comma-separated list.
    The name of the iteration variable is selected similarly, from the "xaxis_itervar" property.
    The "group_by" list should not contain the given "xaxis_itervar" name.

    If both "xaxis_itervar" and "group_by" are given (non-empty), they are simply returned
    after some sanity checks, with "group_by" split into a list.
    If both of them are empty, a reasonable guess is made for which columns should be used.

    The data in `df` should be in the format as returned by `result.get_scalars()`,
    and the result can be used directly by `utils.pivot_for_scatterchart()`.

    Returns:
        - (xaxis_itervar, group_by): An iteration variable name, and a list of strings
          containing the selected column names to be used as groups.
    """
    xaxis_itervar = props["xaxis_itervar"]
    group_by = split(props["group_by"])

    if not xaxis_itervar and not group_by:
        print("The 'X Axis' and 'Group By' options were not set in the dialog, inferring them from the data..")
        xaxis_itervar, group_by = select_best_partitioning_column_pair(df, props)
        group_by = [group_by] if group_by else []
        print("X Axis: " + xaxis_itervar + ", Group By: " + ",".join(group_by))

    if xaxis_itervar:
        assert_columns_exist(df, [xaxis_itervar], "The iteration variable for the X axis could not be found")
    else:
        raise chart.ChartScriptError("Please select the iteration variable for the X axis!")

    if xaxis_itervar in group_by:
        raise chart.ChartScriptError("X axis column also in grouper columns: " + xaxis_itervar)

    if group_by:
        assert_columns_exist(df, group_by, "An iteration variable for grouping could not be found")


    # report averaging
    uninteresting = ["runID", "value", "datetime", "datetimef", "processid",
                    "iterationvars", "iterationvarsf", "iterationvarsd",
                    "measurement", "replication", "runnumber", "seedset",
                    "title", "source", "interpolationmode"]

    for c in df:
        ul = len(df[c].unique())
        if ul > 1 and c != xaxis_itervar and c not in group_by and c not in uninteresting:
            print("Points are averaged from an overall", ul, "unique", c, "values.")

    return xaxis_itervar, group_by


def assert_columns_exist(df, cols, message="Expected column missing from DataFrame"):
    """
    Ensures that the dataframe contains the given columns. If any of them are missing,
    the function raises an error with the given message.

    Parameters:

    - `cols` (list of strings): Column names to check.
    """
    for c in cols:
        if c not in df:
            raise chart.ChartScriptError(message + ": " + c)


def to_numeric(df, columns=None, errors="ignore", downcast=None):
    """
    Convenience function. Runs `pandas.to_numeric` on the given
    (or all) columns of `df`. If any of the given columns doesn't
    exist, throws an error.

    Parameters:

    - `df` (DataFrame): The DataFrame to operate on
    - `columns` (list of strings): The list of column names to convert.
      If not given, all columns will be converted.
    - `errors`, `downcast` (string): Will be passed to `pandas.to_numeric()`
    """
    df = df.copy()

    if columns:
        assert_columns_exist(df, columns)
    else:
        columns = df.columns.values

    for c in columns:
        df[c] = pd.to_numeric(df[c], errors=errors, downcast=downcast)

    return df


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
            raise RuntimeError("Illegal rc line #" + str(line_no) + ": " + line)
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


def _split_by_types(df, types):
    result = list()
    for t in types:
        mask = df['type'] == t
        result.append(df[mask])
        df = df[~mask]
    result.append(df)
    return result

def _append_metadata_columns(df, meta, suffix):
    meta = pd.pivot_table(meta, index="runID", columns="attrname", values="attrvalue", aggfunc="first")

    if not meta.empty:
        df = df.join(meta, on="runID", rsuffix=suffix)

    return df

def _select_param_assignments(config_entries_df):
    names = config_entries_df["attrname"]

    is_typename = names.str.endswith(".typename")
    is_param = ~is_typename & names.str.match(r"^.*\.[^.-]+$")

    result = config_entries_df.loc[is_param]

    return result

def _pivot_results(df, include_attrs, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    itervars, runattrs, configs, attrs, df = _split_by_types(df, ["itervar", "runattr", "config", "attr"])
    params = _select_param_assignments(configs)

    if include_attrs and attrs is not None and not attrs.empty:
        attrs = pd.pivot_table(attrs, columns="attrname", aggfunc='first', index=["runID", "module", "name"], values="attrvalue")
        # this column is no longer needed, and it collided with the commonly used "type" result attribute in `merge`
        df.drop(["type"], axis=1, inplace=True, errors="ignore")
        df = df.merge(attrs, left_on=["runID", "module", "name"], right_index=True, how='left', suffixes=(None, "_attr"))

    if include_itervars:
        df = _append_metadata_columns(df, itervars, "_itervar")
    if include_runattrs:
        df = _append_metadata_columns(df, runattrs, "_runattr")
    if include_config_entries:
        df = _append_metadata_columns(df, configs, "_config")
    if include_param_assignments and not include_config_entries:
        df = _append_metadata_columns(df, params, "_param")

    df.drop(['type', 'attrname', 'attrvalue'], axis=1, inplace=True, errors="ignore")

    df.reset_index(inplace=True, drop=True)

    return df


def _pivot_metadata(df, metadf, include_runattrs, include_itervars, include_param_assignments, include_config_entries):
    itervars, runattrs, configs, remainder = _split_by_types(metadf, ["itervar", "runattr", "config"])
    params = _select_param_assignments(configs)

    assert(remainder.empty)

    if include_itervars:
        df = _append_metadata_columns(df, itervars, "_itervar")
    if include_runattrs:
        df = _append_metadata_columns(df, runattrs, "_runattr")
    if include_config_entries:
        df = _append_metadata_columns(df, configs, "_config")
    if include_param_assignments and not include_config_entries:
        df = _append_metadata_columns(df, params, "_param")

    df = df.drop(['type', 'attrname', 'attrvalue'], axis=1, errors="ignore")

    df.reset_index(inplace=True, drop=True)

    return df
