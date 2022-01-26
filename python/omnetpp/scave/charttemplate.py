"""
Loading and instantiating chart templates.
"""

import os, sys, glob, time
import omnetpp.scave.analysis as analysis
import xml.etree.ElementTree as ET

from ._version import __version__

class ChartTemplate:
    """
    Represents a chart template.
    """
    def __init__(self, id:str, name:str, type:str, icon:str, script:str, dialog_pages, properties):
        """
        Creates a chart template from the given data elements.

        Parameters:

        - `id` (string): A short string that uniquely identifies the chart template.
        - `name` (string): Name of the chart template.
        - `type` (string): Chart type, one of: "bar"/"histogram"/"line"/"matplotlib".
        - `icon` (string): Name of the icon to be used for charts of this type.
        - `script` (string): The Python chart script.
        - `dialog_pages` (list of `DialogPage`): Pages of the "Configure Chart" dialog.
        - `properties` (string->string dictionary): Initial values for chart properties.
        """
        assert type in ["MATPLOTLIB", "BAR", "LINE", "HISTOGRAM"]
        self.id = id
        self.name = name
        self.type = type

        self.icon = icon
        self.script = script
        self.dialog_pages = dialog_pages.copy()
        self.properties = properties.copy()

    def __repr__(self):
        return self.id

    def create_chart(self, id:int=None, name:str=None, props=None):
        """
        Creates and returns a chart object (`org.omnetpp.scave.Chart`) from
        this chart template. Chart properties will be set to the default
        values defined by the chart template. If a `props` argument is present,
        property values in it will overwrite the defaults.

        Parameters:

        - `id` (string): A numeric string that identifies the chart within the
           Analysis. Auto-assigned if missing.
        - `name` (string): Name for the chart. If missing, the chart template name
           will be used.
        - `props` (string->string dictionary): Chart properties to set. It may not
           introduce new properties, i.e. the keys must be subset of the property
           keys defined in the chart template.
        """
        if name is None:
            name = self.name
        all_props = self.properties.copy()
        if props:
            unknown_keys = [key for key in props.keys() if key not in self.properties.keys()]
            if unknown_keys:
                raise KeyError("Refusing to set chart properties not defined in the chart template {}: {}".format(repr(self.id), str(unknown_keys)))
            all_props.update(props)
        return analysis.Chart(type=self.type, name=name, template=self.id, icon=self.icon, script=self.script, dialog_pages=self.dialog_pages.copy(), properties=all_props)

def get_chart_template_locations():
    """
    Returns a list of locations (directories) where the chart templates that
    come with the IDE can be found.
    """
    omnetpp_root = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../.."))
    src_dir = os.path.join(omnetpp_root, "ui/org.omnetpp.scave.templates/charttemplates")
    install_dir = os.path.join(omnetpp_root, "ide/plugins/org.omnetpp.scave.templates_*/charttemplates")
    dirs = glob.glob(src_dir) + glob.glob(install_dir)
    return dirs

def load_chart_templates(dirs=[], add_default_locations=True):
    """
    Loads chart templates from the given list of directories, and returns them
    in a dictionary. Chart templates are loaded from files with the `.properties`
    extension.

    Parameters:

    - `dirs` (string list): A short string that uniquely identifies the chart template.
    - `add_default_locations` (bool): Whether the directories returned by `get_chart_template_locations()`
      should also be searched in addition to the specified directory list.

    Returns:

    - A string->ChartTemplate dictionary, with chart template IDs used as keys.
    """
    if isinstance(dirs, tuple):
        dirs = list(dirs)
    if not isinstance(dirs, list):
        dirs = [ dirs ]
    if add_default_locations:
        dirs = dirs + get_chart_template_locations()
    templates = {}
    for dir in dirs:
        for file in glob.glob(dir+"/*.properties"):
            template = load_chart_template(file)
            templates[template.id] = template
    return templates

def load_chart_template(properties_file):
    """
    Loads the chart template from the specified `.properties` file, and
    returns it as a `ChartTemplate` object.
    """
    # read properties file
    p = _read_properties_file(properties_file)
    def get(p,key):
        return p[key] if key in p else None
    dir = os.path.dirname(properties_file)

    # mandatory elements
    id = p["id"]
    name = p["name"]
    type = p["type"]
    script_file = p["scriptFile"]
    script = open(os.path.join(dir, script_file)).read()

    # optional elements
    icon = get(p,"icon")
    resultTypes = get(p,"resultTypes")
    description = get(p,"description")
    score = get(p,"score")

    # load dialog pages
    dialog_pages = list()
    i = 0
    while True:
        page_id = get(p, "dialogPage." + str(i) + ".id")
        if page_id is None:
            break
        page_label = get(p, "dialogPage." + str(i) + ".label")
        xswt_file = get(p, "dialogPage." + str(i) + ".xswtFile")
        xswt_content = open(os.path.join(dir, xswt_file)).read()
        dialog_page = analysis.DialogPage(page_id, page_label, xswt_content)
        dialog_pages.append(dialog_page)
        i = i + 1

    # parse XSWT pages for property default values
    properties = _collect_editable_properties(dialog_pages)

    return ChartTemplate(id=id, name=name, type=type, icon=icon, script=script, dialog_pages=dialog_pages, properties=properties)

# When creating a chart, the IDE initializes many plot properties using default values
# from the PlotProperty class. Since we cannot access the Java class directly, here is
# a copy of those default values. Run PlotProperty.dump() to re-generate this list.
_plot_prop_defaults = {
    'Axes.Grid' : 'Major',
    'Axes.GridColor' : 'grey80',
    'Axis.Label.Font' : 'Arial-regular-8',
    'Axis.Title.Font' : 'Arial-regular-8',
    'Bar.Color' : 'grey80',
    'Bar.OutlineColor' : 'black',
    'Bar.Placement' : 'Aligned',
    'Bars.Baseline.Color' : 'grey80',
    'Bars.Baseline' : '0.0',
    'Hist.Bar' : 'Solid',
    'Hist.Color' : 'blue',
    'Hist.Cumulative' : 'false',
    'Hist.Density' : 'false',
    'Hist.ShowOverflowCell' : 'false',
    'Insets.BackgroundColor' : '#F6F5F4',
    'Insets.LineColor' : 'black',
    'Legend.Anchoring' : 'North',
    'Legend.Border' : 'false',
    'Legend.Display' : 'true',
    'Legend.Font' : 'Arial-regular-8',
    'Legend.Position' : 'Inside',
    'Line.Color' : 'blue',
    'Line.Display' : 'true',
    'Line.DrawStyle' : 'linear',
    'Line.Style' : 'solid',
    'Line.Width' : '1.5',
    'Plot.Antialias' : 'true',
    'Plot.BackgroundColor' : 'white',
    'Plot.Caching' : 'true',
    'Plot.Title.Color' : 'black',
    'Plot.Title.Font' : 'Arial-regular-10',
    'Plot.Title' : '',
    'Symbols.Size' : '4',
    'Symbols.Type' : 'Square',
    'X.Axis.Labels.Show' : 'true',
    'X.Axis.Log' : 'false',
    'X.Axis.Max' : 'Infinity',
    'X.Axis.Min' : '-Infinity',
    'X.Axis.Title.Show' : 'true',
    'X.Axis.Title' : '',
    'X.Label.RotateBy' : '0',
    'X.Label.Wrap' : 'true',
    'Y.Axis.Labels.Show' : 'true',
    'Y.Axis.Log' : 'false',
    'Y.Axis.Max' : 'Infinity',
    'Y.Axis.Min' : '-Infinity',
    'Y.Axis.Title.Show' : 'true',
    'Y.Axis.Title' : '',
}

def _collect_editable_properties(dialog_pages):
    """Helper for load_chart_template()"""
    props = {}
    for page in dialog_pages:
        root = ET.fromstring(page.content)
        for e in root.iter():
            id = e.attrib.get('{http://sweet_swt.sf.net/xswt}id')
            if id is not None:
                default = e.attrib.get('{http://sweet_swt.sf.net/xswt}id.default')
                if default is None and id in _plot_prop_defaults:
                    default = _plot_prop_defaults[id]
                if default is None:
                    default = ""
                props[id] = default
    return props

def _read_properties_file(filename):
    """Helper for load_chart_template()"""
    content = open(filename).read()
    content = content.replace("\\\n", "")  # join continued lines
    props = {}
    for line in content.split("\n"):
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        tup = line.split('=', 1)
        if len(tup) != 2:
            raise RuntimeError("Missing '=' in line '" + line + "'")
        key, val = tup
        key = key.strip()
        val = val.strip()
        if key in props:
            raise RuntimeError("Duplicate key " + key + " in line '" + line + "'")
        props[key] = val
    return props

