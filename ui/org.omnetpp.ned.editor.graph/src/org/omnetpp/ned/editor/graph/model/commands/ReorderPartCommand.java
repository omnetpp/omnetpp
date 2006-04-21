package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Move a child to a different position in the parent's list
 * Used by the tree container policy to drag and drop in the outline tree
 * @author rhornig
 *
 */
public class ReorderPartCommand extends Command {

    private NEDElement node;
    private NEDElement parent;
    private NEDElement oldInsertBeforePos;
    private NEDElement newInsertChildBefore;

    public ReorderPartCommand(NEDElement insertBefore, NEDElement child) {
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