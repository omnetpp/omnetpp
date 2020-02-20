"""
This module provides functions to access the properties of the chart object.
"""

# Technically, this module only delegates all of its functions to one of two different
# implementations of the API described here, based on whether it is running from within the IDE
# or not (for example, from opp_charttool), detected using the WITHIN_OMNETPP_IDE environment variable.

import os

if os.getenv("WITHIN_OMNETPP_IDE") == "yes":
    from omnetpp.scave.impl_ide import chart as impl
else:
    from .impl_charttool import chart as impl


def get_properties():
    """
    Returns the currently set properties of the chart as a `dict`
    whose keys and values are both all strings.
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
    Returns the chart type, one of the strings "bar"/"histogram"/"line"/"matplotlib"
    """
    return impl.get_chart_type(**locals())

def is_native_chart():
    """
    Returns True if this chart uses the IDE's built-in plotting widgets.
    """
    return get_chart_type() != "matplotlib"

def set_suggested_chart_name(name):
    """
    Sets a proposed name for the chart. The IDE may offer this name to the user
    when saving the chart.
    """
    return impl.set_suggested_chart_name(**locals())

