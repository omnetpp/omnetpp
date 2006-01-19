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

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.misc.NativeDropRequest;
import org.omnetpp.ned.editor.graph.model.Comment;
import org.omnetpp.ned.editor.graph.model.commands.CommentCommand;

public class CommentEditPolicy extends NedComponentEditPolicy {

    public Command getCommand(Request request) {
        if (NativeDropRequest.ID.equals(request.getType()))
            return getDropTextCommand((NativeDropRequest) request);
        return super.getCommand(request);
    }

    protected Command getDropTextCommand(NativeDropRequest request) {
        CommentCommand command = new CommentCommand((Comment) getHost().getModel(), (String) request
                .getData());
        return command;
    }

    public EditPart getTargetEditPart(Request request) {
        if (NativeDropRequest.ID.equals(request.getType())) return getHost();
        return super.getTargetEditPart(request);
    }

}
