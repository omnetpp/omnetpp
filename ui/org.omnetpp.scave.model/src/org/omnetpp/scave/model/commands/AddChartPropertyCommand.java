package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.AnalysisObject;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;

public class AddChartPropertyCommand implements ICommand {

    private Chart chart;
    private Property property;

    public AddChartPropertyCommand(Chart chart, Property property) {
        this.chart = chart;
        this.property = property;
    }

    @Override
    public void execute() {
        chart.addProperty(property);
    }

    @Override
    public void undo() {
        chart.removeProperty(property);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Add chart property";
    }

    @Override
    public Collection<AnalysisObject> getAffectedObjects() {
        return Arrays.asList(chart, property);
    }
}
