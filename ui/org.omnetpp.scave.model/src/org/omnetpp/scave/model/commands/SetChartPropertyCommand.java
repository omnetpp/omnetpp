/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;

/**
 * Command to set (or add if doesn't exist) a chart property.
 */
public class SetChartPropertyCommand implements ICommand {

    private Chart chart;
    private String propertyName;
    private String oldValue;
    private String newValue;
    private boolean isNewProperty;

    public SetChartPropertyCommand(Chart chart, String propertyName, String newValue) {
        this.chart = chart;
        this.propertyName = propertyName;
        this.newValue = newValue;

        Assert.isNotNull(newValue);
    }

    @Override
    public void execute() {
        Property property = chart.getProperty(propertyName);
        isNewProperty = (property == null);
        if (isNewProperty)
            chart.addProperty(new Property(propertyName, newValue));
        else {
            oldValue = property.getValue();
            property.setValue(newValue);
        }
    }

    @Override
    public void undo() {
        Property property = chart.getProperty(propertyName);
        if (isNewProperty)
            chart.removeProperty(property);
        else
            property.setValue(oldValue);
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
