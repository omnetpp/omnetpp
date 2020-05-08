/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;
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
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart, property);
    }
}
