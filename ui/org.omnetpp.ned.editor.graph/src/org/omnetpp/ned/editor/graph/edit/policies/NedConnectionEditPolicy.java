package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ConnectionEditPolicy;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.commands.ConnectionCommand;
import org.omnetpp.ned2.model.ConnectionNodeEx;

public class NedConnectionEditPolicy extends ConnectionEditPolicy {

    @Override
    protected Command getDeleteCommand(GroupRequest request) {
        ConnectionCommand c = new ConnectionCommand((ConnectionNodeEx) getHost().getModel());
        // do not set any src/destModule / Gate for the connection command. This command
        // will delete the whole connection and remove it from the model
        return c;
    }

}