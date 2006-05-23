package org.omnetpp.ned.editor.graph.model.commands;

import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.NEDElement;

/**
 * This command adds an already existing submodule to the SubmoduleContainer 
 * @author rhornig
 */
public class AddSubmoduleCommand extends org.eclipse.gef.commands.Command {

    private INamedGraphNode child;
    private INamedGraphNode nextSibling;
    private ISubmoduleContainer parent;

    public AddSubmoduleCommand(ISubmoduleContainer parent, INamedGraphNode child) {
    	this.child = child;
    	this.parent = parent;
	}
    
    @Override
    public void execute() {
        setLabel("Add " + child.getName());
        parent.insertSubmodule(nextSibling, child);
        nextSibling = (INamedGraphNode)((NEDElement)child).getNextSibling();
    }

    @Override
    public void redo() {
    	execute();
    }

    @Override
    public void undo() {
    	child.removeFromParent();
    }
}