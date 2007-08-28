package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ConnectionEditPolicy;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.commands.ConnectionCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleConnectionEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.PolicyUtil;
import org.omnetpp.ned.model.ex.ConnectionElementEx;

/**
 * Generally this contains only the delete command for the connections
 *
 * @author rhornig
 */
public class NedConnectionEditPolicy extends ConnectionEditPolicy {

    @Override
    protected Command getDeleteCommand(GroupRequest request) {
        // do not allow delete if we are read only components
        if (!PolicyUtil.isEditable(getHost()))
            return null;

        ModuleEditPart srcEP = (ModuleEditPart)((ModuleConnectionEditPart)getHost()).getSource();
        ModuleEditPart targetEP = (ModuleEditPart)((ModuleConnectionEditPart)getHost()).getTarget();
        CompoundModuleEditPart parentEP = (CompoundModuleEditPart)getHost().getParent();
        // do not set any src/destModule / Gate for the connection command. This command
        // will delete the whole connection and remove it from the model
        return new ConnectionCommand((ConnectionElementEx) getHost().getModel(), parentEP, srcEP, targetEP);
    }

}