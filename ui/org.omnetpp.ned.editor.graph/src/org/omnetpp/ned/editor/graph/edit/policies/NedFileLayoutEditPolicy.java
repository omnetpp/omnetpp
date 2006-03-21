package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.Iterator;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedModule;
import org.omnetpp.ned.editor.graph.model.NedFileNodeEx;
import org.omnetpp.ned.editor.graph.model.commands.AddCommand;
import org.omnetpp.ned.editor.graph.model.commands.CloneCommand;
import org.omnetpp.ned.editor.graph.model.commands.CreateCommand;
import org.omnetpp.ned.editor.graph.model.commands.ReorderPartCommand;

public class NedFileLayoutEditPolicy
	extends org.eclipse.gef.editpolicies.FlowLayoutEditPolicy
{

/**
 * Override to return the <code>Command</code> to perform an {@link
 * RequestConstants#REQ_CLONE CLONE}. By default, <code>null</code> is
 * returned.
 * @param request the Clone Request
 * @return A command to perform the Clone.
 */
protected Command getCloneCommand(ChangeBoundsRequest request) {
	CloneCommand clone = new CloneCommand();
	clone.setParent((NedFileNodeEx)getHost().getModel());
	
	EditPart after = getInsertionReference(request);
	int index = getHost().getChildren().indexOf(after);
	
	Iterator i = request.getEditParts().iterator();
	GraphicalEditPart currPart = null;
	
	while (i.hasNext()) {
		currPart = (GraphicalEditPart)i.next();
		clone.addPart((NedFileNodeEx)currPart.getModel(), index++);
	}
	
	return clone;
}
	
protected Command createAddCommand(EditPart child, EditPart after) {
	AddCommand command = new AddCommand();
	command.setChild((INedModule)child.getModel());
	command.setParent((INedContainer)getHost().getModel());
	int index = getHost().getChildren().indexOf(after);
	command.setIndex(index);
	return command;
}

/**
 * @see org.eclipse.gef.editpolicies.LayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
 */
protected EditPolicy createChildEditPolicy(EditPart child) {
	NedResizableEditPolicy policy = new NedResizableEditPolicy();
	policy.setResizeDirections(0);
	return policy;
}

protected Command createMoveChildCommand(EditPart child, EditPart after) {
	INedModule childModel = (INedModule)child.getModel();
	INedContainer parentModel = (INedContainer)getHost().getModel();
	int oldIndex = getHost().getChildren().indexOf(child);
	int newIndex = getHost().getChildren().indexOf(after);
	if (newIndex > oldIndex)
		newIndex--;
    
    
//	ReorderPartCommand command = new ReorderPartCommand(childModel, parentModel, newIndex);
//	return command;
    return UnexecutableCommand.INSTANCE;
}

protected Command getCreateCommand(CreateRequest request) {
	CreateCommand command = new CreateCommand();
	EditPart after = getInsertionReference(request);
	command.setChild((INedModule)request.getNewObject());
	command.setParent((INedContainer)getHost().getModel());
	int index = getHost().getChildren().indexOf(after);
	command.setIndex(index);
	return command;
}

}


