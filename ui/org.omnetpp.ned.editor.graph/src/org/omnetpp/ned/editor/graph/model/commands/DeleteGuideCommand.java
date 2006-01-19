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

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.eclipse.gef.commands.Command;

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.Guide;
import org.omnetpp.ned.editor.graph.model.Ruler;
import org.omnetpp.ned.editor.graph.model.NedElement;

/**
 * Deletes a guide from the ruler object and detaches all parts from it
 * @author rhornig
 *
 */
public class DeleteGuideCommand extends Command {

    private Ruler parent;
    private Guide guide;
    private Map oldParts;

    public DeleteGuideCommand(Guide guide, Ruler parent) {
        super(MessageFactory.DeleteGuideCommand_Label);
        this.guide = guide;
        this.parent = parent;
    }

    public boolean canUndo() {
        return true;
    }

    public void execute() {
        oldParts = new HashMap(guide.getMap());
        Iterator iter = oldParts.keySet().iterator();
        while (iter.hasNext()) {
            guide.detachPart((NedElement) iter.next());
        }
        parent.removeGuide(guide);
    }

    public void undo() {
        parent.addGuide(guide);
        Iterator iter = oldParts.keySet().iterator();
        while (iter.hasNext()) {
            NedElement part = (NedElement) iter.next();
            guide.attachPart(part, ((Integer) oldParts.get(part)).intValue());
        }
    }
}
