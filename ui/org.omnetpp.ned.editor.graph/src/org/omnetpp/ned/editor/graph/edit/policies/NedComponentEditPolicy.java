package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;
import org.omnetpp.ned2.model.ISubmoduleContainer;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.NEDElement;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        DeleteCommand deleteCmd = new DeleteCommand((NEDElement)getHost().getModel());
//        deleteCmd.setContainer((ISubmoduleContainer)getHost().getParent().getModel());
//        deleteCmd.setChild((INamedGraphNode) getHost().getModel());
        return deleteCmd;
    }
}