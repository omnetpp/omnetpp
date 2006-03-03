package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.INedContainer;
import org.omnetpp.ned.editor.graph.model.INedNode;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    protected Command createDeleteCommand(GroupRequest request) {
        DeleteCommand deleteCmd = new DeleteCommand();
        deleteCmd.setParent((INedContainer)getHost().getParent().getModel());
        deleteCmd.setChild((INedNode) getHost().getModel());
        return deleteCmd;
    }
}