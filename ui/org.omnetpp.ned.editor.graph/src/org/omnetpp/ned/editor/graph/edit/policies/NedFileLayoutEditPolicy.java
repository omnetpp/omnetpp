package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.FlowLayoutEditPolicy;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.ned.editor.graph.model.commands.CreateToplevelComponentCommand;
import org.omnetpp.ned2.model.NEDElement;

public class NedFileLayoutEditPolicy extends FlowLayoutEditPolicy {

	
	/* (non-Javadoc)
	 * @see org.eclipse.gef.editpolicies.FlowLayoutEditPolicy#isHorizontal()
	 * we override this function so it is possible to use the ToolbarLayout
	 * with the FlowLayoutPolicy, because implementation of FlowLayoutEditPolicy#isHorizonta()
	 * depends on FlowLayout
	 */
	@Override
	protected boolean isHorizontal() {
		return false;
	}
	
	// TODO implement generic clone command
	protected Command getCloneCommand(ChangeBoundsRequest request) {
//		CloneCommand clone = new CloneCommand();
//		clone.setParent((LogicDiagram)getHost().getModel());
//		
//		EditPart after = getInsertionReference(request);
//		int index = getHost().getChildren().indexOf(after);
//		
//		Iterator i = request.getEditParts().iterator();
//		GraphicalEditPart currPart = null;
//		
//		while (i.hasNext()) {
//			currPart = (GraphicalEditPart)i.next();
//			clone.addPart((LogicSubpart)currPart.getModel(), index++);
//		}
//		
//		return clone;
		return null;
	}
		
	// TODO implement add command or maybe we don't need it at all?
	protected Command createAddCommand(EditPart child, EditPart after) {
//		AddCommand command = new AddCommand();
//		command.setChild((LogicSubpart)child.getModel());
//		command.setParent((LogicFlowContainer)getHost().getModel());
//		int index = getHost().getChildren().indexOf(after);
//		command.setIndex(index);
//		return command;
		return null;
	}

	/**
	 * @see org.eclipse.gef.editpolicies.LayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
	 */
	protected EditPolicy createChildEditPolicy(EditPart child) {
        ResizableEditPolicy policy = new ResizeFeedbackEditPolicy();
        return policy;
	}


	// TODO implement child movement and reordering
	protected Command createMoveChildCommand(EditPart child, EditPart after) {
//		LogicSubpart childModel = (LogicSubpart)child.getModel();
//		LogicDiagram parentModel = (LogicDiagram)getHost().getModel();
//		int oldIndex = getHost().getChildren().indexOf(child);
//		int newIndex = getHost().getChildren().indexOf(after);
//		if (newIndex > oldIndex)
//			newIndex--;
//		ReorderPartCommand command = new ReorderPartCommand(childModel, parentModel, newIndex);
//		return command;
		return null;
	}

	protected Command getCreateCommand(CreateRequest request) {
		NEDElement newElement = (NEDElement)request.getNewObject();
		EditPart insertionPoint = getInsertionReference(request);
		NEDElement where = (insertionPoint != null) ? (NEDElement)insertionPoint.getModel() : null;
		NEDElement parent = (NEDElement)getHost().getModel();
		return new CreateToplevelComponentCommand(parent, where, newElement);
	}
}
