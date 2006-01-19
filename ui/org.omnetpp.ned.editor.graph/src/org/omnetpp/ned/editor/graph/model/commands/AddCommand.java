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

import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.Container;
import org.omnetpp.ned.editor.graph.model.NedElement;

/**
 * This command adds a child element to a root model element (Container)
 * @author rhornig
 * 
 */
public class AddCommand extends org.eclipse.gef.commands.Command {

    private NedElement child;
    private Container parent;
    private int index = -1;

    public AddCommand() {
        super(MessageFactory.AddCommand_Label);
    }

    public void execute() {
        if (index < 0)
            parent.addChild(child);
        else
            parent.addChild(child, index);
    }

    public Container getParent() {
        return parent;
    }

    public void redo() {
        if (index < 0)
            parent.addChild(child);
        else
            parent.addChild(child, index);
    }

    public void setChild(NedElement subpart) {
        child = subpart;
    }

    public void setIndex(int i) {
        index = i;
    }

    public void setParent(Container newParent) {
        parent = newParent;
    }

    public void undo() {
        parent.removeChild(child);
    }

}