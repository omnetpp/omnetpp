package org.omnetpp.scave.charttemplates;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.Chart.DialogPage;

public class ChartTemplate {

    private String ID;
    private String name;
    private ChartType chartType;
    private String iconPath;
    private String pythonScript;
    private List<DialogPage> dialogPages;
    private int toolbarOrder = -1;
    private String toolbarIconPath;
    private List<String> propertyNames = new ArrayList<String>();

    public ChartTemplate(String ID, String name, ChartType chartType, String iconPath, String pythonScript, List<DialogPage> dialogPages, int toolbarOrder, String toolbarIconPath) {
        this.ID = ID;
        this.name = name;
        this.chartType = chartType;
        this.iconPath = iconPath;
        this.pythonScript = pythonScript;
        this.dialogPages = dialogPages;
        this.toolbarOrder = toolbarOrder;
        this.toolbarIconPath = toolbarIconPath;
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

    public Chart.ChartType getChartType() {
        return chartType;
    }

    public void setChartType(Chart.ChartType chartType) {
        this.chartType = chartType;
    }

    public String getIconPath() {
        return iconPath;
    }

    public void setIconPath(String iconPath) {
        this.iconPath = iconPath;
    }

    public String getPythonScript() {
        return pythonScript;
    }

    public void setPythonScript(String pythonScript) {
        this.pythonScript = pythonScript;
    }

    public List<DialogPage> getDialogPages() {
        return Collections.unmodifiableList(dialogPages);
    }

    public void setDialogPages(List<DialogPage> dialogPages) {
        this.dialogPages = dialogPages;
    }

    public int getToolbarOrder() {
        return toolbarOrder;
    }

    public void setToolbarOrder(int toolbarOrder) {
        this.toolbarOrder = toolbarOrder;
    }

    public String getToolbarIconPath() {
        return toolbarIconPath;
    }

    public void setToolbarIconPath(String toolbarIconPath) {
        this.toolbarIconPath = toolbarIconPath;
    }

    public List<String> getPropertyNames() {
        return Collections.unmodifiableList(propertyNames);
    }

    public void setPropertyNames(List<String> propertyNames) {
        this.propertyNames = propertyNames;
    }

}
