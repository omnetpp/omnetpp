/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charttemplates;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import org.apache.commons.lang3.EnumUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.omnetpp.common.Debug;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.Markers;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.ScaveUtil;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.ChartTemplate;
import org.osgi.framework.Bundle;

/**
 * Loads and maintains the list of available chart templates.
 */
public class ChartTemplateRegistry {
    private static final String CHARTTEMPLATES_FOLDER = "charttemplates";

    private static boolean debug = false;
    private static boolean sortByLastUsageTimestamps = false;

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

    public void reloadBuiltinTemplates() {
        Debug.time("Reloading builtin chart templates", 1, () -> {
            templates.removeIf(x -> x.isBuiltin());

            try {
                // load from the plugin
                String pluginTemplatesFolder = getTemplatesFolder(null);
                for (String propertiesFile : readFile(pluginTemplatesFolder, "FILELIST").split("\n"))
                    registerChartTemplate(pluginTemplatesFolder, propertiesFile, true);
            }
            catch (IOException|CoreException e) {
                ScavePlugin.logError("Error loading builtin chart templates", e);
            }

            lastTimeLoaded = System.currentTimeMillis();
            // custom ones should come first, and Collections.sort is guaranteed to be stable
            Collections.sort(templates, (a, b) -> Boolean.compare(a.isBuiltin(), b.isBuiltin()));
        });
    }

    public void reloadUserTemplates() {
        Debug.time("Reloading custom chart templates", 1, () -> {
            templates.removeIf(x -> !x.isBuiltin());

            try {
                // load from projects
                IProject[] projectsToScan = ProjectUtils.getAllReferencedProjects(project, true, true);
                for (IProject project : projectsToScan)
                    if (project.getFolder(CHARTTEMPLATES_FOLDER).exists())
                        for (IResource member : project.getFolder(CHARTTEMPLATES_FOLDER).members())
                            if (member instanceof IFile && member.getName().endsWith(".properties"))
                                registerChartTemplate(getTemplatesFolder(project), member.getName(), false);
            }
            catch (CoreException e) {
                ScavePlugin.logError("Error loading custom chart templates", e);
            }

            lastTimeLoaded = System.currentTimeMillis();
            // custom ones should come first, and Collections.sort is guaranteed to be stable
            Collections.sort(templates, (a, b) -> Boolean.compare(a.isBuiltin(), b.isBuiltin()));
        });
    }

    public void reloadAllTemplates() {
        reloadBuiltinTemplates();
        reloadUserTemplates();
    }

    private void registerChartTemplate(String templatesFolder, String propertiesFile, boolean builtin) {
        IContainer workspaceFolder = getWorkspaceFolder(templatesFolder);
        try {
            if (workspaceFolder != null)
                workspaceFolder.getFile(new Path(propertiesFile)).deleteMarkers(Markers.CHARTTEMPLATE_PROBLEMMARKER_ID, true, IResource.DEPTH_ZERO);
            ChartTemplate chartTemplate = loadChartTemplate(templatesFolder, propertiesFile, builtin);
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

    private ChartTemplate loadChartTemplate(String templatesFolder, String propertiesFile, boolean builtin) throws IOException, CoreException {
        Properties props = new Properties();
        props.load(new StringReader(readFile(templatesFolder, propertiesFile)));

        // base properties
        String id = props.getProperty("id");
        String name = props.getProperty("name");
        String description = props.getProperty("description", "");
        String descriptionFile = props.getProperty("descriptionFile", "");
        String type = props.getProperty("type");
        String icon = props.getProperty("icon");
        String scriptFile = props.getProperty("scriptFile");
        String menuIcon = props.getProperty("menuIcon");
        int score = Integer.parseInt(props.getProperty("score", "0"));
        String originFolder = new Path(templatesFolder + "/" + propertiesFile).removeLastSegments(1).toString();

        // dialog pages
        List<Chart.DialogPage> pages = new ArrayList<Chart.DialogPage>();
        for (int i = 0; true; ++i) {
            String pageId = props.getProperty("dialogPage." + i + ".id");

            if (pageId == null)
                break;

            String pageLabel = props.getProperty("dialogPage." + i + ".label", "");
            String xswtFile = props.getProperty("dialogPage." + i + ".xswtFile", pageId + ".xswt");

            String xswtContent = readFile(templatesFolder, xswtFile);
            Chart.DialogPage page = new DialogPage(pageId, pageLabel, xswtContent);
            pages.add(page);
        }

        // chart type
        ChartType chartType = EnumUtils.getEnum(Chart.ChartType.class, type);
        if (chartType == null)
            throw new RuntimeException("Unrecognized chart type '" + type + "', use one of: " + StringUtils.join(Chart.ChartType.values(), ","));

        // description
        if (!StringUtils.isEmpty(description) && !StringUtils.isEmpty(descriptionFile))
            throw new RuntimeException("Both description and descriptionFile given, please decide which one you want");
        if (!StringUtils.isEmpty(descriptionFile))
            description = readFile(templatesFolder, descriptionFile);

        // chart script
        String script = scriptFile != null ? readFile(templatesFolder, scriptFile) : "";

        // properties and their defaults are taken from the pages (x:id and x:id.default)
        Map<String,String> properties = collectEditableProperties(pages);

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

        ChartTemplate template = new ChartTemplate(id, name, description, chartType, icon, resultTypes, script, pages, score, menuIcon, properties, originFolder.toString(), builtin);

        return template;
    }

    protected Map<String,String> collectEditableProperties(List<DialogPage> pages) {
        Map<String,String> properties = new HashMap<>();
        for (DialogPage page : pages) {
            try {
                Map<String, String> pageProperties = ScaveUtil.collectEditablePropertiesFromXswt(page.xswtForm);
                for (String name : pageProperties.keySet())
                    if (properties.containsKey(name))
                        throw new RuntimeException("Property '" + name + "'already occurs on another page");
                properties.putAll(pageProperties);
            }
            catch (Exception e) {
                throw new RuntimeException("Page '" + page.label + "': " + e.getMessage(), e);
            }
        }
        return properties;
    }

    protected String getTemplatesFolder(IProject project) {
        return project == null ? "plugin:" + CHARTTEMPLATES_FOLDER : project.getName() + "/" + CHARTTEMPLATES_FOLDER;
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
                int result = 0;

                if (sortByLastUsageTimestamps) {
                    long ta = templatesLastUsageTimestamps.getOrDefault(a.getId(), 0l);
                    long tb = templatesLastUsageTimestamps.getOrDefault(b.getId(), 0l);
                    result = -Long.compare(ta, tb); // decreasing order of last usage timestamp
                }

                if (result == 0) // most likely neither of them have been used yet, sort decreasing by score
                    result = -Long.compare(a.getScore(), b.getScore());

                return result;
            }
        });
        return result;
    }

    public String readFile(String folder, String name) throws IOException, CoreException {
        String path = folder + "/" + name;
        InputStream stream;
        if (path.startsWith("plugin:")) {
            Bundle bundle = Platform.getBundle("org.omnetpp.scave.templates");
            URL resource = bundle.getResource(path.substring(7));
            stream = (resource == null) ? null : resource.openStream();
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
            reloadAllTemplates();
        }
        if (System.currentTimeMillis() > lastTimeLoaded + 1000) {
            if (Debug.isDebugging())
                reloadAllTemplates();
            else
                reloadUserTemplates();
        }
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


