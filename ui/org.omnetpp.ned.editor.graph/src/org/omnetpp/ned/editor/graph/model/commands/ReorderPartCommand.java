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

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.Container;
import org.omnetpp.ned.editor.graph.model.NedNode;

/**
 * Move a child to a different position in the parent's list
 * Used by the tree containe policy to drang and drop in the outline tree
 * TODO Do we need this functionality at all? Is it useful?
 * @author rhornig
 *
 */
public class ReorderPartCommand extends Command {

    private int oldIndex, newIndex;
    private NedNode child;
    private Container parent;

    public ReorderPartCommand(NedNode child, Container parent, int newIndex) {
        super(MessageFactory.ReorderPartCommand_Label);
        this.child = child;
        this.parent = parent;
        this.newIndex = newIndex;
    }

    public void execute() {
        oldIndex = parent.getChildren().indexOf(child);
        parent.removeChild(child);
        parent.addChild(child, newIndex);
    }

    public void undo() {
        parent.removeChild(child);
        parent.addChild(child, oldIndex);
    }

}