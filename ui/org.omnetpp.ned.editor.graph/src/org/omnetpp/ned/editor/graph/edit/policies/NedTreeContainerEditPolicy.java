package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.TreeContainerEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;

public class NedTreeContainerEditPolicy extends TreeContainerEditPolicy {

//    protected Command createCreateCommand(INamedGraphNode child, Rectangle r, int index, String label) {
//        CreateSubmoduleCommand cmd = new CreateSubmoduleCommand((ISubmoduleContainer) getHost().getModel(),	child);
//        Rectangle rect;
//        if (r == null) {
//            rect = new Rectangle();
//            rect.setSize(new Dimension(-1, -1));
//        } else {
//            rect = r;
//        }
//        cmd.setLocation(rect);
//        cmd.setLabel(label);
//        if (index >= 0) cmd.setIndex(index);
//        return cmd;
//    }
//
//    @Override
//    protected Command getAddCommand(ChangeBoundsRequest request) {
//        CompoundCommand command = new CompoundCommand();
//        command.setDebugLabel("Add in NedTreeContainerEditPolicy");//$NON-NLS-1$
//        List editparts = request.getEditParts();
//        int index = findIndexOfTreeItemAt(request.getLocation());
//
//        for (int i = 0; i < editparts.size(); i++) {
//            EditPart child = (EditPart) editparts.get(i);
//            if (isAncestor(child, getHost()))
//                command.add(UnexecutableCommand.INSTANCE);
//            else {
//                INamedGraphNode childModel = (INamedGraphNode) child.getModel();
//                command.add(createCreateCommand(childModel, new Rectangle(
//                        new org.eclipse.draw2d.geometry.Point(), childModel.getDisplayString().getSize()), index,
//                        "Reparent NedElement"));//$NON-NLS-1$
//            }
//        }
//        return command;
//    }
//
//    @Override
//    protected Command getCreateCommand(CreateRequest request) {
//    	INamedGraphNode child = (INamedGraphNode) request.getNewObject();
//        INamedGraphNode targetNode = 
//        	(INamedGraphNode)((NedTreeEditPart)findTreeItemAt(request.getLocation()).getData()).getModel();
//        
//    	System.out.println("child: "+child+" target: "+targetNode+" "+request.getLocation().toString());
//
//    	if (child instanceof SubmoduleNodeEx && targetNode instanceof SubmoduleNodeEx)
//        {
//        	return createCreateCommand(child, null, -1, "Create NedElement");//$NON-NLS-1$
//        }
//        
//        return UnexecutableCommand.INSTANCE;
//    }

    @Override
    protected Command getMoveChildrenCommand(ChangeBoundsRequest request) {
        CompoundCommand command = new CompoundCommand();
        List editparts = request.getEditParts();
        List children = getHost().getChildren();
        int newIndex = findIndexOfTreeItemAt(request.getLocation());
        
        for (int i = 0; i < editparts.size(); i++) {
            EditPart child = (EditPart) editparts.get(i);
            int tempIndex = newIndex;
            int oldIndex = children.indexOf(child);
            if (oldIndex == tempIndex || oldIndex + 1 == tempIndex) {
                command.add(UnexecutableCommand.INSTANCE);
                return command;
            } else if (oldIndex <= tempIndex) {
                tempIndex--;
            }
// FIXME removed for debugging            
//            command.add(new ReorderPartCommand((NEDElement) child.getModel(), (NEDElement) getHost().getModel(), tempIndex));
        }
        return command;
    }

    protected boolean isAncestor(EditPart source, EditPart target) {
        if (source == target) return true;
        if (target.getParent() != null) return isAncestor(source, target.getParent());
        return false;
    }

	@Override
	protected Command getAddCommand(ChangeBoundsRequest request) {
		return UnexecutableCommand.INSTANCE;
	}

	@Override
	protected Command getCreateCommand(CreateRequest request) {
		return UnexecutableCommand.INSTANCE;
	}

}