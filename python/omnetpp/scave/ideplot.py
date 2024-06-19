"""
This module is the interface for displaying plots using the IDE's native
(non-Matplotlib) plotting widgets from chart scripts. The API is intentionally
very close to `matplotlib.pyplot`: most functions and the parameters they accept
are a subset of `pyplot`'s interface. If one restricts themselves to a subset of
Matplotlib's functionality, switching between `omnetpp.scave.ideplot` and
`matplotlib.pyplot` in a chart script may be as simple as much as editing the
`import` statement.

When the API is used outside the context of a native plotting widget (such as
during the run of `opp_charttool`, or in IDE during image export), the functions
are emulated with Matplotlib.

Note that this module is stateful. It is set up appropriately by the OMNeT++ IDE
or `opp_charttool` before the chart script is run.
"""

import math

from ._version import __version__

def is_native_plot():
    """
    Returns True if the script is running in the context of a native plotting
    widget, and False otherwise.
    """
    try:
        from omnetpp.internal import Gateway
        if Gateway.chart_plotter:
            return True
    except:
        pass
    return False

# Import one of the two implementations, based on whether we are running
# in the context of a native plotting widget or not.
if is_native_plot():
    from omnetpp.scave.impl_ide import ideplot as impl
else:
    from .impl import ideplot_matplotlib as impl



def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    """
    Plot ys versus xs as lines and/or markers. Call `plot` multiple times
    to plot multiple sets of data.

    Parameters:

    - `xs`, `ys` (array-like or scalar): The horizontal / vertical coordinates of the data points.
    - `key` (string): Identifies the series in the native plot widget.
    - `label` (string): Series label for the legend
    - `drawstyle` (string): Matplotlib draw style ('default', 'steps', 'steps-pre', 'steps-mid', 'steps-post')
    - `linestyle` (string): Matplotlib line style ('-', '--', '-.', ':', etc)
    - `linewidth` (float): Line width in pixels
    - `color` (string): Matplotlib color name or abbreviation ('b' for blue, 'g' for green, etc.)
    - `marker` (string): Matplotlib marker name ('.', ',', 'o', 'x', '+', etc.)
    - `markersize` (float): Size of markers in pixels.
    """
    return impl.plot(**locals())

# note: the default of histtype is not bar, because we don't support that
# also, minvalue, maxvalue, underflows and overflows are NOT accepted by mpl.pyplot.hist
def hist(x, bins, key=None, density=False, weights=None, cumulative=False, bottom=None, histtype='stepfilled', color=None, label=None, linewidth=None,
         underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    """
    Make a histogram plot. This function adds one histogram to the bar plot; make
    multiple calls to add multiple histograms.

    Parameters:

    - `x` (array-like): Input values.
    - `bins` (array-like): Bin edges, including the left edge of the first bin and the right edge of the last bin.
    - `key` (string): Identifies the series in the native plot widget.
    - `density` (bool): See `mpl.hist()`.
    - `weights` (array-like): Weights.
    - `cumulative` (bool): See `mpl.hist()`.
    - `bottom` (float): Location of the bottom baseline for bins.
    - `histtype` (string): Whether to fill the area under the plot. Accepted values are 'step' and 'stepfilled'.
    - `color` (string): Matplotlib color name or abbreviation ('b' for blue, 'g' for green, etc.)
    - `label` (string): Series label for the legend
    - `linewidth` (float): Line width in pixels
    - `underflows`, `overflows`: Number of values / sum of weights outside the histogram bins in both directions.
    - `minvalue`, `maxvalue`: The minimum and maximum value, or `nan` if unknown.

    Restrictions:

    1. Overflow bin data (minvalue, maxvalue, underflows, and overflows) is not accepted by `pyplot.hist()`.
    2. The native plot widget only accepts a precomputed histogram (using the trick documented for `pyplot.hist()`)
    """
    return impl.hist(**locals())

def bar(x, height, width=0.8, key=None, label=None, color=None, edgecolor=None):
    """
    Make a bar plot. This function adds one series to the bar plot; make
    multiple calls to add multiple series.

    The bars are positioned at x with the given alignment. Their dimensions
    are given by width and height. The vertical baseline is bottom (default 0).

    Each of x, height, width, and bottom may either be a scalar applying to
    all bars, or it may be a sequence of length N providing a separate value
    for each bar.

    Parameters:

    - `x` (sequence of scalars): The x coordinates of the bars.
    - `height` (scalar or sequence of scalars): The height(s) of the bars.
    - `width` (scalar or array-like): The width(s) of the bars.
    - `key` (string): Identifies the series in the native plot widget.
    - `label` (string): The label of the series the bars represent.
    - `color` (string): The fill color of the bars.
    - `edgecolor` (string): The edge color of the bars.

    The native plot implementation has the following restrictions:

    - widths are automatic (the parameter is ignored)
    - x coordinates are automatic (values are ignored)
    - height must be a sequence (cannot be a scalar)
    - in multiple calls to bar(), the lengths of the height sequence must be
      equal (i.e. all series must have the same number of values)
    - the default color is grey (Matplotlib assigns a different color to each series)
    """
    return impl.bar(**locals())

def set_property(key, value):
    """
    Sets one property of the native plot widget to the given value. When invoked
    outside the context of a native plot widget, the function does nothing.

    Parameters:

    - `key` (string): Name of the property.
    - `value` (string): The value to set. If any other type other than a string is passed in, it will be converted to a string.
    """
    return impl.set_property(**locals())

def set_properties(props):
    """
    Sets several properties of the native plot widget. It is functionally equivalent to
    repeatedly calling `set_property` with the entries of the `props` dictionary.
    When invoked outside the context of a native plot widget (TODO?), the function does nothing.

    Parameters:

    - `props` (dict): The properties to set.
    """
    return impl.set_properties(**locals())

def get_supported_property_keys():
    """
    Returns the list of property names that the native plot widget supports, such as
    'Plot.Title', 'X.Axis.Max', and 'Legend.Display', among many others.

    Note: This method has no equivalent in `pyplot`. When the script runs outside the IDE (TODO?),
    the method returns an empty list.
    """
    return impl.get_supported_property_keys(**locals())

def set_warning(warning: str):
    """
    Displays the given warning text in the plot.
    """
    impl.set_warning(**locals())

def title(label: str):
    """
    Sets the plot title to the given string.
    """
    impl.title(**locals())

def xlabel(xlabel: str):
    """
    Sets the label of the X-axis to the given string.
    """
    impl.xlabel(**locals())

def ylabel(ylabel: str):
    """
    Sets the label of the Y-axis to the given string.
    """
    impl.ylabel(**locals())

def xlim(left=None, right=None):
    """
    Sets the limits of the X-axis.

    Parameters:

    - `left` (float): The left xlim in data coordinates. Passing None leaves the limit unchanged.
    - `right` (float): The right xlim in data coordinates. Passing None leaves the limit unchanged.
    """
    impl.xlim(**locals())

def ylim(bottom=None, top=None):
    """
    Sets the limits of the Y-axis.

    Parameters:

    - `bottom` (float): The bottom ylim in data coordinates. Passing None leaves the limit unchanged.
    - `top` (float): The top ylim in data coordinates. Passing None leaves the limit unchanged.
    """
    impl.ylim(**locals())

def xscale(value: str):
    """
    Sets the scale of the X-axis. Possible values are 'linear' and 'log'.
    """
    impl.xscale(**locals())

def yscale(value: str):
    """
    Sets the scale of the Y-axis.
    """
    impl.yscale(**locals())

def xticks(ticks=None, labels=None, rotation=0):
    """
    Sets the current tick locations and labels of the x-axis.

    Parameters:

    - `ticks` (array_like): A list of positions at which ticks should be placed. You can pass an empty list to disable xticks.
    - `labels` (array_like): A list of explicit labels to place at the given locs.
    - `rotation` (float): Label rotation in degrees.
    """
    impl.xticks(**locals())

def grid(b=True, which="major"):
    """
    Configure the grid lines.

    Parameters:

    - `b` (bool or `None`): Whether to show the grid lines.
    - `which` ('major', 'minor', or 'both'): The grid lines to apply the changes to.
    """
    impl.grid(**locals())

def legend(show=None, frameon=None, loc=None):
    """
    Place a legend on the axes.

    Parameters:

    - `show` (bool or `None`): Whether to show the legend. TODO does pyplot have this?
    - `frameon` (bool or `None`): Control whether the legend should be drawn on a patch (frame).
        Default is `None`, which will take the value from the resource file.
    - `loc` (string or `None`): The location of the legend. Possible values are
        'best', 'upper right', 'upper left', 'lower left', 'lower right', 'right',
        'center left', 'center right', 'lower center', 'upper center', 'center'
        (these are the values supported by Matplotlib), plus additionally
        'outside top left', 'outside top center', 'outside top right',
        'outside bottom left', 'outside bottom center', 'outside bottom right',
        'outside left top', 'outside left center', 'outside left bottom',
        'outside right top', 'outside right center', 'outside right bottom'.
    """
    impl.legend(**locals())

