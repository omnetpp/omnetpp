package org.omnetpp.ned.editor.graph.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.model.INEDElement;

/**
 * Move a child to a different position in the parent's list
 * Used by the tree container policy to drag and drop in the outline tree
 *
 * @author rhornig
 */
public class ReorderCommand extends Command {

    private INEDElement node;
    private INEDElement parent;
    private INEDElement oldInsertBeforePos;
    private INEDElement newInsertChildBefore;

    public ReorderCommand(INEDElement insertBefore, INEDElement child) {
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