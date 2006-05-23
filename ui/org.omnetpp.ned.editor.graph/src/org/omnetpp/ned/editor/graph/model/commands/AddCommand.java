package org.omnetpp.ned.editor.graph.model.commands;

import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * This command adds a child element to a container model element 
 * @author rhornig
 * 
 */
// TODO do not use index to position the child. Use the next sibling child istead
public class AddCommand extends org.eclipse.gef.commands.Command {

    private INamedGraphNode child;
    private ISubmoduleContainer parent;
    private int index = -1;

    @Override
    public void execute() {
        setLabel("Add " + child.getName());
        
        if (index < 0)
            parent.addSubmodule(child);
        else
            parent.insertSubmodule(index, child);
    }

    public ISubmoduleContainer getParent() {
        return parent;
    }

    @Override
    public void redo() {
        if (index < 0)
            parent.addSubmodule(child);
        else
            parent.insertSubmodule(index, child);
    }

    public void setChild(INamedGraphNode module) {
        child = module;
    }

    public void setIndex(int i) {
        index = i;
    }

    public void setParent(ISubmoduleContainer newParent) {
        parent = newParent;
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
    }

}