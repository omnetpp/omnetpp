package org.omnetpp.scave.charttemplates;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.lang3.EnumUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.Debug;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.XmlUtils;
import org.omnetpp.scave.Markers;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.ChartTemplate;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/**
 * Loads and maintains the list of available chart templates.
 */
public class ChartTemplateRegistry {
    private static final String CHARTTEMPLATES_FOLDER = "charttemplates";

    private static boolean debug = false;

    private IProject project = null;
    private ArrayList<ChartTemplate> templates = null;

    private long lastTimeLoaded = 0;

    private Map<String, Long> templatesLastUsageTimestamps = new HashMap<String, Long>();

    public void setProject(IProject project) {
        if (project != this.project) {
            this.project = project;
            templates = null; // invalidate
        }
    }

    public void reloadTemplates() {
        templates = new ArrayList<>();

        try {
            // load from the plugin
            String pluginTemplatesFolder = getTemplatesFolder(null);
            for (String propertiesFile : readFile(pluginTemplatesFolder, "FILELIST").split("\n"))
                registerChartTemplate(pluginTemplatesFolder, propertiesFile);

            // load from projects
            IProject[] projectsToScan = ProjectUtils.getAllReferencedProjects(project, true, true);
            for (IProject project : projectsToScan)
                if (project.getFolder(CHARTTEMPLATES_FOLDER).exists())
                    for (IResource member : project.getFolder(CHARTTEMPLATES_FOLDER).members())
                        if (member instanceof IFile && member.getName().endsWith(".properties"))
                            registerChartTemplate(getTemplatesFolder(project), member.getName());
        }
        catch (IOException|CoreException e ) {
            ScavePlugin.logError("Error loading chart templates", e);
        }

        lastTimeLoaded = System.currentTimeMillis();
    }

    private void registerChartTemplate(String templatesFolder, String propertiesFile) {
        IContainer workspaceFolder = getWorkspaceFolder(templatesFolder);
        try {
            if (workspaceFolder != null)
                workspaceFolder.getFile(new Path(propertiesFile)).deleteMarkers(Markers.CHARTTEMPLATE_PROBLEMMARKER_ID, true, IResource.DEPTH_ZERO);
            ChartTemplate chartTemplate = loadChartTemplate(templatesFolder, propertiesFile);
            if (doFindTemplateByID(templates, chartTemplate.getId()) != null)
                throw new RuntimeException("template ID is not unique");
            templates.add(chartTemplate);
            if (debug)
                Debug.println("Loaded chart template " + chartTemplate);
        }
        catch (Exception e) {
            ScavePlugin.logError("Cannot load chart template " + propertiesFile + " from " + templatesFolder, e);
            if (workspaceFolder != null)
                addErrorMarker(workspaceFolder.getFile(new Path(propertiesFile)), e.getMessage());
        }
    }

    private ChartTemplate loadChartTemplate(String templatesFolder, String propertiesFile) throws IOException, CoreException {
        Properties props = new Properties();
        props.load(new StringReader(readFile(templatesFolder, propertiesFile)));

        // base properties
        String id = props.getProperty("id");
        String name = props.getProperty("name");
        String description = props.getProperty("description");
        String type = props.getProperty("type");
        String icon = props.getProperty("icon");
        String scriptFile = props.getProperty("scriptFile");
        String toolbarIcon = props.getProperty("toolbarIcon");
        int toolbarOrder = Integer.parseInt(props.getProperty("toolbarOrder", "-1"));
        String originFolder = new Path(templatesFolder + "/" + propertiesFile).removeLastSegments(1).toString();

        HashMap<String,String> properties = new HashMap<>();

        // dialog pages
        List<Chart.DialogPage> pages = new ArrayList<Chart.DialogPage>();
        for (int i = 0; true; ++i) {
            String pageId = props.getProperty("dialogPage." + i + ".label");

            if (pageId == null)
                break;

            String pageLabel = props.getProperty("dialogPage." + i + ".label", "");
            String xswtFile = props.getProperty("dialogPage." + i + ".xswtFile", pageId + ".xswt");

            String xswtContent = readFile(templatesFolder, xswtFile);
            Chart.DialogPage page = new DialogPage(pageId, pageLabel, xswtContent);
            pages.add(page);

            collectPropertiesFromXswt(xswtFile, xswtContent, properties);
        }

        // chart type
        ChartType chartType = EnumUtils.getEnum(Chart.ChartType.class, type);
        if (chartType == null)
            throw new RuntimeException("Unrecognized chart type '" + type + "', use one of: " + StringUtils.join(Chart.ChartType.values(), ","));

        // chart script
        String script = scriptFile != null ? readFile(templatesFolder, scriptFile) : "";

        String propertyNamesProp = props.getProperty("propertyNames", "");
        if (!StringUtils.isBlank(propertyNamesProp))
            Debug.println("WARNING: " + propertiesFile + " contains obsolete property 'propertyNames', please use 'x:id.default' attributes in XSWT pages to declare default values");

        // parse result types
        int resultTypes = 0;
        String resultTypesProp = props.getProperty("resultTypes");
        if (resultTypesProp != null) {
            for (String resultType : resultTypesProp.split(",")) {
                switch (resultType) {
                    case "parameter":  resultTypes |= ResultFileManager.PARAMETER;  break;
                    case "scalar":     resultTypes |= ResultFileManager.SCALAR;     break;
                    case "vector":     resultTypes |= ResultFileManager.VECTOR;     break;
                    case "statistics": resultTypes |= ResultFileManager.STATISTICS; break;
                    case "histogram":  resultTypes |= ResultFileManager.HISTOGRAM;  break;
                }
            }
        }

        ChartTemplate template = new ChartTemplate(id, name, description, chartType, icon, resultTypes, script, pages, toolbarOrder, toolbarIcon, properties, originFolder.toString());

        return template;
    }

    protected void collectPropertiesFromXswt(String xswtFile, String xswtContent, Map<String, String> properties) {
        try {
            DocumentBuilder db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            Document d = db.parse(new ByteArrayInputStream(xswtContent.getBytes()));
            ArrayList<Element> controls = XmlUtils.collectElements(d.getDocumentElement(), (e) -> (e.getAttributeNode("x:id") != null));
            for (Element e : controls) {
                String name = e.getAttribute("x:id");
                if (properties.containsKey(name))
                    Debug.print("WARNING: Duplicate property in XSWT pages: " + name);
                Attr valueNode = e.getAttributeNode("x:id.default");
                String value = "";
                if (valueNode != null)
                    value = valueNode.getValue();
                else {
                    PlotProperty prop = PlotProperty.loookup(name);
                    if (prop != null)
                        value = prop.getDefaultValueAsString();
                }
                properties.put(name, value);
            }
        }
        catch (Exception e) {
            ScavePlugin.logError("Error collecting properties from XSWT page " + xswtFile, e);
        }
    }

    protected String getTemplatesFolder(IProject project) {
        return project == null ? "plugin:/" + CHARTTEMPLATES_FOLDER : project.getName() + "/" + CHARTTEMPLATES_FOLDER;
    }

    protected IContainer getWorkspaceFolder(String folder) {
        return folder.startsWith("plugin:") ? null : ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(folder));
    }

    public List<ChartTemplate> getChartTemplatesForResultTypes(int resultTypes) {
        List<ChartTemplate> result = new ArrayList<ChartTemplate>();

        for (ChartTemplate templ : getAllTemplates())
            if ((resultTypes & templ.getSupportedResultTypes()) == resultTypes)
                result.add(templ);

        result.sort(new Comparator<ChartTemplate>() {
            @Override
            public int compare(ChartTemplate a, ChartTemplate b) {
                long ta = templatesLastUsageTimestamps.getOrDefault(a.getId(), 0l);
                long tb = templatesLastUsageTimestamps.getOrDefault(b.getId(), 0l);
                int result = -Long.compare(ta, tb); // decreasing order of last usage timestamp

                if (result == 0) { // most likely neither of them have been used yet, sort by toolbar order
                    int oa = a.getToolbarOrder();
                    int ob = b.getToolbarOrder();
                    if (oa != -1 && ob != -1)
                        result = Long.compare(oa, ob);
                    else if (oa == -1 && ob != -1) // ob is on the toolbar, oa isn't
                        result = Long.compare(ob+1, ob);
                    else if (oa != -1 && ob == -1) // oa is on the toolbar, ob isn't
                        result = Long.compare(oa, oa+1);
                }

                return result;
            }
        });
        return result;
    }

    public String readFile(String folder, String name) throws IOException, CoreException {
        String path = folder + "/" + name;
        InputStream stream;
        if (path.startsWith("plugin:")) {
            stream = ScavePlugin.getDefault().openStream(new Path(path.substring(7)));
            if (stream == null)
                throw new IOException("Could not read resource file: " + path);
        }
        else {
            IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
            stream = file.getContents();
            if (stream == null) {
                addErrorMarker(file, "Could not read file: " + file.getFullPath());
                return "";
            }
        }

        String contents = FileUtils.readTextFile(stream, null);
        return contents.replace("\r\n", "\n");
    }

    private void addErrorMarker(IFile file, String message) {
        try {
            IMarker marker = file.createMarker(Markers.CHARTTEMPLATE_PROBLEMMARKER_ID);
            marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
            marker.setAttribute(IMarker.LINE_NUMBER, 1);
            marker.setAttribute(IMarker.MESSAGE, message);
        }
        catch (CoreException e) {
            ScavePlugin.logError("Cannot create error marker", e);
        }
    }

    public ArrayList<ChartTemplate> getAllTemplates() {
        if (templates == null) {
            templates = new ArrayList<ChartTemplate>();
            reloadTemplates();
        }
        if (Debug.isDebugging() && (System.currentTimeMillis() > lastTimeLoaded + 1000))
            reloadTemplates();
        return templates;
    }

    private ChartTemplate doFindTemplateByID(List<ChartTemplate> templates, String id) {
        for (ChartTemplate templ : templates)
            if (templ.getId().equals(id))
                return templ;
        return null;
    }

    public ChartTemplate findTemplateByID(String id) {
        return doFindTemplateByID(getAllTemplates(), id);
    }

    public ChartTemplate getTemplateByID(String id) {
        ChartTemplate templ = findTemplateByID(id);
        if (templ == null)
            throw new RuntimeException("No such chart template: " + id);
        return templ;
    }

    public void markTemplateUsage(ChartTemplate t) {
        templatesLastUsageTimestamps.put(t.getId(), System.currentTimeMillis());
    }

    public String storeTimestamps() {
        String result = "";
        for (String id : templatesLastUsageTimestamps.keySet()) {
            if (!result.isEmpty())
                result += ",";
            result += id + "=" + templatesLastUsageTimestamps.get(id);
        }
        return result;
    }

    public void restoreTimestamps(String pref) {
        templatesLastUsageTimestamps.clear();
        for (String i : pref.split(",")) {
            String[] kv = i.split("=", 2);
            if (kv.length != 2)
                continue;
            try {
                templatesLastUsageTimestamps.put(kv[0], Long.parseLong(kv[1]));
            } catch (NumberFormatException e) {
                // ignore
            }
        }
    }
}


