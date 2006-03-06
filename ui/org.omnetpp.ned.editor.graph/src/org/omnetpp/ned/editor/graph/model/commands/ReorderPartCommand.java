package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModule;

/**
 * Move a child to a different position in the parent's list
 * Used by the tree containe policy to drang and drop in the outline tree
 * TODO Do we need this functionality at all? Is it useful?
 * @author rhornig
 *
 */
public class ReorderPartCommand extends Command {

    private int oldIndex, newIndex;
    private INedModule child;
    private INedContainer parent;

    public ReorderPartCommand(INedModule child, INedContainer parent, int newIndex) {
        super(MessageFactory.ReorderPartCommand_Label);
        this.child = child;
        this.parent = parent;
        this.newIndex = newIndex;
    }

    public void execute() {
        oldIndex = parent.getModelChildren().indexOf(child);
        parent.removeModelChild(child);
        parent.insertModelChild(newIndex, child);
    }

    public void undo() {
        parent.removeModelChild(child);
        parent.insertModelChild(oldIndex, child);
    }

}