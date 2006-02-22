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

import java.util.List;

import org.eclipse.draw2d.geometry.Point;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.old.ContainerModel;
import org.omnetpp.ned.editor.graph.model.old.NedNodeModel;

/**
 * This command removes a model element from its parent.
 * @author rhornig
 *
 */

public class OrphanChildCommand extends Command {

    private Point oldLocation;
    private ContainerModel diagram;
    private NedNodeModel child;
    private int index;

    public OrphanChildCommand() {
        super(MessageFactory.OrphanChildCommand_Label);
    }

    public void execute() {
        List children = diagram.getChildren();
        index = children.indexOf(child);
        oldLocation = child.getLocation();
        diagram.removeChild(child);
    }

    public void redo() {
        diagram.removeChild(child);
    }

    public void setChild(NedNodeModel child) {
        this.child = child;
    }

    public void setParent(ContainerModel parent) {
        diagram = parent;
    }

    public void undo() {
        child.setLocation(oldLocation);
        diagram.addChild(child, index);
    }

}
