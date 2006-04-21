package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned2.model.INedContainer;
import org.omnetpp.ned2.model.INedModule;

/**
 * Adds a newly created element to the model
 * @author rhornig
 *
 */

//TODO do not use index to position the child. Use the next sibling child istead
public class CreateCommand extends org.eclipse.gef.commands.Command {

    private INedModule child;
    private Rectangle rect;
    private INedContainer parent;
    private int index = -1;

    @Override
    public boolean canExecute() {
        return child != null && parent != null;
    }

    @Override
    public void execute() {
        setLabel("Create " + child.getName());

        if (rect != null) {
            child.setLocation(rect.getLocation());
            if (!rect.isEmpty()) 
                child.setSize(rect.getSize());
        }
        redo();
    }

    @Override
    public void redo() {
        parent.insertModelChild(index, child );
    }

    public void setChild(INedModule module) {
        child = module;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }

    public void setParent(INedContainer newParent) {
        parent = newParent;
    }

    @Override
    public void undo() {
        parent.removeModelChild(child);
    }

}