import os

import functools

print = functools.partial(print, flush=True)

from omnetpp.scave import results
from omnetpp.internal import Gateway


def get_configured_properties():
    return Gateway.properties_provider.getChartProperties()


def get_configured_property(key):
    return Gateway.properties_provider.getChartProperties()[key]  # TODO: could be optimized


def get_name():
    return Gateway.properties_provider.getChartName()

