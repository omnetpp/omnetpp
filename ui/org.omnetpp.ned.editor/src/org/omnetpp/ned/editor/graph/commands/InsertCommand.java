/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Adds a NED element to the model tree
 *
 * @author rhornig
 */
public class InsertCommand extends Command {
    private final INedElement child;
    private final INedElement parent;
    private final INedElement insertBefore;

    public InsertCommand(INedElement parent, INedElement child) {
        this(parent, child, null);
    }

    public InsertCommand(INedElement parent, INedElement child, INedElement where) {
        this.child = child;
        this.parent = parent;
        this.insertBefore = where;

        String label = "Add";
        if (child instanceof IHasName)
            label += " "+((IHasName)child).getName();
        setLabel(label);
    }

    @Override
    public boolean canExecute() {
        return child != null && parent != null;
    }

    @Override
    public void execute() {
        redo();
    }

    @Override
    public void redo() {
        parent.insertChildBefore(insertBefore, child);
    }

    @Override
    public void undo() {
        child.removeFromParent();
    }

}
