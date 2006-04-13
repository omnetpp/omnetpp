package org.omnetpp.ned.editor.graph.model.commands;

import java.util.List;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned2.model.INedContainer;
import org.omnetpp.ned2.model.INedModule;
import org.omnetpp.ned2.model.NEDElement;

/**
 * This command removes a model element from its parent.
 * @author rhornig
 *
 */

public class OrphanChildCommand extends Command {

    private Point oldLocation;
    private INedContainer container;
    private INedModule child;
    private NEDElement insertBeforeChild;

    public void execute() {
        setLabel("Remove " + child.getName());

        insertBeforeChild = ((NEDElement)child).getNextSibling();
        oldLocation = child.getLocation();
       redo();
    }

    public void redo() {
        container.removeModelChild(child);
    }

    public void setChild(INedModule child) {
        this.child = child;
    }

    public void setParent(INedContainer parent) {
        container = parent;
    }

    public void undo() {
        child.setLocation(oldLocation);
        container.insertModelChild((INedModule)insertBeforeChild, child);
    }

}
