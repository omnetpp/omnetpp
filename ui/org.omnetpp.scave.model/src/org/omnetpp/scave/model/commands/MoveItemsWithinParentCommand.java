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

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelObject;

/**
 * Change the order of child items within a folder.
 * @author andras
 */
public class MoveItemsWithinParentCommand implements ICommand {
    private Folder container;
    private AnalysisItem item;
    private int oldIndex;
    private int newIndex;

    public MoveItemsWithinParentCommand(Folder container, AnalysisItem item, int newIndex) {
        this.container = container;
        this.item = item;
        this.newIndex = newIndex;

        Assert.isTrue(oldIndex >= 0);
    }

    @Override
    public void execute() {
        this.oldIndex = container.getItems().indexOf(item);

        List<AnalysisItem> items = new ArrayList<AnalysisItem>(container.getItems());

        items.remove(oldIndex);
        items.add(newIndex, item);

        container.setItems(items);
    }

    @Override
    public void undo() {
        List<AnalysisItem> items = new ArrayList<AnalysisItem>(container.getItems());

        items.remove(newIndex);
        items.add(oldIndex, item);

        container.setItems(items);
    }

    @Override
    public void redo() {
        execute();
    }

    @Override
    public String getLabel() {
        return "Move items";
    }

    @Override
    public Collection<ModelObject> getAffectedObjects() {
        return Arrays.asList(container, item);
    }

}
