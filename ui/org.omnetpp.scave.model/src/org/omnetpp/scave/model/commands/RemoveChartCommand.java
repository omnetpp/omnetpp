package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.AnalysisObject;
import org.omnetpp.scave.model.Charts;

public class RemoveChartCommand implements ICommand {

    private int index;
    private AnalysisObject parent;
    private AnalysisItem chart;

    public RemoveChartCommand(AnalysisItem chart) {
        this.chart = chart;
        this.parent = chart.getParent();
    }

    @Override
    public void execute() {
        AnalysisObject parent = chart.getParent();

        if (parent instanceof Charts) {
            index = ((Charts)parent).getCharts().indexOf(chart);
        }

        if (parent instanceof Charts)
            ((Charts)parent).removeChart(chart);
        else {
            // TODO
        }
    }

    @Override
    public void undo() {
        if (parent instanceof Charts)
            ((Charts)parent).addChart(chart, index);

    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Remove chart";
    }

    @Override
    public Collection<AnalysisObject> getAffectedObjects() {
        return Arrays.asList(parent, chart);
    }

}
