package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;
import org.omnetpp.ned2.model.ConnectionNodeEx;

public class ConnectionEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEditPolicy {

    @Override
    protected Command getDeleteCommand(GroupRequest request) {
        ConnectionCommand c = new ConnectionCommand((ConnectionNodeEx) getHost().getModel());
        // the new source and destination module is not set (ie NULL) this effectively removes 
        // the connection from the modell
        return c;
//    	return new DeleteCommand((ConnectionNodeEx) getHost().getModel());
    }

}