import os

import functools

print = functools.partial(print, flush=True)

from omnetpp.scave import results, plot
from omnetpp.internal import Gateway


def get_properties():
    # converting it into a regular dict, instead of the Py4J "Java Map emulator",
    # so we get a KeyError when getting a non-existent key, instead of None
    return dict(Gateway.chart_provider.getChartProperties())

def get_property(key):
    return get_properties()[key]  # TODO: could be optimized

def get_name():
    return Gateway.chart_provider.getChartName()

def get_chart_type():
    return Gateway.chart_provider.getChartType()

def set_suggested_chart_name(name):
    return Gateway.chart_provider.setSuggestedChartName(name)
