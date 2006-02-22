/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.ContainerEditPolicy;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.commands.OrphanChildCommand;
import org.omnetpp.ned.editor.graph.model.old.ContainerModel;
import org.omnetpp.ned.editor.graph.model.old.NedNodeModel;

public class NedContainerEditPolicy extends ContainerEditPolicy {

    protected Command getCreateCommand(CreateRequest request) {
        return null;
    }

    public Command getOrphanChildrenCommand(GroupRequest request) {
        List parts = request.getEditParts();
        CompoundCommand result = new CompoundCommand(
                MessageFactory.LogicContainerEditPolicy_OrphanCommandLabelText);
        for (int i = 0; i < parts.size(); i++) {
            OrphanChildCommand orphan = new OrphanChildCommand();
            orphan.setChild((NedNodeModel) ((EditPart) parts.get(i)).getModel());
            orphan.setParent((ContainerModel) getHost().getModel());
            orphan.setLabel(MessageFactory.LogicElementEditPolicy_OrphanCommandLabelText);
            result.add(orphan);
        }
        return result.unwrap();
    }

}