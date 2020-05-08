/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Chart.DialogPage;
import org.omnetpp.scave.model.ModelObject;

public class SetChartDialogPagesCommand implements ICommand {

    private Chart chart;
    private List<DialogPage> oldDialogPages;
    private List<DialogPage> newDialogPages;

    public SetChartDialogPagesCommand(Chart chart, List<DialogPage> dialogPages) {
        this.chart = chart;
        this.newDialogPages = dialogPages;
    }

    @Override
    public void execute() {
        oldDialogPages = new ArrayList<DialogPage>(chart.getDialogPages());
        chart.setDialogPages(new ArrayList<DialogPage>(newDialogPages));
    }

    @Override
    public void undo() {
        chart.setDialogPages(new ArrayList<DialogPage>(oldDialogPages));
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Reset chart dialog pages to template";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(chart);
    }

}
