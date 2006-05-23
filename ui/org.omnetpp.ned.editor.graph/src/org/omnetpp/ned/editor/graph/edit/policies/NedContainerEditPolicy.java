package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.ContainerEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.commands.OrphanChildCommand;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;

public class NedContainerEditPolicy extends ContainerEditPolicy {

    @Override
    protected Command getCreateCommand(CreateRequest request) {
    	// it does not provide a creation command because the ...LayoutPolicy already
    	// provides it
        return null;
    }

    @Override
    public Command getOrphanChildrenCommand(GroupRequest request) {
        List parts = request.getEditParts();
        CompoundCommand result = new CompoundCommand("Orphan children");
        for (int i = 0; i < parts.size(); i++) {
            OrphanChildCommand orphan = new OrphanChildCommand();
            orphan.setChild((INamedGraphNode) ((EditPart) parts.get(i)).getModel());
            orphan.setParent((ISubmoduleContainer) getHost().getModel());
            orphan.setLabel("Orphan child");
            result.add(orphan);
        }
        return result.unwrap();
    }

}