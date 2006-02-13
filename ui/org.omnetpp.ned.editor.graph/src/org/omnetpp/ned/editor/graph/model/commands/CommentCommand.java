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
package org.omnetpp.ned.editor.graph.model.commands;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.model.Comment;

/**
 * Changes the content of a comment
 * @author rhornig
 *
 */
public class CommentCommand extends Command {

    private String newName, oldName;
    private Comment label;

    public CommentCommand(Comment l, String s) {
        label = l;
        if (s != null)
            newName = s;
        else
            newName = ""; //$NON-NLS-1$
    }

    public void execute() {
        oldName = label.getLabelContents();
        label.setLabelContents(newName);
    }

    public void undo() {
        label.setLabelContents(oldName);
    }

}
