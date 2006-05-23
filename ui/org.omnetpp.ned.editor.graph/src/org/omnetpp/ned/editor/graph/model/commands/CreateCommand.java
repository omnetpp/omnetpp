package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * Adds a newly created element to the model
 * @author rhornig
 *
 */

//TODO do not use index to position the child. Use the next sibling child istead
public class CreateCommand extends org.eclipse.gef.commands.Command {

    private INamedGraphNode child;
    private Rectangle rect;
    private ISubmoduleContainer parent;
    private int index = -1;

    @Override
    public boolean canExecute() {
        return child != null && parent != null;
    }

    @Override
    public void execute() {
        setLabel("Create " + child.getName());

        if (rect != null) {
            child.getDisplayString().setConstraint(rect.getLocation(), rect.getSize());
        }
        redo();
    }

    @Override
    public void redo() {
        parent.insertSubmodule(index, child );
    }

    public void setChild(INamedGraphNode module) {
        child = module;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }

    public void setParent(ISubmoduleContainer newParent) {
        parent = newParent;
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
    }

}