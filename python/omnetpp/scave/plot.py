"""
This module is the interface for displaying data using the built-in plot widgets (i.e. without matplotlib).
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import matplotlib as mpl

# Note: For chart export we use the Matplotlib impl even inside the IDE, 
# that's why we check the Matplotlib backend instead of "WITHIN_OMNETPP_IDE".
if "omnetpp" in mpl.get_backend():
    from omnetpp.scave.impl_ide import plot as impl
else:
    from .impl_charttool import plot as impl

import math
import numpy as np


def plot_bars(df):
    return impl.plot_bars(**locals())

def plot_lines(df):
    return impl.plot_lines(**locals())

def plot_histograms(df):
    return impl.plot_histograms(**locals())


def plot(xs, ys, key=None, label=None, drawstyle=None, linestyle=None, linewidth=None, color=None, marker=None, markersize=None):
    return impl.plot(**locals())

# note: the default of histtype is not bar, because we don't support that
# also, minvalue, maxvalue, underflows and overflows are NOT accepted by mpl.pyplot.hist
def hist(x, bins, density=None, weights=None, cumulative=False, bottom=None, histtype='stepfilled', color=None, label=None, linewidth=None,
         underflows=0.0, overflows=0.0, minvalue=math.nan, maxvalue=math.nan):
    return impl.hist(**locals())

def bar(x, height, width=0.8, label=None, color=None, edgecolor=None):
    return impl.bar(**locals())


def set_property(key, value):
    """
    Sets one property of the chart to the given value. If there was no property with the given
    name (key) yet, it is added.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. It is not persistent and is not reflected
    in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_property(**locals())


def set_properties(props):
    """
    Updates or adds any number of properties of the chart with the values given in `props.
    Any existing property values will be overwritten, and any new keys will be inserted.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. This change is not persistent and is not
    reflected in later calls to `get_property()` or `get_properties()`.
    """
    return impl.set_properties(**locals())


def get_supported_property_keys():
    """
    Updates or adds any number of properties of the chart with the values given in `props.
    Any existing property values will be overwritten, and any new keys will be inserted.

    Please note that this function does not change or affect the actual chart
    object at all (as that is treated strictly as a read-only input); instead, it only
    changes some visual property on its view. This change is not persistent and is not
    reflected in later calls to `get_property()` or `get_properties()`.
    """
    return impl.get_supported_property_keys(**locals())


def set_warning(warning):
    """
    Displays the given warning text in the plot.
    """
    impl.set_warning(**locals())


def title(label):
    """
    Sets plot title.
    """
    impl.title(**locals())

def xlabel(xlabel):
    """
    Sets the label of the X axis.
    """
    impl.xlabel(**locals())

def ylabel(ylabel):
    """
    Sets the label of the Y axis.
    """
    impl.ylabel(**locals())

def xlim(left=None, right=None):
    """
    Sets the limits of the X axis.
    """
    impl.xlim(**locals())

def ylim(bottom=None, top=None):
    """
    Sets the limits of the Y axis.
    """
    impl.ylim(**locals())

def xscale(value):
    """
    Sets the scale of the X axis. Possible values are 'linear' and 'log'.
    """
    impl.xscale(**locals())

def yscale(value):
    """
    Sets the scale of the Y axis. Possible values are 'linear' and 'log'.
    """
    impl.yscale(**locals())

def xticks(ticks=None, labels=None, rotation=0):
    impl.xticks(**locals())

def grid(b=True, which="major"):
    impl.grid(**locals())

def legend(show=None, frameon=None, loc=None):
    impl.legend(**locals())

