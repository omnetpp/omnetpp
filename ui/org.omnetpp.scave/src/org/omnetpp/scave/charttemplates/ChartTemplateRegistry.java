package org.omnetpp.scave.charttemplates;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;

public class ChartTemplateRegistry {

    private static final String CHARTTEMPLATES_FOLDER = "charttemplates";

    public static final String BARCHART_ID = "barchart_legacy";
    public static final String LINECHART_ID = "linechart_legacy";
    public static final String HISTOGRAMCHART_ID = "histogramchart_legacy";
    public static final String SCATTERCHART_ID = "scatterchart_legacy";

    private static ArrayList<ChartTemplate> templates = new ArrayList<ChartTemplate>();

    private static long lastTimeLoaded = 0;

    static {
        reloadTemplates();
    }

    public static void reloadTemplates() {
        templates.clear();

        try {
            // load from the plugin
            for (String propertiesFile : readFile("FILELIST", null).split("\n"))
                registerChartTemplate(propertiesFile, null);

            // load from projects
            for (IProject project : ResourcesPlugin.getWorkspace().getRoot().getProjects())
                if (project.getFolder(CHARTTEMPLATES_FOLDER).exists())
                    for (IResource member : project.getFolder(CHARTTEMPLATES_FOLDER).members())
                        if (member instanceof IFile && member.getName().endsWith(".properties"))
                            registerChartTemplate(member.getName(), project);
        }
        catch (IOException|CoreException e ) {
            ScavePlugin.logError("Error loading chart templates", e);
        }

        lastTimeLoaded = System.currentTimeMillis();
    }

    private static void registerChartTemplate(String propertiesFile, IProject fromProject) {
        try {
            ChartTemplate chartTemplate = loadChartTemplate(propertiesFile, fromProject);
            templates.add(chartTemplate);
        }
        catch (Exception e) {
            String location = fromProject == null ? "plugin " + ScavePlugin.PLUGIN_ID : "project " + fromProject.getName();
            ScavePlugin.logError("Cannot load chart template " + propertiesFile + " from " + location, e);
        }
    }

    private static ChartTemplate loadChartTemplate(String propertiesFile, IProject fromProject) throws IOException, CoreException {
        Properties props = new Properties();
        props.load(new StringReader(readFile(propertiesFile, fromProject)));

        String id = props.getProperty("id");
        String name = props.getProperty("name");
        String type = props.getProperty("type");
        String icon = props.getProperty("icon");
        String scriptFile = props.getProperty("scriptFile");

        int toolbarOrder = -1;

        String toolbarOrderProp = props.getProperty("toolbarOrder");
        if (toolbarOrderProp != null)
            toolbarOrder = Integer.parseInt(toolbarOrderProp);

        String toolbarIcon = props.getProperty("toolbarIcon");

        List<Chart.DialogPage> pages = new ArrayList<Chart.DialogPage>();

        for (int i = 0; true; ++i) {
            String pageId = props.getProperty("dialogPage." + i + ".label");

            if (pageId == null)
                break;

            String pageLabel = props.getProperty("dialogPage." + i + ".label");
            String xswtFile = props.getProperty("dialogPage." + i + ".xswtFile");

            Chart.DialogPage page = new DialogPage(pageId, pageLabel, readFile(xswtFile, fromProject));
            pages.add(page);
        }

        ChartTemplate template = new ChartTemplate(id, name, Chart.ChartType.valueOf(type), icon, readFile(scriptFile, fromProject), pages, toolbarOrder, toolbarIcon);

        String propertyNamesProp = props.getProperty("propertyNames");
        if (propertyNamesProp != null) {
            List<String> propertyNames = Arrays.asList(propertyNamesProp.split(","));
            template.setPropertyNames(propertyNames);
        }

        return template;
    }

    public static String readFile(String name, IProject fromProject) throws IOException, CoreException {
        InputStream stream;
        if (fromProject == null) {
            String path = "/" + CHARTTEMPLATES_FOLDER + "/" + name;
            stream = ChartTemplateRegistry.class.getResourceAsStream(path);
            if (stream == null)
                throw new IOException("Could not read resource file: " + path);
        }
        else {
            IFile file = fromProject.getFolder(CHARTTEMPLATES_FOLDER).getFile(name);
            stream = file.getContents();
            if (stream == null)
                throw new IOException("Could not read file: " + file.getFullPath());
        }

        String contents = FileUtils.readTextFile(stream, null);
        return contents.replace("\r\n", "\n");
    }

    public static ArrayList<ChartTemplate> getAllTemplates() {
        if (Debug.isDebugging() && (System.currentTimeMillis() > lastTimeLoaded + 1000))
            reloadTemplates();
        return templates;
    }

    public static ChartTemplate findTemplateByID(String id) {
        for (ChartTemplate templ : getAllTemplates())
            if (templ.getID().equals(id))
                return templ;
        return null;
    }

    public static ChartTemplate getTemplateByID(String id) {
        ChartTemplate templ = findTemplateByID(id);
        if (templ == null)
            throw new RuntimeException("No such chart template: " + id);
        return templ;
    }
}


