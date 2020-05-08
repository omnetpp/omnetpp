/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.ModelObject;

public class RemoveChartCommand implements ICommand {

    private int index;
    private ModelObject parent;
    private AnalysisItem chart;

    public RemoveChartCommand(AnalysisItem chart) {
        this.chart = chart;
        this.parent = chart.getParent();
    }

    @Override
    public void execute() {
        ModelObject parent = chart.getParent();

        if (parent instanceof Charts) {
            index = ((Charts)parent).getCharts().indexOf(chart);
        }

        if (parent instanceof Charts)
            ((Charts)parent).removeChart(chart);
        else {
            // TODO
        }
    }

    @Override
    public void undo() {
        if (parent instanceof Charts)
            ((Charts)parent).addChart(chart, index);

    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Remove chart";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(parent, chart);
    }

}
