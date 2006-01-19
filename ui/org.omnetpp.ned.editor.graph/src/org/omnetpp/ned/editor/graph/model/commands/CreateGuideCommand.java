/*******************************************************************************
 * Copyright (c) 2003, 2004 IBM Corporation and others.
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
import org.omnetpp.ned.editor.graph.model.Guide;
import org.omnetpp.ned.editor.graph.model.Ruler;

/**
 * Creates a new guide line and adds it to the model (Ruler object)
 * @author Pratik Shah
 */
public class CreateGuideCommand extends Command {

    private Guide guide;
    private Ruler parent;
    private int position;

    public CreateGuideCommand(Ruler parent, int position) {
        super(MessageFactory.CreateGuideCommand_Label);
        this.parent = parent;
        this.position = position;
    }

    public boolean canUndo() {
        return true;
    }

    public void execute() {
        if (guide == null) guide = new Guide(!parent.isHorizontal());
        guide.setPosition(position);
        parent.addGuide(guide);
    }

    public void undo() {
        parent.removeGuide(guide);
    }

}