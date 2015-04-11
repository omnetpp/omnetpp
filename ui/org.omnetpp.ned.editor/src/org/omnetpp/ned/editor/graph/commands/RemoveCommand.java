/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.core.runtime.Assert;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Removes a NED element. Useful as a building block for higher level
 * compound commands.
 *
 * @author andras
 */
public class RemoveCommand extends Command {
    private INedElement element;
    private INedElement parent;
    private INedElement nextSibling;

    public RemoveCommand(INedElement element) {
        this.element = element;
    }

    @Override
    public void execute() {
        parent = element.getParent();
        nextSibling = element.getNextSibling();
        Assert.isNotNull(parent);

        String label = "Delete";
        if (element instanceof IHasName)
            label += " "+((IHasName)element).getName();
        setLabel(label);

        redo();
    }

    @Override
    public void redo() {
        element.removeFromParent();
    }

    @Override
    public void undo() {
        parent.insertChildBefore(nextSibling, element);
    }
}
