package org.omnetpp.ned.editor.graph.model.commands;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModule;

/**
 * This command adds a child element to a root model element (Container)
 * @author rhornig
 * 
 */
public class AddCommand extends org.eclipse.gef.commands.Command {

    private INedModule child;
    private INedContainer parent;
    private int index = -1;

    public AddCommand() {
        super(MessageFactory.AddCommand_Label);
    }

    public void execute() {
        if (index < 0)
            parent.addModelChild(child);
        else
            parent.insertModelChild(index, child);
    }

    public INedContainer getParent() {
        return parent;
    }

    public void redo() {
        if (index < 0)
            parent.addModelChild(child);
        else
            parent.insertModelChild(index, child);
    }

    public void setChild(INedModule subpart) {
        child = subpart;
    }

    public void setIndex(int i) {
        index = i;
    }

    public void setParent(INedContainer newParent) {
        parent = newParent;
    }

    public void undo() {
        parent.removeModelChild(child);
    }

}