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

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;

import org.omnetpp.ned.editor.graph.model.Container;
import org.omnetpp.ned.editor.graph.model.NedElement;
import org.omnetpp.ned.editor.graph.model.commands.DeleteCommand;

public class NedComponentEditPolicy extends org.eclipse.gef.editpolicies.ComponentEditPolicy {

    protected Command createDeleteCommand(GroupRequest request) {
        Object parent = getHost().getParent().getModel();
        DeleteCommand deleteCmd = new DeleteCommand();
        deleteCmd.setParent((Container) parent);
        deleteCmd.setChild((NedElement) getHost().getModel());
        return deleteCmd;
    }

}