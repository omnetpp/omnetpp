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

import org.omnetpp.ned.editor.graph.misc.MessageFactory;

/**
 * A ned compund module displaying its submodules and supporting several gate connections
 * @author rhornig
 *
 */
public class CompoundModuleModel extends NedNodeModel {

//    static final long serialVersionUID = 1;

    public static String TERMINALS_OUT[] = new String[] { "1", "2", "3", "4", //$NON-NLS-4$//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
            "5", "6", "7", "8" };//$NON-NLS-4$//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$

    public static String TERMINALS_IN[] = new String[] { "A", "B", "C", "D", //$NON-NLS-4$//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
            "E", "F", "G", "H" };//$NON-NLS-4$//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$


    public String toString() {
        return MessageFactory.Circuit_LabelText + " #" + getName(); //$NON-NLS-1$
    }
}
