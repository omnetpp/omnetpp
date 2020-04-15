/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.DirectEditPolicy;
import org.eclipse.gef.requests.DirectEditRequest;
import org.omnetpp.ned.editor.graph.commands.RenameCommand;
import org.omnetpp.ned.model.interfaces.IHasName;

/**
 * Handle direct edit request which is a rename in our case
 *
 * @author rhornig
 */
public class NedDirectEditPolicy extends DirectEditPolicy {
    @Override
    protected Command getDirectEditCommand(DirectEditRequest request) {
        String newName = (String)request.getCellEditor().getValue();
        return new RenameCommand((IHasName)getHost().getModel(), newName);
    }

    @Override
    protected void showCurrentEditValue(DirectEditRequest request) {
    }

}
