package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.edit.PolicyUtil;
import org.omnetpp.ned.model.INEDElement;

/**
 * Adds support for deleting model elements
 *
 * @author rhornig
 */
public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        // do not allow delete if we are read only components
        if (!PolicyUtil.isEditable(getHost()))
            return null;

        return new DeleteCommand((INEDElement)getHost().getModel());
    }
}