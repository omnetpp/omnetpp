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

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.CompoundModuleModel;
import org.omnetpp.ned.editor.graph.model.ContainerModel;
import org.omnetpp.ned.editor.graph.model.NedNodeModel;

/**
 * Adds a newly created element to the model (similar to the AddCommand)
 * TODO check what is the difference between these two commands. Do we need both?
 * @author rhornig
 *
 */
public class CreateCommand extends org.eclipse.gef.commands.Command {

    private NedNodeModel child;
    private Rectangle rect;
    private ContainerModel parent;
    private int index = -1;

    public CreateCommand() {
        super(MessageFactory.CreateCommand_Label);
    }

    public boolean canExecute() {
        return child != null && parent != null;
    }

    public void execute() {
        if (rect != null) {
            Insets expansion = getInsets();
            if (!rect.isEmpty())
                rect.expand(expansion);
            else {
                rect.x -= expansion.left;
                rect.y -= expansion.top;
            }
            child.setLocation(rect.getLocation());
            if (!rect.isEmpty()) child.setSize(rect.getSize());
        }
        redo();
    }

    private Insets getInsets() {
        if (child instanceof CompoundModuleModel) return new Insets(2, 0, 2, 0);
        return new Insets();
    }

    public void redo() {
        parent.addChild(child, index);
    }

    public void setChild(NedNodeModel subpart) {
        child = subpart;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setLocation(Rectangle r) {
        rect = r;
    }

    public void setParent(ContainerModel newParent) {
        parent = newParent;
    }

    public void undo() {
        parent.removeChild(child);
    }

}