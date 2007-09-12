package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.ConnectionEditPolicy;
import org.eclipse.gef.requests.GroupRequest;

import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.edit.EditPartUtil;
import org.omnetpp.ned.model.INEDElement;

/**
 * Generally this contains only the delete command for the connections
 *
 * @author rhornig
 */
public class NedConnectionEditPolicy extends ConnectionEditPolicy {

    @Override
    protected Command getDeleteCommand(GroupRequest request) {
        // do not allow delete if we are read only components
        if (!EditPartUtil.isEditable(getHost()))
            return UnexecutableCommand.INSTANCE;

        return new DeleteCommand((INEDElement)getHost().getModel());
    }

}