/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INedElement;

/**
 * Move a child to a different position in the parent's list
 * Used by the tree container policy to drag and drop in the outline tree
 *
 * @author rhornig
 */
public class ReorderCommand extends Command {

    private INedElement node;
    private INedElement parent;
    private INedElement oldInsertBeforePos;
    private INedElement newInsertChildBefore;

    public ReorderCommand(INedElement insertBefore, INedElement child) {
        super("Reorder");
        node = child;
        parent = child.getParent();
        newInsertChildBefore = insertBefore;
    }

    @Override
    public void execute() {
        oldInsertBeforePos = node.getNextSibling();
        node.removeFromParent();
        parent.insertChildBefore(newInsertChildBefore, node);
    }

    @Override
    public void undo() {
        node.removeFromParent();
        parent.insertChildBefore(oldInsertBeforePos, node);
    }

}