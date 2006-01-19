/*******************************************************************************
 * Copyright (c) 2004 IBM Corporation and others.
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

import org.omnetpp.ned.editor.graph.model.Guide;
import org.omnetpp.ned.editor.graph.model.NedElement;

/**
 * Reassigns a part to a new diagram guide and detaches it from the previous one.
 * @author Pratik Shah
 */
public class ChangeGuideCommand extends Command {

    private NedElement part;
    private Guide oldGuide, newGuide;
    private int oldAlign, newAlign;
    private boolean horizontal;

    public ChangeGuideCommand(NedElement part, boolean horizontalGuide) {
        super();
        this.part = part;
        horizontal = horizontalGuide;
    }

    protected void changeGuide(Guide oldGuide, Guide newGuide, int newAlignment) {
        if (oldGuide != null && oldGuide != newGuide) {
            oldGuide.detachPart(part);
        }
        // You need to re-attach the part even if the oldGuide and the newGuide
        // are the same
        // because the alignment could have changed
        if (newGuide != null) {
            newGuide.attachPart(part, newAlignment);
        }
    }

    public void execute() {
        // Cache the old values
        oldGuide = horizontal ? part.getHorizontalGuide() : part.getVerticalGuide();
        if (oldGuide != null) oldAlign = oldGuide.getAlignment(part);

        redo();
    }

    public void redo() {
        changeGuide(oldGuide, newGuide, newAlign);
    }

    public void setNewGuide(Guide guide, int alignment) {
        newGuide = guide;
        newAlign = alignment;
    }

    public void undo() {
        changeGuide(newGuide, oldGuide, oldAlign);
    }

}