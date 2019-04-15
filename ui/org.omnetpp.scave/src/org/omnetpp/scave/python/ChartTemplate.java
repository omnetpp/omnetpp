package org.omnetpp.scave.python;

public class ChartTemplate {

    public enum ChartType {
        BAR,
        HISTOGRAM,
        LINE,
        MATPLOTLIB,
        SCATTER
    }

    private String ID;
    private String name;
    private ChartType chartType;
    private String pythonScript;
    private String xswtForm;

    public ChartTemplate(String ID, String name, ChartType chartType, String pythonScript, String xswtForm) {
        this.ID = ID;
        this.name = name;
        this.chartType = chartType;
        this.pythonScript = pythonScript;
        this.xswtForm = xswtForm;
    }

    public String getID() {
        return ID;
    }

    public void setID(String iD) {
        ID = iD;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public ChartType getChartType() {
        return chartType;
    }

    public void setChartType(ChartType chartType) {
        this.chartType = chartType;
    }

    public String getPythonScript() {
        return pythonScript;
    }

    public void setPythonScript(String pythonScript) {
        this.pythonScript = pythonScript;
    }

    public String getXswtForm() {
        return xswtForm;
    }

    public void setXswtForm(String xswtForm) {
        this.xswtForm = xswtForm;
    }
}
