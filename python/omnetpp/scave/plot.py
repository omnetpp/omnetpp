"""
This module is the interface for displaying data using the built-in plot widgets (i.e. without matplotlib).
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE", "no") == "yes":
    from omnetpp.scave.impl_ide import plot as impl
else:
    from .impl_charttool import plot as impl

import math


def plot_bars(df):
    return impl.plot_bars(**locals())


def plot_lines(df):
    return impl.plot_lines(**locals())


def plot_histogram_data(df):
    return impl.plot_histogram_data(**locals())


# TODO: rename row_label to group_label?
def plot_scalars(df_or_values, labels=None, row_label=None):
    """
    Takes a set of scalar type results and plots them as a "native" bar chart - using the
    IDE's built-in drawing widget.

    `df_or_values` can be in one of three different formats:

    - A `pd.DataFrame` with (at least) `value`, `module` and `name` columns,
        for example as returned by `results.get_scalars()`
    - A `pd.DataFrame` containing only the numeric values. This is what you
        might get for example from `pd.pivot_table()`.
        Each line in the dataframe will be in it's own bar group, similar to
        what `df.plot.bar()` would draw by default.
        The names in both the horizontal and vertical indices are used as
        labels in the legend.
    - A simple `list` or `np.array` of numbers. They will all be in a single group.

    The optional `labels` and `row_label` parameters are only used in the third
    case, as markers for the legend; and ignored in the first two cases.

    Returns `None`.
    """
    return impl.plot_scalars(**locals())


def plot_vector(label, xs, ys):
    """
    Plots a single vector as a "native" line chart - using the
    IDE's built-in drawing widget.

    # Parameters

    - **label**: A string that identifies this line on the plot. It will appear on the legend, so should be unique across invocations.
    - **xs**, **ys**: Lists or `np.array`s of numbers. Containing the X and Y coordinates (like time and value) of each point on the line,
      respectively. There is no requirement on either of these to hold positive, unique or monotonous values, so this method can be used to
      draw arbitrary scatter plots as well. They must be equal in length.

    Can be called repeatedly, each invocation adds a new line to the existing ones on the plot.

    Returns `None`.
    """
    return impl.plot_vector(**locals())


def plot_vectors(df_or_list):
    """
    Takes a set of vector type results and plots them as a "native" line chart - using the
    IDE's built-in drawing widget.

    `df_or_values` can be in one of three different formats:

    - A `pd.DataFrame` with (at least) `vectime` and `vecvalue` columns,
        for example as returned by `results.get_vectors()`
    - A `pd.DataFrame` containing only the numeric values. If there is a `"time"`
      column, that will provide the X coordinates, otherwise an artificial sequencial
      X coordinate series will be generated. Every column (other than "time") will be
      a different line.
    - A `list` of 3-tuples: `(label, x, y)`, each element of the list describes
      a line. The first element of each tuple is a string, which will be the label
      of the line, while the second and third are lists or `ndarray` of numbers.
      In every given tuple, the second and third element must be the equal length,
      but this length van differ between individual tuples.

    There is no requirement on the inputs to hold positive, unique or monotonous values,
    so this method can be used to draw arbitrary scatter plots as well.

    Returns `None`.
    """
    return impl.plot_vectors(**locals())


# TODO document, allow multiple iso columns, selecting the "value" column,
# maybe sorting by a different column than xdata
def plot_scatter(df, xdata, iso_column=None):
    """
    Pivots a DataFrame of numeric results (like scalars, itervars or statistics fields)
    and plots them on a line type chart as a scatter plot. The data points can be optionally
    separated into multiple isolines.

    # Parameters

    - **df**: A DataFrame that holds the data to be pivoted.
    - **xdata** *(string)*: The name of a column in `df` which will provide the X coordinate for the data points
    - **iso_column** *(string)*: Optional. The name of a column in `df` whose the values will be used to separate
      the data points into multiple isolines.

    Returns `None`.
    """
    return impl.plot_scatter(**locals())


def plot_histogram(label, binedges, binvalues, underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    """
    Plots a single histogram as a "native" chart - using the
    IDE's built-in drawing widget.

    # Parameters

    - **label**: A string that identifies this line on the plot. It will appear on the legend, so should be unique across invocations.
    - **binedges**, **binvalues**: Lists or `np.array`s of numbers. Containing the bin edges and the values of the histogram,
      respectively. `edges` should be one element longer than `values.
    - **underflows**, **overflows**: Optional. The sum of weights of the collected values that fell under or over the histogram bin range, respectively.
    - **minvalue**, **maxvalue**: Optional. The smallest and the highest collected value, respectively.

    Can be called repeatedly, each invocation adds a new histogram to the existing ones on the plot.

    Returns `None`.
    """
    return impl.plot_histogram(**locals())


def plot_histograms(df):
    """
    Takes a set of histogram type results and plots them as a "native" chart - using the
    IDE's built-in drawing widget.

    `df` is a DataFrame, containing (at least) `binedges` and `binvalues` columns,
    for example as returned by `results.get_histograms()`

    Returns `None`.
    """
    return impl.plot_histograms(**locals())


def set_plot_property(key, value):
    """
    Sets one property of the chart to the given value. If there was no property with the given
    name (key) yet, it is added.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. It is not persistent and is not reflected
    in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_plot_property(**locals())


def set_plot_properties(props):
    """
    Updates or adds any number of properties of the chart with the values given in `props.
    Any existing property values will be overwritten, and any new keys will be inserted.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. This change is not persistent and is not
    reflected in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_plot_properties(**locals())


def set_message(message):
    impl.set_message(**locals())
