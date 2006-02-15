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

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.NedNode;

/**
 * Change the size and location of the element
 * @author rhornig
 *
 */
public class SetConstraintCommand extends org.eclipse.gef.commands.Command {

    private Point newPos;
    private Dimension newSize;
    private Point oldPos;
    private Dimension oldSize;
    private NedNode part;

    public void execute() {
        oldSize = part.getSize();
        oldPos = part.getLocation();
        redo();
    }

    public String getLabel() {
        if (oldSize.equals(newSize)) return MessageFactory.SetLocationCommand_Label_Location;
        return MessageFactory.SetLocationCommand_Label_Resize;
    }

    public void redo() {
        part.setSize(newSize);
        part.setLocation(newPos);
    }

    public void setLocation(Rectangle r) {
      setLocation(r.getLocation());
        setSize(r.getSize());
    }

    public void setLocation(Point p) {
        newPos = p;
    }

    public void setPart(NedNode part) {
        this.part = part;
    }

    public void setSize(Dimension p) {
        newSize = p;
    }

    public void undo() {
        part.setSize(oldSize);
        part.setLocation(oldPos);
    }

}