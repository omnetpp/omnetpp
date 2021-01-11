
import os
import sys
import traceback
import subprocess
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
from omnetpp.scave.impl_charttool import results, chart


class Chart:
    def __init__(self, index=int(), type=str(), name=str(), script=str(), properties=dict()):
        self.index = index
        self.type = type
        self.name = name
        self.script = script
        self.properties = properties

class Analysis:
    def __init__(self, inputs=list(), charts=list()):
        self.inputs = inputs
        self.charts = charts

def error(message):
    print("ERROR: " + message)
    sys.exit(1)


def _make_chart(i, c):
    script = c.get('script') if 'script' in c.keys() else c.text

    sn = c.find('script')
    if sn is not None and sn.text is not None:
        script = sn.text.strip() # should get the CDATA contents instead, but shouldn't matter much
    else:
        script = ""

    props = { p.get('name') : p.get('value') for p in c.findall('property') }
    return Chart(i, c.get('type'), c.get('name'), script, props)


def load_anf_file(anf_file_name):
    analysis = ET.parse(anf_file_name).getroot()

    version = analysis.get('version')

    if version != "2":
        error("Unsupported analysis file version: \"{}\" (only \"2\" is supported).".format(version))

    inputs = [i.get('pattern') for i in analysis.findall("inputs/input")]
    charts = [_make_chart(i, c) for i, c in enumerate(analysis.findall("charts/chart"))]

    return Analysis(inputs, charts)

def get_opp_root():
    try:
        out = subprocess.check_output("opp_configfilepath")
    except:
        error("Could not determine OMNeT++ directory (try \". setenv\" in this shell).")
    return os.path.abspath(os.path.dirname(out.decode('utf-8')))

def print_analysis_info(anf_file, analysis):
    print('\nThe "{}" file contains {} charts:\n'.format(anf_file, len(analysis.charts)))
    for i, c in enumerate(analysis.charts):
        print('\t{}.\t"{}"\t({})'.format(i, c.name, c.type))

    print("\nAnd {} inputs:\n".format(len(analysis.inputs)))
    for i, inp in enumerate(analysis.inputs):
        print('\t"{}"'.format(inp))


def run_chart(wd, c, export, show):
    assert(os.path.isabs(wd))

    os.chdir(wd)
    sys.path = [wd,
        get_opp_root() + "/ui/org.omnetpp.scave.pychart/python/"
    ] + sys.path

    chart.name = c.name
    chart.chart_type = c.type
    chart.properties.clear()
    chart.properties.update(c.properties)

    global _show_called
    _show_called = False

    got_error = False
    try:
        exec(c.script, {})
    except:
        print("Error in chart " + c.name, file = sys.stderr)
        print(traceback.format_exc(), file = sys.stderr)
        got_error = True

    if export:
        # maybe warn if _show_called is set
        # TODO: heuristic: only prepend index if there would be collision (either among exported charts, or with existing files?)
        plt.savefig(str(c.index) + "-" + c.name + ".png")
    else:
        if show and not _show_called:
            plt.show()

    return not got_error
