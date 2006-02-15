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

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.Command;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.Container;
import org.omnetpp.ned.editor.graph.model.NedNode;
import org.omnetpp.ned.editor.graph.model.Wire;

/**
 * Deletes an object from the model and also removes all associated connections
 * @author rhornig
 *
 */
public class DeleteCommand extends Command {

    private NedNode child;
    private Container parent;
    private int vAlign, hAlign;
    private int index = -1;
    private List sourceConnections = new ArrayList();
    private List targetConnections = new ArrayList();

    public DeleteCommand() {
        super(MessageFactory.DeleteCommand_Label);
    }

    private void deleteConnections(NedNode part) {
        if (part instanceof Container) {
            List children = ((Container) part).getChildren();
            for (int i = 0; i < children.size(); i++)
                deleteConnections((NedNode) children.get(i));
        }
        sourceConnections.addAll(part.getSourceConnections());
        for (int i = 0; i < sourceConnections.size(); i++) {
            Wire wire = (Wire) sourceConnections.get(i);
            wire.detachSource();
            wire.detachTarget();
        }
        targetConnections.addAll(part.getTargetConnections());
        for (int i = 0; i < targetConnections.size(); i++) {
            Wire wire = (Wire) targetConnections.get(i);
            wire.detachSource();
            wire.detachTarget();
        }
    }

    public void execute() {
        primExecute();
    }

    protected void primExecute() {
        deleteConnections(child);
        index = parent.getChildren().indexOf(child);
        parent.removeChild(child);
    }

    public void redo() {
        primExecute();
    }

    private void restoreConnections() {
        for (int i = 0; i < sourceConnections.size(); i++) {
            Wire wire = (Wire) sourceConnections.get(i);
            wire.attachSource();
            wire.attachTarget();
        }
        sourceConnections.clear();
        for (int i = 0; i < targetConnections.size(); i++) {
            Wire wire = (Wire) targetConnections.get(i);
            wire.attachSource();
            wire.attachTarget();
        }
        targetConnections.clear();
    }

    public void setChild(NedNode c) {
        child = c;
    }

    public void setParent(Container p) {
        parent = p;
    }

    public void undo() {
        parent.addChild(child, index);
        restoreConnections();
    }

}
