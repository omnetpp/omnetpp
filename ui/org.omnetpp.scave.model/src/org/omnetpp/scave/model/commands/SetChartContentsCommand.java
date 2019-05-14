package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

public class SetChartContentsCommand implements ICommand {

    private Chart chart;
    private Chart oldContents;
    private Chart newContents;

    public SetChartContentsCommand(Chart chart, Chart oldContents, Chart newContents) {
        this.chart = chart;
        this.oldContents = oldContents;
        this.newContents = newContents;

        Assert.isTrue(chart.equals(oldContents) || chart.equals(newContents));
        Assert.isTrue(chart != oldContents && chart != newContents);
    }

    @Override
    public void execute() {
        chart.copyFrom(newContents);
    }

    @Override
    public void undo() {
        chart.copyFrom(oldContents);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Edit chart";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

    public boolean isEmpty() {
        return oldContents.equals(newContents);
    }
}
