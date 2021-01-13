/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

public class SetChartPropertiesCommand implements ICommand {

    private Chart chart;
    private Map<String,String> oldProperties;
    private Map<String,String> newProperties;

    public SetChartPropertiesCommand(Chart chart, Map<String,String> properties) {
        this.chart = chart;
        this.newProperties = properties;
    }

    @Override
    public void execute() {
        oldProperties = chart.getPropertiesAsMap();
        chart.setProperties(newProperties);
    }

    @Override
    public void undo() {
        chart.setProperties(oldProperties);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart properties";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
