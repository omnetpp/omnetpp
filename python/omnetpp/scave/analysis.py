"""
This module allows reading, writing, creating and editing
OMNeT++ Analysis (.anf) files, querying their contents, and
running the charts scripts they contain. The main user of this
module is `opp_charttool`.
"""

import os
import sys
import importlib
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom # because ET cannot pretty-print
from xml.parsers.expat import ExpatError
import matplotlib as mpl
import matplotlib.pyplot as plt
import omnetpp.scave.impl.results_standalone as results_module
import omnetpp.scave.impl.chart_charttool as chart_module
import omnetpp.scave.utils as utils

from ._version import __version__

_real_show = plt.show

def _fake_show(*args, **kwargs):
    global _show_called
    _real_show(*args, **kwargs)
    _show_called = True

plt.show = _fake_show

_next_id = 0

def _make_id(id_attr:str):
    global _next_id
    try:
        id = None if id_attr is None else int(id_attr)
    except:
        raise RuntimeError(f"Wrong Chart or Folder id '{id_attr}': IDs are expected to be numeric strings")

    if id is None:
        id = _next_id
        _next_id += 1
    else:
        if _next_id <= id:
            _next_id = id + 1
    return str(id)

class DialogPage:
    """
    Represents a dialog page in a `Chart`. Dialog pages have an ID, a label
    (which the IDE displays on the page's tab in the *Chart Properties* dialog),
    and XSWT content (which describes the UI controls on the page).
    """
    def __init__(self, id:str=None, label:str="", content:str=""):
        self.id = id
        self.label = label
        self.content = content

class Chart:
    """
    Represents a chart in an `Analysis`. Charts have an ID, a name, a chart script
    (a Python script that mainly uses Pandas and the `omnetpp.scave.*` modules),
    dialog pages (which make up the contents of the Chart Properties dialog in the IDE),
    and properties (which are what the *Chart Properties* dialog in the IDE edits).
    """
    def __init__(self, id:str=None, name:str="", type:str="MATPLOTLIB", template:str=None, icon:str=None, script:str="", dialog_pages=list(), properties=dict()):
        assert type in ["MATPLOTLIB", "BAR", "LINE", "HISTOGRAM"]
        self.id = _make_id(id)
        self.name = name
        self.type = type

        self.template = template
        self.icon = icon
        self.script = script
        self.dialog_pages = dialog_pages.copy()
        self.properties = properties.copy()

    def __repr__(self):
        return f"Chart(type='{self.type}',name='{self.name}',id={self.id})"

class Folder:
    """
    Represents a folder in an `Analysis`. Folders may contain charts and further folders.
    """
    def __init__(self, id:str=None, name:str="", items=list()):
        self.id = _make_id(id)
        self.name = name
        self.items = items

    def __iter__(self):
        return iter(self.items)

    def __len__(self):
        return len(self.items)

    def __repr__(self):
        return f"Folder(name='{self.name}', {len(self.items)} items)"

class Workspace:
    """
    This is an abstraction of an IDE workspace, and makes it possible to map
    workspace paths to filesystem paths. This is necessary because the inputs
    in the `Analysis` are workspace paths. The class tolerates if workspace
    metadata (the `.metadata` subdirectory) is missing; then it looks for projects
    in directories adjacent to other known projects.
    """
    def __init__(self, workspace_dir=None, project_locations=None):
        """
        Accepts the workspace location, plus a dict that contains the (absolute,
        or workspace-location-relative) location of projects by name. The latter is
        useful for projects that are NOT at the <workspace_dir>/<projectname> location.
        """
        if workspace_dir:
            if not os.path.isdir(workspace_dir):
                raise RuntimeError("Specified workspace directory doesn't exist, or is not a directory: " + workspace_dir)
            #if not os.path.isdir(workspace_dir + "/.metadata"):
            #   print("Warning: Directory does not look like a workspace (it has no .metadata subdirectory): " + workspace_dir)
            self.workspace_dir = os.path.abspath(workspace_dir)
        else:
            self.workspace_dir = None
        self.project_locations = project_locations or {}

    @staticmethod
    def find_workspace(dir=None):
        """
        Utility function: Find the IDE workspace directory searching from the
        given directory (or the current dir if not given) upwards. The workspace
        directory of the Eclipse-based IDE can be recognized by having a `.metadata`
        subdir. If the workspace is not found, None is returned.
        """
        if not dir:
            dir = os.getcwd()

        while True:
            if os.path.isdir(os.path.join(dir, ".metadata")):  # Eclipse metadata dir
                return dir
            parent_dir = os.path.dirname(dir)
            if parent_dir == dir:
                break
            dir = parent_dir
        return None

    @staticmethod
    def find_enclosing_project_location(file=None):
        """
        Utility function: Find the project directory searching from the
        given directory (or the current dir if not given) upwards. Project
        directories of the Eclipse-based IDE can be recognized by having a
        `.project` file in them.
        """
        dir = os.path.abspath(file) if file else os.getcwd()

        while True:
            if os.path.isfile(os.path.join(dir, ".project")):  # Eclipse project dir
                break
            parent_dir = os.path.dirname(dir)
            if parent_dir == dir:
                return None
            dir = parent_dir

        return dir

    def get_project_name(self, project_dir):
        """
        Returns the "real" name of the project from the `.project` (project
        description) file in the given project directory.
        """
        try:
            desc = os.path.join(project_dir, ".project")
            project_dom = minidom.parse(desc)
            names = [name.firstChild.nodeValue for name in project_dom.getElementsByTagName("name") if name.parentNode.tagName == "projectDescription"]
            return names[0] if names else None
        except Exception as e:
            raise RuntimeError("Missing or invalid .project file in " + project_dir) from e


    def find_enclosing_project(self, file=None):
        """
        Find the project name searching from the given directory (or the current
        dir if not given) upwards. Project directories of the Eclipse-based IDE
        can be recognized by having a `.project` file in them.
        """
        dir = Workspace.find_enclosing_project_location(file)
        if not dir:
            raise RuntimeError("Could not find enclosing project" + (f" for file '{file}'" if file else ""))

        name = self.get_project_name(dir)
        if name:
            self.project_locations[name] = dir
        return name

    def get_project_location(self, project_name):
        """
        Returns the location of the given workspace project in the filesystem path.
        """
        if project_name in self.project_locations:
            return self.project_locations[project_name]

        if self.workspace_dir and os.path.isdir(self.workspace_dir + "/.metadata"):
            # parse workspace metadata
            location_filename = self.workspace_dir + "/.metadata/.plugins/org.eclipse.core.resources/.projects/" + project_name + "/.location"
            if not os.path.isfile(location_filename):
                # Eclipse doesn't create a .location file if project's name equals its folder's name and it is at the default location
                location = os.path.join(self.workspace_dir, project_name)
            else:
                with open(location_filename, "rb") as locfile:
                    loc = locfile.read()
                    start = b"URI//file:"
                    locindex = loc.find(start) + len(start)
                    endindex = locindex + loc[locindex:].find(b"\0")
                    location = loc[locindex:endindex].decode("utf-8")
            if not os.path.isfile(location + "/.project"):
                raise RuntimeError(f"Cannot determine location for project '{project_name}' from Eclipse workspace data: stated location '{location}' is not valid")
        else:
            # no workspace metadata, look in other places
            workspace_candidates = [os.path.dirname(loc) for loc in self.project_locations.values()]
            if self.workspace_dir:
                workspace_candidates.insert(0, self.workspace_dir)
            workspace_candidates = list(set(workspace_candidates))  # filter out duplicates

            def get_project_name_or_none(location):
                try:
                    return self.get_project_name(location)
                except:
                    return None

            def find_project_in_any_of(workspace_candidates):
                for workspace_candidate in workspace_candidates:
                    for fname in os.listdir(workspace_candidate):
                        location = os.path.join(workspace_candidate, fname)
                        if get_project_name_or_none(location) == project_name:
                            return location
                return None

            location = find_project_in_any_of(workspace_candidates)
            if location is None or not os.path.isfile(location + "/.project"):
                raise RuntimeError(f"Cannot determine location for project '{project_name}': It is not next to any known project, and an Eclipse workspace is not available for more info")

        # cache and return result
        self.project_locations[project_name] = location
        return location

    def get_referenced_projects(self, project_name):
        """
        Returns a list of projects that are referenced by the given project.
        """
        project_file = self.get_project_location(project_name) + "/.project"
        try:
            project_dom = minidom.parse(project_file)
            return [ref.firstChild.nodeValue for ref in project_dom.getElementsByTagName("project")]
        except Exception as e:
            raise RuntimeError("Missing or invalid .project file: " + project_file) from e


    def get_all_referenced_projects(self, project_name, include_self=False):
        """
        Returns a list of projects that are referenced by the given project, even transitively.
        """
        result = [project_name] if include_self else []

        projs = self.get_referenced_projects(project_name)
        result += projs
        for proj in projs:
            result += self.get_all_referenced_projects(proj, False)

        return result

    def to_filesystem_path(self, wspath):
        """
        Translate workspace paths to filesystem path.
        """
        if wspath.startswith('/'):
            project_name,*rest = wspath[1:].split('/', 1)
            project_loc = self.get_project_location(project_name)
            if not os.path.isdir(project_loc):
                raise RuntimeError(f"Directory for project {project_name} doesn't exist (workspace dir not specified?): {project_loc}")
            return os.path.join(project_loc, *rest)
        else:
            return wspath # part is relative to current working directory

    def __repr__(self):
        return f"Workspace(workspace_dir='{self.workspace_dir}', project_locations={self.project_locations})"

class Analysis:
    """
    Represents an OMNeT++ Analysis, i.e. the contents of an  `anf` file. Methods
    allow reading/writing `anf` files, and running the charts in them for interactive
    display, image/data export or other side effects.
    """
    def __init__(self, inputs=list(), items=list()):
        self.inputs = inputs
        self.root_folder = Folder(items=items)

    @staticmethod
    def from_anf_file(anf_file_name):
        """
        Reads the given anf file and returns its content as an `Analysis` object.
        """
        analysis = ET.parse(anf_file_name).getroot()
        version = analysis.get('version')
        if version != "2":
            raise RuntimeError(f"Unsupported analysis file version: \"{version}\" (only \"2\" is supported).")

        def make_folder(folder_elem):
            items = list()
            for child_elem in folder_elem:
                if child_elem.tag == 'folder':
                    items.append(make_folder(child_elem))
                elif child_elem.tag == 'chart':
                    items.append(make_chart(child_elem))
                else:
                    pass
            return Folder(
                id = folder_elem.get('id'),
                name = folder_elem.get('name'),
                items = items)

        def make_chart(chart_elem):
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

        inputs = [input_elem.get('pattern') for input_elem in analysis.findall("inputs/input")]
        items = make_folder(analysis.find('charts')).items
        return Analysis(inputs, items)

    def collect_charts(self, folder=None):
        """
        Collects and returns a list of all charts in the specified folder, or
        in this Analysis if no folder is given.
        """
        if folder is None:
            folder = self.root_folder
        charts = list()
        for item in folder:
            if type(item) == Chart:
                charts.append(item)
            elif type(item) == Folder:
                charts += self.collect_charts(item)

        return charts

    def get_item_path(self, item):
        """
        Returns the path of the item (Chart or Folder) within the Analysis
        as list of path segments (Folder items). The returned list includes
        both the root folder of the Analysis and the item itself. If the
        item is not part of the Analysis, None is returned.
        """
        def find(item, folder):
            for child in folder:
                if child == item:
                    return [ folder, item ]
                elif type(child) == Folder:
                    path = find(item, child)
                    if path is not None:
                        return [ folder ] + path
            return None
        # Perform a search for the item in the Analysis. If this ever becomes
        # a performance bottleneck (doubtful), it can be fixed by introducing
        # a cache (child->parent dictionary) which is rebuilt once an
        # inconsistency is detected.
        return find(item, self.root_folder)

    def get_item_path_as_string(self, item, separator=" / "):
        """
        Returns the path of the item (Chart or Folder) within the Analysis as a
        string. Segments are joined with the given separator. The returned
        string includes the item name itself, but not the root folder (i.e. for
        items in the root folder, the path string equals to the item name). If
        the item is not part of the Analysis, None is returned.
        """
        path = self.get_item_path(item)
        if path is None:
            return None
        return " / ".join([ segment.name for segment in path[1:] ])

    def run_chart(self, chart, wd, workspace, extra_props=dict(), show=False):
        """
        Runs a chart script with the given working directory, workspace, and extra
        properties in addition to the chart's properties. If `show=True`, it calls
        `plt.show()` if it was not already called by the script.
        """
        assert(os.path.isabs(wd))

        # set up state in 'results' module
        fs_inputs = [ workspace.to_filesystem_path(i) for i in self.inputs ]
        results_module.set_inputs(fs_inputs)

        # set up state in 'chart' module
        chart_module.name = chart.name
        chart_module.chart_type = chart.type
        chart_module.properties.clear()
        chart_module.properties.update(chart.properties)
        chart_module.properties.update(extra_props)

        plt.clf()

        global _show_called
        _show_called = False

        od = os.getcwd()
        orig_sys_path = sys.path.copy()
        orig_rcparams = mpl.rcParams.copy()

        try:
            os.chdir(wd)
            sys.path.insert(1, wd)

            project = workspace.find_enclosing_project(wd)
            refprojs = workspace.get_all_referenced_projects(project, True)

            for rp in refprojs:
                sys.path.insert(1, workspace.get_project_location(rp) + "/python")

            exec(chart.script, { "exit": sys.exit })
            if show and not _show_called:
                plt.show()
        except SystemExit as se:
            if se.code != 0:
                raise RuntimeError("Chart script exited with code " + str(se.code))
        finally:
            os.chdir(od)
            sys.path = orig_sys_path
            mpl.rcParams.clear()
            mpl.rcParams.update(orig_rcparams)


    def export_image(self, chart, wd, workspace, format="svg", target_folder=None, filename=None, width=None, height=None, dpi=96, enforce=True, extra_props=dict()):
        """
        Runs a chart script for image export. This method just calls `run_chart()`
        with extra properties that instruct the chart script to perform image export.
        (It is assumed that the chart script invokes `utils.export_image_if_needed()`
        or implements equivalent functionality).
        """
        props = dict()
        props.update(chart.properties)
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

        # have to reload these modules, otherwise some charts are not entirely
        # reproducible, elements can be shifted around by about 1 pixel
        importlib.reload(mpl)
        importlib.reload(plt)
        mpl.use(utils._get_mpl_backend_for_image_format(str(format)))

        self.run_chart(chart, wd, workspace, extra_props=props, show=False)

        fname = utils.get_image_export_filepath(props)
        fname = os.path.join(wd, fname)
        return self._check_file_created(fname, "image", enforce)

    def export_data(self, chart, wd, workspace, format="csv", target_folder=None, filename=None, enforce=True, extra_props=dict()):
        """
        Runs a chart script for data export. This method just calls `run_chart()`
        with extra properties that instruct the chart script to perform data export.
        (It is assumed that the chart script invokes `utils.export_data_if_needed()`
        or implements equivalent functionality).
        """
        props = dict()
        props.update(extra_props)

        props['export_data'] = "true"
        if format:
            props['data_export_format'] = str(format)
        if target_folder:
            props['data_export_folder'] = str(target_folder)
        if filename:
            props['data_export_filename'] = str(filename)

        self.run_chart(chart, wd, workspace, extra_props=props, show=False)

        fname = utils.get_data_export_filepath(props)
        fname = os.path.join(wd, fname)
        return self._check_file_created(fname, "data", enforce)

    def _check_file_created(self, fname, what, enforce):
        if not os.path.isfile(fname):
            msg = f"Chart script silently failed to create {what} file '{fname}'"
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
        domTree = minidom.parseString("<analysis version='2'/>")
        analysisEl = domTree.documentElement

        # utility functions
        def appendChild(parent, tag):
            return parent.appendChild(domTree.createElement(tag))
        def setAttr(element, name, value):
            if value is not None:
                # working around https://bugs.python.org/issue5752 with the replace
                element.setAttribute(name, str(value).replace("\n", "💩"))
        def setContent(element, text):
            element.appendChild(domTree.createCDATASection(text))
            element.appendChild(domTree.createTextNode("\n"))

        def makeFolderEl(folder, tag='folder'):
            folderEl = domTree.createElement(tag)
            setAttr(folderEl, 'id', folder.id)
            setAttr(folderEl, 'name', folder.name)
            for child in folder.items:
                if type(child) == Chart:
                    folderEl.appendChild(makeChartEl(child))
                elif type(child) == Folder:
                    folderEl.appendChild(makeFolderEl(child))
                else:
                    pass
            return folderEl

        def makeChartEl(chart):
            chartEl = domTree.createElement('chart')
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
            return chartEl

        # build DOM tree
        inputsEl = appendChild(analysisEl,"inputs")
        for input in self.inputs:
            inputEl = appendChild(inputsEl, "input")
            setAttr(inputEl, "pattern", input)
        analysisEl.appendChild(makeFolderEl(self.root_folder, 'charts'))

        # write to file
        with open(filename, 'wt') as f:
            xml = domTree.toprettyxml(indent="    ", newl="\n")
            f.write(xml.replace("💩", "&#10;"))

def load_anf_file(anf_file_name):
    """
    Reads the given anf file and returns its content as an `Analysis` object.
    This is synonym for `Analysis.from_anf_file()`.
    """
    return Analysis.from_anf_file(anf_file_name)
