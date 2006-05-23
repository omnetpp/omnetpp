package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        DeleteCommand deleteCmd = new DeleteCommand();
        deleteCmd.setParent((ISubmoduleContainer)getHost().getParent().getModel());
        deleteCmd.setChild((INamedGraphNode) getHost().getModel());
        return deleteCmd;
    }
}