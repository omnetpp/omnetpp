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

import org.eclipse.draw2d.geometry.Dimension;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

/**
 * Model object that represent a submodule that cannot contain any other
 * object as its child (represented by an ICON and a LABEL)  
 * @author rhornig
 *
 */
public class SimpleModule extends NedElement {

//    static final long serialVersionUID = 1;
    private static int count;
    public static String TERMINAL_OUT = "OUT"; //$NON-NLS-1$
    public static String TERMINAL_A = "A", //$NON-NLS-1$
            TERMINAL_B = "B"; //$NON-NLS-1$

    public String toString() {
        return MessageFactory.Simple_LabelText + " #" + getName(); //$NON-NLS-1$
    }

    public String getNewID() {
        return Integer.toString(count++);
    }

//    public Dimension getSize() {
//        return new Dimension(-1, -1);
//    }

    public void removeOutput(Wire w) {
        outputs.remove(w);
    }

    public Object getPropertyValue(Object propName) {
        if (PROP_SIZE.equals(propName)) { return new String(
                "(" + getSize().width + "," + getSize().height + ")");//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
        }
        return super.getPropertyValue(propName);
    }

    /**
     * Nulls out any changes to this and its subclasses as they are of fixed
     * size.
     */
    public void setPropertyValue(Object id, Object value) {
        if (PROP_SIZE.equals(id))
            super.setPropertyValue(id, new Dimension(getSize()));
        else
            super.setPropertyValue(id, value);
    }

}
