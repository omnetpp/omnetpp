package org.omnetpp.scave.charttemplates;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.ChartType;
import org.omnetpp.scave.model.Chart.DialogPage;

public class ChartTemplate {

    private String id;
    private String name;
    private String description;
    private ChartType chartType;
    private String iconPath;
    private String pythonScript;
    private List<DialogPage> dialogPages;
    private int toolbarOrder = -1;
    private String toolbarIconPath;
    private List<String> propertyNames = new ArrayList<String>();
    private int supportedResultTypes; // a bitwise OR of the constants in ResultFileManager

    public ChartTemplate(String id, String name, String description, ChartType chartType, String iconPath, String pythonScript, List<DialogPage> dialogPages, int toolbarOrder, String toolbarIconPath) {
        this.id = id;
        this.name = name;
        this.description = description;
        this.chartType = chartType;
        this.iconPath = iconPath;
        this.pythonScript = pythonScript;
        this.dialogPages = dialogPages;
        this.toolbarOrder = toolbarOrder;
        this.toolbarIconPath = toolbarIconPath;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
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

    public int getSupportedResultTypes() {
        return supportedResultTypes;
    }

    public void setSupportedResultTypes(int supportedResultTypes) {
        this.supportedResultTypes = supportedResultTypes;
    }

    @Override
    public String toString() {
        return "'" + getName() + "' (id=" + getId() + ")";
    }

}
