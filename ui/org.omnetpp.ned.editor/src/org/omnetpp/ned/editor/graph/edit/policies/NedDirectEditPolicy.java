package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.DirectEditPolicy;
import org.eclipse.gef.requests.DirectEditRequest;
import org.omnetpp.ned.editor.graph.commands.RenameCommand;
import org.omnetpp.ned.model.INEDElement;

public class NedDirectEditPolicy extends DirectEditPolicy {

    @Override
    protected Command getDirectEditCommand(DirectEditRequest request) {
        return new RenameCommand((INEDElement)getHost().getModel(),
                                 (String)request.getCellEditor().getValue());
    }

    @Override
    protected void showCurrentEditValue(DirectEditRequest request) {
    }

}
