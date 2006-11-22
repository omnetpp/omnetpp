package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned2.model.NEDElement;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        // do not allow delete if we are read only components
        if (getHost() instanceof IReadOnlySupport 
                && !((IReadOnlySupport)getHost()).isEditable())
            return null;
        
        return new DeleteCommand((NEDElement)getHost().getModel());
    }
}