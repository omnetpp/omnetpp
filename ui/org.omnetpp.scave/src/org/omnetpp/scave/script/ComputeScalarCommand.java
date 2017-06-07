package org.omnetpp.scave.script;

/**
 * Represents the "COMPUTE scalar" command in analysis scripts
 *
 * @author andras
 */
public class ComputeScalarCommand extends ScriptCommand {
    private String scalarName;
    private String valueExpr;
    private String moduleExpr;
    private String groupByExpr;
    private String filterExpression; // WHERE
    private boolean averageReplications;
    private boolean computeMinMax;
    private boolean computeStddev;
    private boolean computeConfidenceInterval;
    private double confidenceLevel;

    public String getScalarName() {
        return scalarName;
    }

    public void setScalarName(String scalarName) {
        this.scalarName = scalarName;
    }

    public String getValueExpr() {
        return valueExpr;
    }

    public void setValueExpr(String valueExpr) {
        this.valueExpr = valueExpr;
    }

    public String getModuleExpr() {
        return moduleExpr;
    }

    public void setModuleExpr(String moduleExpr) {
        this.moduleExpr = moduleExpr;
    }

    public String getGroupByExpr() {
        return groupByExpr;
    }

    public void setGroupByExpr(String groupByExpr) {
        this.groupByExpr = groupByExpr;
    }

    public String getFilterExpression() {
        return filterExpression;
    }

    public void setFilterExpression(String filterExpression) {
        this.filterExpression = filterExpression;
    }

    public boolean isAverageReplications() {
        return averageReplications;
    }

    public void setAverageReplications(boolean averageReplications) {
        this.averageReplications = averageReplications;
    }

    public boolean isComputeMinMax() {
        return computeMinMax;
    }

    public void setComputeMinMax(boolean computeMinMax) {
        this.computeMinMax = computeMinMax;
    }

    public boolean isComputeStddev() {
        return computeStddev;
    }

    public void setComputeStddev(boolean computeStddev) {
        this.computeStddev = computeStddev;
    }

    public boolean isComputeConfidenceInterval() {
        return computeConfidenceInterval;
    }

    public void setComputeConfidenceInterval(boolean computeConfidenceInterval) {
        this.computeConfidenceInterval = computeConfidenceInterval;
    }

    public double getConfidenceLevel() {
        return confidenceLevel;
    }

    public void setConfidenceLevel(double confidenceLevel) {
        this.confidenceLevel = confidenceLevel;
    }

    @Override
    public String toString() {
        return "ComputeScalarCommand [scalarName=" + scalarName + ", valueExpr=" + valueExpr + ", moduleExpr=" + moduleExpr + ", groupByExpr=" + groupByExpr
                + ", averageReplications=" + averageReplications + ", computeStddev=" + computeStddev + ", computeConfidenceInterval="
                + computeConfidenceInterval + ", confidenceLevel=" + confidenceLevel + ", computeMinMax=" + computeMinMax + "]";
    }

}
