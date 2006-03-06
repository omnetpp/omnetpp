package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModule;

/**
 * Adds a newly created element to the model
 * @author rhornig
 *
 */
public class CreateCommand extends org.eclipse.gef.commands.Command {

    private INedModule child;
    private Rectangle rect;
    private INedContainer parent;
    private int index = -1;

    public CreateCommand() {
        super(MessageFactory.CreateCommand_Label);
    }

    public boolean canExecute() {
        return child != null && parent != null;
    }

    public void execute() {
        if (rect != null) {
            child.setLocation(rect.getLocation());
            // TODO child size must be set correctly in the displaystring
            // if (!rect.isEmpty()) child.setSize(rect.getSize());
        }
        redo();
    }

    public void redo() {
        parent.insertModelChild(index, child );
    }

    public void setChild(INedModule subpart) {
        child = subpart;
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

    public void undo() {
        parent.removeModelChild(child);
    }

}