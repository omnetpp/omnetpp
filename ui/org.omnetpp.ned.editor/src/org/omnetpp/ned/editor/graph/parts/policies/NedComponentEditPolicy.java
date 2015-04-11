/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.ComponentEditPolicy;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.model.INedElement;

/**
 * Adds support for deleting model elements
 *
 * @author rhornig
 */
public class NedComponentEditPolicy extends ComponentEditPolicy {

    @Override
    protected Command createDeleteCommand(GroupRequest request) {
        // do not allow delete if we are read only components
        if (!EditPartUtil.isEditable(getHost()))
            return null;

        return new DeleteCommand((INedElement)getHost().getModel());
    }
}