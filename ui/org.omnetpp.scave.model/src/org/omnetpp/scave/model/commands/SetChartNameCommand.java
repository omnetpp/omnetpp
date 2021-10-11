/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

public class SetChartNameCommand implements ICommand {

    private AnalysisItem item;
    private String oldName;
    private String newName;
    boolean titleWasEmpty = false;

    public SetChartNameCommand(AnalysisItem item, String newValue) {
        this.item = item;
        this.newName = newValue;
    }

    @Override
    public void execute() {
        oldName = item.getName();
        item.setName(newName);

        if (item instanceof Chart) {
            Chart chart = (Chart)item;
            String title = chart.getPropertyValue("title");
            if (title.isEmpty()) {
                titleWasEmpty = true;
                chart.setPropertyValue("title", newName);
            }
        }
    }

    @Override
    public void undo() {
        item.setName(oldName);

        if (titleWasEmpty) {
            Assert.isTrue(item instanceof Chart);
            Chart chart = (Chart)item;
            chart.setPropertyValue("title", "");
        }
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Set chart name (and title)";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(item);
    }

}
