import os

import functools

print = functools.partial(print, flush=True)

from omnetpp.scave import results, plot
from omnetpp.internal import Gateway


def get_properties():
    return Gateway.properties_provider.getChartProperties()


def get_property(key):
    return Gateway.properties_provider.getChartProperties()[key]  # TODO: could be optimized


def get_name():
    return Gateway.properties_provider.getChartName()

def get_chart_type():
    return Gateway.properties_provider.getChartType()

def set_suggested_chart_name(name):
    return Gateway.properties_provider.setSuggestedChartName(name)
