import os

import functools

print = functools.partial(print, flush=True)

from omnetpp.internal import Gateway

# this is a copy of the property map queried from Java, since
# it is not supposed to be modifiable by the script
_properties = None

def get_properties():
    global _properties
    if _properties is None:
        # converting it into a regular dict, instead of the Py4J "Java Map emulator",
        # so we get a KeyError when getting a non-existent key, instead of None
        props = Gateway.chart_provider.getChartProperties()

        _properties = dict()
        for k in props:
            _properties[k] = props[k]

        # this would be simpler, and it is _supposed_ to work, but sometimes
        # it triggers a "Received empty command" error inside Py4J...
        # _properties = dict(props)

    return _properties

def get_property(key):
    return get_properties()[key]

def get_name():
    return Gateway.chart_provider.getChartName()

def get_chart_type():
    return Gateway.chart_provider.getChartType()

def set_suggested_chart_name(name):
    return Gateway.chart_provider.setSuggestedChartName(name)

def set_observed_column_names(column_names):
    return Gateway.chart_provider.setObservedColumnNames([str(n) for n in column_names])
