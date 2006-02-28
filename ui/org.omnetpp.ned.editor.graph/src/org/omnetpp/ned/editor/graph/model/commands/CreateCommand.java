package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedNode;

/**
 * Adds a newly created element to the model
 * @author rhornig
 *
 */
public class CreateCommand extends org.eclipse.gef.commands.Command {

    private INedNode child;
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
            Insets expansion = getInsets();
            if (!rect.isEmpty())
                rect.expand(expansion);
            else {
                rect.x -= expansion.left;
                rect.y -= expansion.top;
            }
            child.setLocation(rect.getLocation());
            if (!rect.isEmpty()) child.setSize(rect.getSize());
        }
        redo();
    }

    private Insets getInsets() {
        if (child instanceof CompoundModuleModel) return new Insets(2, 0, 2, 0);
        return new Insets();
    }

    public void redo() {
        parent.addChild(child, index);
    }

    public void setChild(NedNodeModel subpart) {
        child = subpart;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }

    public void setParent(ContainerModel newParent) {
        parent = newParent;
    }

    public void undo() {
        parent.removeChild(child);
    }

}