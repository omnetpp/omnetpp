#!/usr/bin/env python3

import os
import sys

from omnetpp.scave.libcharttool import *
from omnetpp.scave.impl_charttool import results, chart, plot
OPP_ROOT = get_opp_root() + "/"
TEMPLATE_DIR = OPP_ROOT + "ui/org.omnetpp.scave.templates/charttemplates/"

messages = list()

def warn(warning):
    global messages
    messages.append(warning)

plot.set_warning = warn

test_serial = 0
def test_chart(script, props):
    global test_serial
    c = Chart(test_serial, "matplotlib", "test chart", script, props)
    test_serial += 1

    messages.clear()
    try:
        print("--------")
        print(props)
        run_chart(OPP_ROOT + "test/scave/charttemplates", c, show=False, export=True)
    except BaseException as be:
        msgs_str = ""
        if messages:
            msgs_str = "; ".join(messages)
        raise RuntimeError(str(be) + "; " + msgs_str)

class Tester:
    def __init__(self):
        self.total = 0
        self.failed = 0
        self.faileds = []

    def run(self, scriptname, testparams, test, expect_exception=None, testlabel=None):
        self.total += 1
        failed = False
        try:
            test()
            if expect_exception:
                failed = True
        except BaseException as e:
            print("exception:", e)
            if not expect_exception or expect_exception not in str(e):
                failed = True

        if failed:
            self.failed += 1
            self.faileds.append((scriptname, testlabel, testparams))
            print("FAIL")
        else:
            print("PASS")

    def print_summary(self):
        print("tests run:", self.total, "failed:", self.failed)
        for sn, tl, tp in self.faileds:
            print("FAIL: in", sn, ", label", tl, ", params: ", tp)

def test_script(tester, scriptname, default_props, tests):
    with open(TEMPLATE_DIR + "/" + scriptname, "rt") as f:
        script = str(f.read())

    for propname, propvalue, exc in tests:
        params = {propname: propvalue}
        tester.run(scriptname, params, lambda:
                test_chart( script, {**default_props, **params}),
            exc)


if __name__ == "__main__":

    results.add_inputs(["/aloha/*.sca"], OPP_ROOT + "samples/resultfiles/", {})
    results.add_inputs(["/aloha/*.vec"], OPP_ROOT + "samples/resultfiles/", {})

    tester = Tester()

    test_script(tester, "barchart.py",
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
            ("groups", "numHosts", "not 1-dimensional"),
            ("groups", "experiment", None),
            ("groups", "name", None),
            ("groups", "aa bb", "No such iteration variable"),
            ("groups", "", "set both the Groups and Series properties"),

            ("series", "iaMean", "not 1-dimensional"),
            ("series", "numHosts", None),
            ("series", "experiment", None),
            ("series", "name", None),
            ("series", "", "set both the Groups and Series properties"),
        ]
    )

    test_script(tester, "linechart.py",
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

    test_script(tester, "scatterchart_itervars.py",
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
            ("xaxis_itervar", "numHosts", "not 1-dimensional"),
            ("xaxis_itervar", "experiment", None),
            ("xaxis_itervar", "name", None),
            ("xaxis_itervar", "aa bb", "iteration variable for the X axis could not be found"),
            ("xaxis_itervar", "", "select the iteration variable for the X axis"),

            ("iso_itervar", "iaMean", "not 1-dimensional"),
            ("iso_itervar", "numHosts", None),
            ("iso_itervar", "numHosts, replication", None),
            ("iso_itervar", "experiment", None),
            ("iso_itervar", "name", None),
            ("iso_itervar", "aa bb", "iteration variable for the iso lines could not be found"),
            ("iso_itervar", "", None),
        ]
    )

    test_script(tester, "histogramchart.py",
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

    test_script(tester, "generic_mpl.py",
        {
            "input": "Hello",
        },
        [
            ("input", "", None),
            ("input", "FooBar", None),
        ]
    )

    test_script(tester, "generic_xyplot.py",
        {
        },
        [
            ("dummy", "", None),
        ]
    )

    test_script(tester, "3dchart_itervars.py",
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
            ("xaxis_itervar", "numHosts", "not 1-dimensional"),
            ("xaxis_itervar", "aa bb", "iteration variable for the X axis could not be found"),
            ("xaxis_itervar", "", "set both the X Axis and Y Axis options"),

            ("yaxis_itervar", "iaMean", "not 1-dimensional"),
            ("yaxis_itervar", "numHosts", None),
            ("yaxis_itervar", "aa bb", "iteration variable for the Y axis could not be found"),
            ("yaxis_itervar", "", "set both the X Axis and Y Axis options"),

            ("chart_type", "points", None),
            ("chart_type", "surface", None),
            ("chart_type", "trisurf", None),
        ]
    )

    test_script(tester, "boxwhiskers.py",
        {
            "filter": "*:histogram"
        },
        [
            ("filter", "type =~ histogram", None),
            ("filter", "aa bb", "Syntax error"),
            ("filter", "", "Error while querying results: Empty filter expression"),
        ]
    )

    tester.print_summary()
