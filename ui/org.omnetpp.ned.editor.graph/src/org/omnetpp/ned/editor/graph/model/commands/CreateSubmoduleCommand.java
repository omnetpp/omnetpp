package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;

/**
 * Adds a newly created element to the model
 * @author rhornig
 *
 */
public class CreateSubmoduleCommand extends org.eclipse.gef.commands.Command {

    private INamedGraphNode child;
    private Rectangle rect;
    private ISubmoduleContainer parent;
    private int index = -1;

    
    public CreateSubmoduleCommand(ISubmoduleContainer parent, INamedGraphNode child) {
    	this.child = child;
    	this.parent = parent;
    }
    
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
        parent.insertSubmodule(index, child);
    }

    @Override
    public void undo() {
        parent.removeSubmodule(child);
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }
}