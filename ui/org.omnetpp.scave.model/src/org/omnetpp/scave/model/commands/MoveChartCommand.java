package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.ModelObject;

public class MoveChartCommand implements ICommand { // TODO AnalysisItem ?

    Charts charts;
    AnalysisItem chart;
    int oldIndex;
    int newIndex;

    public MoveChartCommand(Charts charts, AnalysisItem chart, int newIndex) {
        this.charts = charts;
        this.chart = chart;
        this.newIndex = newIndex;

        Assert.isTrue(oldIndex >= 0);
    }

    @Override
    public void execute() {
        this.oldIndex = charts.getCharts().indexOf(chart);

        List<AnalysisItem> items = new ArrayList<AnalysisItem>(charts.getCharts());

        items.remove(oldIndex);
        items.add(newIndex, chart);

        charts.setCharts(items);
    }

    @Override
    public void undo() {
        List<AnalysisItem> items = new ArrayList<AnalysisItem>(charts.getCharts());

        items.remove(newIndex);
        items.add(oldIndex, chart);

        charts.setCharts(items);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Move charts";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(charts, chart);
    }

}
