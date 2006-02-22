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

import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;
import org.omnetpp.ned.editor.graph.model.old.WireModel;

public class WireEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEditPolicy {

    protected Command getDeleteCommand(GroupRequest request) {
        ConnectionCommand c = new ConnectionCommand();
        c.setWire((WireModel) getHost().getModel());
        return c;
    }

}