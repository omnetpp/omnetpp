package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;

public class WireEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEditPolicy {

    protected Command getDeleteCommand(GroupRequest request) {
        ConnectionCommand c = new ConnectionCommand();
        c.setWire((ConnectionNodeEx) getHost().getModel());
        return c;
    }

}