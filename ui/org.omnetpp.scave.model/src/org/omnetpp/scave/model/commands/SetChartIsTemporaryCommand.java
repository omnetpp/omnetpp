package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

public class SetChartIsTemporaryCommand implements ICommand {

    private Chart chart;
    private boolean oldValue;
    private boolean newValue;

    public SetChartIsTemporaryCommand(Chart chart, boolean newValue) {
        this.chart = chart;
        this.newValue = newValue;
    }

    @Override
    public void execute() {
        oldValue = chart.isTemporary();
        chart.setTemporary(newValue);
    }

    @Override
    public void undo() {
        chart.setTemporary(oldValue);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart temporariness";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
