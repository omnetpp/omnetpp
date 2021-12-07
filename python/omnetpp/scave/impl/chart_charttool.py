
properties = dict()
name = ""
chart_type = None # "bar"/"histogram"/"line"/"matplotlib"

def get_properties():
    return properties


def get_property(key):
    return properties[key]


def get_name():
    return name

def get_chart_type():
    return chart_type

def set_suggested_chart_name(name):
    pass

def set_observed_column_names(column_names):
    pass
