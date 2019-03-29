package org.omnetpp.scave.python;

import java.io.IOException;
import java.util.ArrayList;

import org.omnetpp.common.util.FileUtils;
import org.omnetpp.scave.python.ChartTemplate.ChartType;

public class ChartTemplateRegistry {
    private static ArrayList<ChartTemplate> templates = new ArrayList<ChartTemplate>();

    public static String readResourceFile(String name) {
        try {
            String template = FileUtils.readTextFile(ChartTemplateRegistry.class.getResourceAsStream(name), null);
            return template.replace("\r\n", "\n");
        } catch (IOException e) {
            return "";
        }
    }

    public static final ChartTemplate lineChart = addChartTemplate("linechart", "Line Chart", ChartType.LINE, "chart_templ_linechart.py", "chart_templ_linechart.xswt");
    public static final ChartTemplate lineChartInternal = addChartTemplate("linechart_internal", "Line Chart (internal)", ChartType.LINE, "chart_templ_linechart_internal.py", "chart_templ_linechart_internal.xswt");
    public static final ChartTemplate lineChartMpl = addChartTemplate("linechart_mpl", "Line Chart with MatPlotLib", ChartType.MATPLOTLIB, "chart_templ_linechart_mpl.py", "chart_templ_linechart_mpl.xswt");
    public static final ChartTemplate barChart = addChartTemplate("barchart", "Bar Chart", ChartType.BAR, "chart_templ_barchart.py", "chart_templ_barchart.xswt");
    public static final ChartTemplate barChartInternal = addChartTemplate("barchart_internal", "Bar Chart (internal)", ChartType.BAR, "chart_templ_barchart_internal.py", "chart_templ_barchart_internal.xswt"); // TODO: rename as _legacy ?
    public static final ChartTemplate barChartMpl = addChartTemplate("barchart_mpl", "Bar Chart with MatPlotLib", ChartType.MATPLOTLIB, "chart_templ_barchart_mpl.py", "chart_templ_barchart_mpl.xswt");
    public static final ChartTemplate histogramChart = addChartTemplate("histogramchart", "Histogram Chart", ChartType.HISTOGRAM, "chart_templ_histogram.py", "chart_templ_histogram.xswt");
    public static final ChartTemplate histogramChartMpl = addChartTemplate("histogramchart_mpl", "Histogram Chart", ChartType.MATPLOTLIB, "chart_templ_histogram_mpl.py", "chart_templ_histogram_mpl.xswt");
    public static final ChartTemplate boxWhiskersChartMpl = addChartTemplate("boxwhiskers_mpl", "Box and Whiskers Chart with MatPlotLib", ChartType.MATPLOTLIB, "chart_templ_boxwhiskers.py", "chart_templ_boxwhiskers.xswt");
    public static final ChartTemplate scatterChart = addChartTemplate("scatterchart", "Scatter Chart", ChartType.MATPLOTLIB, "chart_templ_scatterchart.py", "chart_templ_scatterchart.xswt");
    public static final ChartTemplate scatterChartInternal = addChartTemplate("scatterchart", "Scatter Chart (internal)", ChartType.SCATTER, "chart_templ_scatterchart_internal.py", "chart_templ_scatterchart_internal.xswt");
    public static final ChartTemplate threeDChart = addChartTemplate("3d", "3D Surface Chart", ChartType.MATPLOTLIB, "chart_templ_3d.py", "chart_templ_3d.xswt");

    private static ChartTemplate addChartTemplate(ChartTemplate template) {
        templates.add(template);
        return template;
    }

    private static ChartTemplate addChartTemplate(String ID, String name, ChartType type, String scriptResourceFile, String formResourceFile) {
        return addChartTemplate(new ChartTemplate(ID, name, type, readResourceFile(scriptResourceFile), readResourceFile(formResourceFile)));
    }

    public static ArrayList<ChartTemplate> getAllTemplates() {
        return templates;
    }

    public static ChartTemplate findTemplateByID(String id) {
        for (ChartTemplate templ : templates)
            if (templ.getID().equals(id))
                return templ;
        return null;
    }
}


