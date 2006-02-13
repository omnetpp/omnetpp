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
package org.omnetpp.ned.editor.graph.model;

import java.io.IOException;

import org.eclipse.draw2d.geometry.Dimension;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

/**
 * Object representing comments that can be displayed as sticky notes on the diagram
 * @author rhornig
 *
 */
public class Comment extends NedElement {
//    static final long serialVersionUID = 1;

    private String text = MessageFactory.LogicPlugin_Tool_CreationTool_LogicLabel;

    private static int count;

    public Comment() {
        super();
        size.width = 50;
    }

    public String getLabelContents() {
        return text;
    }

    protected String getNewID() {
        return Integer.toString(count++);
    }

    public Dimension getSize() {
        return new Dimension(size.width, -1);
    }

    private void readObject(java.io.ObjectInputStream s) throws IOException, ClassNotFoundException {
        s.defaultReadObject();
    }

    public void setSize(Dimension d) {
        d.height = -1;
        super.setSize(d);
    }

    public void setLabelContents(String s) {
        text = s;
        firePropertyChange("labelContents", null, text); //$NON-NLS-2$//$NON-NLS-1$
    }

    public String toString() {
        return MessageFactory.LogicPlugin_Tool_CreationTool_LogicLabel + " #" + getName() + " " //$NON-NLS-1$ //$NON-NLS-2$
                + MessageFactory.PropertyDescriptor_Label_Text + "=" + getLabelContents(); //$NON-NLS-1$ 
    }

}