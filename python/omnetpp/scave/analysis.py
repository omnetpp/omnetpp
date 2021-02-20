import os
import sys
import traceback
import subprocess
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom # because ET cannot pretty-print
import matplotlib.pyplot as plt
import omnetpp.scave.impl_charttool.results as results_module
import omnetpp.scave.impl_charttool.chart as chart_module
import omnetpp.scave.utils as utils


"""
For reading/writing OMNeT++ Analysis (.anf) files, and executing charts scripts in them.
"""

_real_show = plt.show

def _fake_show(*args, **kwargs):
    global _show_called
    _real_show(*args, **kwargs)
    _show_called = True

plt.show = _fake_show

class DialogPage:
    """
    Represents a dialog page in a Chart. Dialog pages have an ID, a label
    (which the IDE displays on the page's tab in the Chart Properties dialog),
    and XSWT content (which describes the UI controls on the page).
    """
    def __init__(self, id:str=None, label:str="", content:str=""):
        self.id = id
        self.label = label
        self.content = content

class Chart:
    """
    Represents a chart in an Analysis. Charts have an ID, a name, a chart script
    (a Python script that mainly uses Pandas and the `omnetpp.scave.*` modules),
    dialog pages (which make up the contents of the Chart Properties dialog in the IDE),
    and properties (which are what the Chart Properties dialog in the IDE edits).
    """
    def __init__(self, type:str, name:str="", id:str=None, template:str=None, icon:str=None, script:str="", dialog_pages=list(), properties=dict()):
        assert type in ["MATPLOTLIB", "BAR", "LINE"]
        self.type = type
        self.name = name
        self.id = id
        self.template = template
        self.icon = icon
        self.script = script
        self.dialog_pages = dialog_pages
        self.properties = properties

class Workspace:
    """
    This is an abstraction of an IDE workspace, and makes it possible to map
    workspace paths to filesystem paths. This is necessary because the inputs
    in the Analysis are workspace paths.
    """
    def __init__(self, workspace_dir, project_paths):
        self.workspace_dir = workspace_dir
        self.project_paths = project_paths

    def to_filesystem_path(self, workspace_path):
        pass #TODO

class Analysis:
    """
    Represents an OMNeT++ Analysis, i.e. the contents of an  `anf` file. Methods
    allow reading/writing `anf` files, and running the charts in them for interactive
    display, image/data export or other side effects.
    """
    def __init__(self, inputs=list(), charts=list()):
        self.inputs = inputs
        self.charts = charts

    @staticmethod
    def from_anf_file(anf_file_name):
        """
        Reads the given anf file and returns its content as an `Analysis` object.
        """
        analysis = ET.parse(anf_file_name).getroot()
        version = analysis.get('version')
        if version != "2":
            raise RuntimeError("Unsupported analysis file version: \"{}\" (only \"2\" is supported).".format(version))

        inputs = [input_elem.get('pattern') for input_elem in analysis.findall("inputs/input")]
        charts = [Analysis._make_chart(chart_elem) for chart_elem in analysis.findall("charts/chart")]
        return Analysis(inputs, charts)

    @staticmethod
    def _make_chart(chart_elem):
        def content(element):
            if element is not None and element.text is not None:
                return element.text.strip()+"\n" # should get the CDATA contents instead, but shouldn't matter much
            return ""

        script = content(chart_elem.find('script'))
        dialog_pages = [ DialogPage(id = dp.get('id'), label = dp.get('label'), content = content(dp)) for dp in chart_elem.findall('dialogPage') ]
        props = { p.get('name') : p.get('value') for p in chart_elem.findall('property') }

        return Chart(
            id = chart_elem.get('id'),
            type = chart_elem.get('type'),
            name = chart_elem.get('name'),
            template = chart_elem.get('template'),
            icon = chart_elem.get('icon'),
            script = script,
            dialog_pages = dialog_pages,
            properties = props)

    def info(self):
        """
        Produces a multi-line string description about the content of this Analysis.
        """
        s =  '{} charts:\n\n'.format(len(self.charts))
        for i, c in enumerate(self.charts):
            s += '\t{}.\t"{}"\t({})\n'.format(i, c.name, c.type)

        s += "\n{} inputs:\n\n".format(len(self.inputs))
        for i, inp in enumerate(self.inputs):
            s += '\t"{}"\n'.format(inp)

        return s

    def run_chart(self, chart, wd, workspace, extra_props=dict(), show=False):
        """
        Runs a chart script with the given working directory, workspace, and extra
        properties in addition to the chart's properties. If `show=True`, it calls
        `plt.show()` if it was not already called by the script.
        """
        assert(os.path.isabs(wd))

        # set up state in 'results' module
        results_module.set_inputs(self.inputs, workspace.workspace_dir, workspace.project_paths)
        results_module.wd = workspace.workspace_dir

        # set up state in 'chart' module
        chart_module.name = chart.name
        chart_module.chart_type = chart.type
        chart_module.properties.clear()
        chart_module.properties.update(chart.properties)
        chart_module.properties.update(extra_props)

        plt.clf()

        global _show_called
        _show_called = False

        try:
            os.chdir(wd)
            exec(chart.script, {})
        except SystemExit as se:
            if se.code != 0:
                raise RuntimeError("Chart script exited with code " + str(se.code))

        if show and not _show_called:
            plt.show()

    def export_image(self, chart, wd, workspace, format="svg", target_folder=None, filename=None, width=6, height=4, dpi=96, enforce=True, extra_props=dict()):
        """
        Runs a chart script for image export. This method just calls `run_chart()`
        with extra properties that instruct the chart script to perform image export.
        (It is assumed that the chart script invokes `utils.export_image_if_needed()`
        or implements equivalent functionality).
        """
        props = dict()
        props.update(extra_props)

        props['export_image'] = "true"
        if format:
            props['image_export_format'] = str(format)
        if target_folder:
            props['image_export_folder'] = str(target_folder)
        if filename:
            props['image_export_filename'] = str(filename)
        if width:
            props['image_export_width'] = str(width)
        if height:
            props['image_export_height'] = str(height)
        if dpi:
            props['image_export_dpi'] = str(dpi)

        self.run_chart(chart, wd, workspace, extra_props=props, show=False)

        fname = utils.get_image_export_filepath(props)
        return self._check_file_created(fname, "image", enforce)

    def export_data(self, chart, wd, workspace, format="svg", target_folder=None, filename=None, width=6, height=4, dpi=96, enforce=True, extra_props=dict()):
        """
        Runs a chart script for data export. This method just calls `run_chart()`
        with extra properties that instruct the chart script to perform data export.
        (It is assumed that the chart script invokes `utils.export_data_if_needed()`
        or implements equivalent functionality).
        """
        props = dict()
        props.update(extra_props)

        props['export_data'] = "true"
        if target_folder:
            props['data_export_folder'] = str(target_folder)
        if filename:
            props['data_export_filename'] = str(filename)

        self.run_chart(chart, wd, workspace, extra_props=props, show=False)

        fname = utils.get_image_export_filepath(props)
        return self._check_file_created(fname, "data", enforce)

    def _check_file_created(self, fname, what, enforce):
        if not os.path.isfile(fname):
            msg = "Chart script silently failed to create {} file '{}'".format(what, fname)
            if enforce:
                raise RuntimeError(msg)
            else:
                print("WARNING: " + msg, file=sys.stderr)
                return None
        return fname

    def to_anf_file(self, filename):
        """
        Saves the analysis to the given .anf file.
        """
        domTree = minidom.parseString("<analysis/>")
        analysisEl = domTree.documentElement

        # utility functions
        def appendChild(parent, tag):
            return parent.appendChild(domTree.createElement(tag))
        def setAttr(element, name, value):
            if value is not None:
                element.setAttribute(name, str(value))
        def setContent(element, text):
            element.appendChild(domTree.createCDATASection(text))
            element.appendChild(domTree.createTextNode("\n"))

        # build DOM tree
        inputsEl = appendChild(analysisEl,"inputs")
        for input in self.inputs:
            inputEl = appendChild(inputsEl, "input")
            setAttr(inputEl, "pattern", input)
            inputsEl.appendChild(inputEl)
        chartsEl = appendChild(analysisEl, "charts")
        for chart in self.charts:
            chartEl = appendChild(chartsEl, "chart")
            setAttr(chartEl, "id", chart.id)
            setAttr(chartEl, "type", chart.type)
            setAttr(chartEl, "name", chart.name)
            setAttr(chartEl, "template", chart.template)
            setAttr(chartEl, "icon", chart.icon)
            scriptEl = appendChild(chartEl, "script")
            setContent(scriptEl, chart.script)
            for dialog_page in chart.dialog_pages:
                dialogpageEl = appendChild(chartEl, "dialogPage")
                setAttr(dialogpageEl, "id", dialog_page.id)
                setAttr(dialogpageEl, "label", dialog_page.label)
                setContent(dialogpageEl, dialog_page.content)
            for key,value in chart.properties.items():
                propertyEl = appendChild(chartEl, "property")
                setAttr(propertyEl, "name", key)
                setAttr(propertyEl, "value", value)

        # write to file
        with open(filename, 'w') as f:
            domTree.writexml(f, addindent="    ", newl="\n")

def load_anf_file(anf_file_name):
    return Analysis.from_anf_file(anf_file_name)
