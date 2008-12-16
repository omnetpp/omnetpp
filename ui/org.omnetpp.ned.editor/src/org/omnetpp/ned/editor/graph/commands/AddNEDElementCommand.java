/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.model.INEDElement;

/**
 * Adds a NED element to the model tree
 *
 * @author rhornig
 */
public class AddNEDElementCommand extends Command {
    private final INEDElement child;
    private final INEDElement parent;
    private final INEDElement insertBefore;

    public AddNEDElementCommand(INEDElement parent, INEDElement child) {
        this(parent, child, null);
    }

    public AddNEDElementCommand(INEDElement parent, INEDElement child, INEDElement where) {
    	this.child = child;
    	this.parent = parent;
    	this.insertBefore = where;
    	setLabel("Add");
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
