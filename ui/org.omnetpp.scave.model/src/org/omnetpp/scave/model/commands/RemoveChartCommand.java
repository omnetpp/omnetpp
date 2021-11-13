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
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelObject;

public class RemoveChartCommand implements ICommand {
    private int index;
    private Folder container;
    private AnalysisItem item;

    public RemoveChartCommand(AnalysisItem item) {
        this.item = item;
        this.container = item.getParentFolder();
        Assert.isNotNull(this.container, "root folder cannot be removed (or: item is not in a folder)");
    }

    @Override
    public void execute() {
        container = item.getParentFolder();
        index = container.getItems().indexOf(item);
        container.remove(item);
    }

    @Override
    public void undo() {
        container.add(item, index);
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
        return Arrays.asList(container, item);
    }

}
