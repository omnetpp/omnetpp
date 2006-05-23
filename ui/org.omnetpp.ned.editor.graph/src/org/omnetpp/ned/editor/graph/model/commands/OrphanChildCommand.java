package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.NEDElement;

/**
 * This command removes a model element from its parent.
 * @author rhornig
 *
 */

public class OrphanChildCommand extends Command {

    private Point oldLocation;
    private ISubmoduleContainer container;
    private INamedGraphNode child;
    private NEDElement insertBeforeChild;

    @Override
    public void execute() {
        setLabel("Remove " + child.getName());

        insertBeforeChild = ((NEDElement)child).getNextSibling();
        oldLocation = child.getDisplayString().getLocation();
       redo();
    }

    @Override
    public void redo() {
        container.removeSubmodule(child);
    }

    public void setChild(INamedGraphNode child) {
        this.child = child;
    }

    public void setParent(ISubmoduleContainer parent) {
        container = parent;
    }

    @Override
    public void undo() {
        child.getDisplayString().setLocation(oldLocation);
        container.insertSubmodule((INamedGraphNode)insertBeforeChild, child);
    }

}
