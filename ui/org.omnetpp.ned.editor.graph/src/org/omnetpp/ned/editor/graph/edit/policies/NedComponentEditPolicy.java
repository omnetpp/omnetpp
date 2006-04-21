package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;
import org.omnetpp.ned2.model.INedContainer;
import org.omnetpp.ned2.model.INedModule;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        DeleteCommand deleteCmd = new DeleteCommand();
        deleteCmd.setParent((INedContainer)getHost().getParent().getModel());
        deleteCmd.setChild((INedModule) getHost().getModel());
        return deleteCmd;
    }
}