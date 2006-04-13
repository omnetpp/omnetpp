package org.omnetpp.ned.editor.graph.model.commands;

import org.omnetpp.ned2.model.INedContainer;
import org.omnetpp.ned2.model.INedModule;

/**
 * This command adds a child element to a container model element 
 * @author rhornig
 * 
 */
// TODO do not use index to position the child. Use the next sibling child istead
public class AddCommand extends org.eclipse.gef.commands.Command {

    private INedModule child;
    private INedContainer parent;
    private int index = -1;

    public void execute() {
        setLabel("Add " + child.getName());
        
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

    public void setChild(INedModule module) {
        child = module;
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