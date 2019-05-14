package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;

public class SetChartPropertyCommand implements ICommand {

    private Chart chart;
    String propertyName;
    private String oldValue;
    private String newValue;

    public SetChartPropertyCommand(Chart chart, String propertyName, String newValue) {
        this.chart = chart;
        this.propertyName = propertyName;
        this.newValue = newValue;
    }

    private static void setPropertyValue(Chart chart, String name, String toValue) {
        Property property = chart.lookupProperty(name);
        String fromValue = property == null ? null : property.getValue();

        if (fromValue == null && toValue == null)
            return;
        else if (fromValue != null && toValue != null)
            property.setValue(toValue);
        else if (fromValue == null)
            chart.addProperty(new Property(name, toValue));
        else if (toValue == null)
            chart.removeProperty(property);
        else
            throw new RuntimeException("cannot happen");
    }

    @Override
    public void execute() {
        Property property = chart.lookupProperty(propertyName);
        oldValue = property == null ? null : property.getValue();
        setPropertyValue(chart, propertyName, newValue);
    }

    @Override
    public void undo() {
        setPropertyValue(chart, propertyName, oldValue);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart property";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
