/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model.commands;

import java.util.Arrays;
import java.util.Collection;

import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelObject;

public class RemoveChartCommand implements ICommand {

    private int index;
    private ModelObject parent;
    private AnalysisItem item;

    public RemoveChartCommand(AnalysisItem item) {
        this.item = item;
        this.parent = item.getParent();
    }

    @Override
    public void execute() {
        ModelObject parent = item.getParent();

        if (parent instanceof Folder) {
            index = ((Folder)parent).getItems().indexOf(item);
        }

        if (parent instanceof Folder)
            ((Folder)parent).remove(item);
        else {
            // TODO
        }
    }

    @Override
    public void undo() {
        if (parent instanceof Folder)
            ((Folder)parent).add(item, index);

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
        return Arrays.asList(parent, item);
    }

}
