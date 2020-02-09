package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
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

        Assert.isNotNull(newValue);
    }

    private static void setPropertyValue(Chart chart, String name, String toValue) {
        Property property = chart.lookupProperty(name);
        Assert.isNotNull(property); //TODO should be able to ADD a property
        String fromValue = property.getValue();

        if (ObjectUtils.equals(fromValue, toValue))
            return; // nothing to do

        property.setValue(toValue);
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
