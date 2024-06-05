"""
Provides access to the properties of the current chart for the chart script.

Note that this module is stateful. It is set up appropriately by the OMNeT++
IDE or `opp_charttool` before the chart script is run.
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

from ._version import __version__

class ChartScriptError(Exception):
    """
    Raised by chart scripts when they encounter an error.
    A message parameter can be passed to the constructor,
    which will be displayed on the plot area in the IDE.
    """
    pass


if os.getenv("WITHIN_OMNETPP_IDE") == "yes":
    from omnetpp.scave.impl_ide import chart as impl
else:
    from .impl import chart_charttool as impl


def get_properties():
    """
    Returns the currently set properties of the chart as a `dict`
    whose keys and values are both strings.
    """
    return impl.get_properties(**locals())


def get_property(key):
    """
    Returns the value of a single property of the chart, or `None` if there is
    no property with the given name (key) set on the chart.
    """
    return impl.get_property(**locals())

def get_name():
    """
    Returns the name of the chart as a string.
    """
    return impl.get_name(**locals())

def get_chart_type():
    """
    Returns the chart type, which is one of the strings "BAR", "LINE", "HISTOGRAM", and "MATPLOTLIB".
    """
    return impl.get_chart_type(**locals())

def is_native_chart():
    """
    Returns True if this chart uses the IDE's built-in plotting widgets.
    """
    return get_chart_type() != "MATPLOTLIB"

def set_suggested_chart_name(name):
    """
    Sets a proposed name for the chart. The IDE may offer this name to the user
    when saving the chart.
    """
    return impl.set_suggested_chart_name(**locals())

def set_observed_column_names(column_names):
    """
    Sets the DataFrame column names observed during the chart script.
    The IDE may use them for content assist when the user edits the legend format string.
    """
    return impl.set_observed_column_names(**locals())
