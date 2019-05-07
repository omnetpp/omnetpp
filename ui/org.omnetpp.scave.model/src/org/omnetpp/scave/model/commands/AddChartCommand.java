package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.AnalysisObject;

public class AddChartCommand implements ICommand {

    private Analysis analysis;
    private AnalysisItem chart;
    private int index;

    public AddChartCommand(Analysis analysis, AnalysisItem chart) {
        this(analysis, chart, analysis.getCharts().getCharts().size());
    }

    public AddChartCommand(Analysis analysis, AnalysisItem chart, int index) {
        this.analysis = analysis;
        this.chart = chart;
        this.index = index;
    }

    @Override
    public void execute() {
        analysis.getCharts().addChart(chart, index);
    }

    @Override
    public void undo() {
        analysis.getCharts().removeChart(chart);

    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Add chart";
    }

    @Override
    public Collection<AnalysisObject> getAffectedObjects() {
        return Arrays.asList(analysis.getCharts(), chart);
    }

}
