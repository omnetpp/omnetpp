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

public class SetChartScriptCommand implements ICommand {

    private Chart chart;
    private String oldScript;
    private String newScript;

    public SetChartScriptCommand(Chart chart, String newValue) {
        this.chart = chart;
        this.newScript = newValue;
    }

    @Override
    public void execute() {
        oldScript = chart.getScript();
        chart.setScript(newScript);
    }

    @Override
    public void undo() {
        chart.setScript(oldScript);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart script";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
