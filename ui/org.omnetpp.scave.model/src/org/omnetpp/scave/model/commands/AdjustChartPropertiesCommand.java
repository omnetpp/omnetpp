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

/**
 * Adjust the set of chart properties, usually so that it contains exactly those edited by the dialog pages.
 *
 * @author andras
 */
public class AdjustChartPropertiesCommand implements ICommand {

    private Chart chart;
    private Map<String,String> oldProperties;
    private Map<String,String> newPropertyDefaults; // property names edited by the new pages, with default values

    public AdjustChartPropertiesCommand(Chart chart, Map<String,String> propertyDefaults) {
        this.chart = chart;
        this.newPropertyDefaults = propertyDefaults;
    }

    @Override
    public void execute() {
        oldProperties = chart.getPropertiesAsMap();
        chart.adjustProperties(newPropertyDefaults);
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
        return "Adjust chart properties";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
