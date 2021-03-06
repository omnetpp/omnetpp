#!/usr/bin/env python3

import os
import sys

from omnetpp.scave.analysis import *

TEMPLATE_DIR = "../../../ui/org.omnetpp.scave.templates/charttemplates/"

skeleton = """
import test_exceptions
test_exceptions.messages.clear()
try:
{}
    raise RuntimeError("Expected error did not occur")
except SystemExit as e: # only scripts that exit via exit(1) are accepted, let all other exceptions through
    test_exceptions.expect_message({})
"""

def indent(s):
    return "\n".join(["    " + l for l in s.splitlines()])

def generate_testcases(scriptname, base_props, tests):
    with open(TEMPLATE_DIR + "/" + scriptname, "rt") as f:
        origscript = str(f.read())

    charts = []

    i = 0
    for propname, propvalue, errmsg in tests:
        name = scriptname.replace(".py", "_" + str(i))
        i = i + 1
        props = base_props.copy()
        props.update({propname: propvalue})
        script = skeleton.format(indent(origscript), repr(errmsg)) if errmsg else origscript

        chart = Chart(type="MATPLOTLIB", name=name, script=script, properties=props)
        charts.append(chart)

    return charts


charts = []

charts += generate_testcases("barchart.py",
    {
        "filter": "name =~ channelUtilization:last",
        "groups": "iaMean",
        "series": "numHosts"
    },
    [
        ("filter", "type =~ scalar", None),
        ("filter", "name =~ channel*", None),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),

        ("groups", "iaMean", None),
        ("groups", "numHosts", "Overlap between Group and Series columns"),
        ("groups", "experiment", None),
        ("groups", "name", None),
        ("groups", "aa bb", "No such iteration variable"),
        ("groups", "", "set both the Groups and Series properties"),

        ("series", "iaMean", "Overlap between Group and Series columns"),
        ("series", "numHosts", None),
        ("series", "experiment", None),
        ("series", "name", None),
        ("series", "", "set both the Groups and Series properties"),
    ]
)

charts += generate_testcases("linechart.py",
    {
        "filter": "name =~ radioState:vector",
        "vector_start_time": "",
        "vector_end_time": "",
        "vector_operations": ""
    },
    [
        ("filter", "type =~ vector", None),
        ("filter", "name =~ *State:vector", None),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),

        ("vector_start_time", "10", None),
        ("vector_end_time", "20", None),

        ("vector_operations", "apply:mean", None),
        ("vector_operations", "compute:sum", None),

        ("vector_operations", "apply:sum\ncompute:divtime\napply:timewinavg(window_size=200) # comment", None)
    ]
)

charts += generate_testcases("scatterchart_itervars.py",
    {
        "filter": "name =~ channelUtilization:last",
        "xaxis_itervar": "iaMean",
        "iso_itervar": "numHosts"
    },
    [
        ("filter", "type =~ scalar", None),
        ("filter", "name =~ channel*", None),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),

        ("xaxis_itervar", "iaMean", None),
        ("xaxis_itervar", "numHosts", "X axis column also in iso line columns:"),
        ("xaxis_itervar", "experiment", None),
        ("xaxis_itervar", "name", None),
        ("xaxis_itervar", "aa bb", "iteration variable for the X axis could not be found"),
        ("xaxis_itervar", "", "select the iteration variable for the X axis"),

        ("iso_itervar", "iaMean", "X axis column also in iso line columns:"),
        ("iso_itervar", "numHosts", None),
        ("iso_itervar", "numHosts, replication", None),
        ("iso_itervar", "experiment", None),
        ("iso_itervar", "name", None),
        ("iso_itervar", "aa bb", "iteration variable for the iso lines could not be found"),
        ("iso_itervar", "", None),
    ]
)

charts += generate_testcases("histogramchart.py",
    {
        "filter": "name =~ channelUtilization:last",
    },
    [
        ("filter", "type =~ histogram", None),
        ("filter", "nonexistent", "returned no data"),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),
    ]
)

charts += generate_testcases("generic_mpl.py",
    {
        "input": "Hello",
    },
    [
        ("input", "", None),
        ("input", "FooBar", None),
    ]
)

charts += generate_testcases("generic_xyplot.py",
    {
    },
    [
        ("dummy", "", None),
    ]
)

charts += generate_testcases("3dchart_itervars.py",
    {
        "filter": "name =~ channelUtilization:last",
        "xaxis_itervar": "iaMean",
        "yaxis_itervar": "numHosts",
        "colormap": "viridis",
        "colormap_reverse": "false",
        "chart_type": "bar"
    },
    [
        ("filter", "type =~ scalar", "scalars must share the same name"),
        ("filter", "name =~ channel*", None),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),

        ("xaxis_itervar", "iaMean", None),
        ("xaxis_itervar", "numHosts", "The itervar for the X and Y axes are the same"),
        ("xaxis_itervar", "aa bb", "iteration variable for the X axis could not be found"),
        ("xaxis_itervar", "", "set both the X Axis and Y Axis options"),

        ("yaxis_itervar", "iaMean", "The itervar for the X and Y axes are the same"),
        ("yaxis_itervar", "numHosts", None),
        ("yaxis_itervar", "aa bb", "iteration variable for the Y axis could not be found"),
        ("yaxis_itervar", "", "set both the X Axis and Y Axis options"),

        ("chart_type", "points", None),
        ("chart_type", "surface", None),
        ("chart_type", "trisurf", None),
    ]
)

charts += generate_testcases("boxwhiskers.py",
    {
        "filter": "*:histogram"
    },
    [
        ("filter", "type =~ histogram", None),
        ("filter", "aa bb", "Syntax error"),
        ("filter", "", "Error while querying results: Empty filter expression"),
    ]
)

inputs = [ "/resultfiles/aloha/*.sca", "/resultfiles/aloha/*.vec" ]
analysis = Analysis(inputs, charts=charts)

analysis.to_anf_file("all_the_tests.anf")
