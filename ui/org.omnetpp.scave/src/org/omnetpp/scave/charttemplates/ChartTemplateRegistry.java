package org.omnetpp.scave.charttemplates;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

import org.omnetpp.common.Debug;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;

public class ChartTemplateRegistry {

    private static long lastTimeLoaded = 0;

    public static final String BARCHART_ID = "barchart_legacy";
    public static final String LINECHART_ID = "linechart_legacy";
    public static final String HISTOGRAMCHART_ID = "histogramchart_legacy";
    public static final String SCATTERCHART_ID = "scatterchart_legacy";

    private static ArrayList<ChartTemplate> templates = new ArrayList<ChartTemplate>();

    public static String readResourceFile(String name) throws IOException {
        InputStream stream = ChartTemplateRegistry.class.getResourceAsStream("/charttemplates/" + name);
        if (stream == null)
            throw new IOException("Could not read resource file: " + name);
        String template = FileUtils.readTextFile(stream, null);
        return template.replace("\r\n", "\n");
    }

    static {
        reloadTemplates();
    }

    public static void reloadTemplates() {
        templates.clear();

        try {
            for (String n : readResourceFile("FILELIST").split("\n"))
                addChartTemplate(n);
        } catch (IOException e) {
            ScavePlugin.logError("Error loading chart templates", e);
        }

        lastTimeLoaded = System.currentTimeMillis();
    }

    private static ChartTemplate addChartTemplate(ChartTemplate template) {
        templates.add(template);
        return template;
    }

    private static ChartTemplate addChartTemplate(String propertiesFile) {
        Properties props = new Properties();
        try {
            props.load(new StringReader(readResourceFile(propertiesFile)));

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

                Chart.DialogPage page = new DialogPage(pageId, pageLabel, readResourceFile(xswtFile));
                pages.add(page);
            }

            ChartTemplate template = addChartTemplate(new ChartTemplate(id, name, Chart.ChartType.valueOf(type), icon, readResourceFile(scriptFile), pages, toolbarOrder, toolbarIcon));

            String propertyNamesProp = props.getProperty("propertyNames");
            if (propertyNamesProp != null) {
                List<String> propertyNames = Arrays.asList(propertyNamesProp.split(","));
                template.setPropertyNames(propertyNames);
            }

            return template;
        }
        catch (Exception e) {
            ScavePlugin.logError("Cannot load chart template " + propertiesFile, e);
            return null;
        }
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


