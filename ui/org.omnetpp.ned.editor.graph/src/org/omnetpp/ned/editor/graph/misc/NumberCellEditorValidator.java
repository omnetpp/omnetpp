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
package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.jface.viewers.ICellEditorValidator;


public class NumberCellEditorValidator implements ICellEditorValidator {

    private static NumberCellEditorValidator instance;

    public static NumberCellEditorValidator instance() {
        if (instance == null) instance = new NumberCellEditorValidator();
        return instance;
    }

    public String isValid(Object value) {
        try {
            new Integer((String) value);
            return null;
        } catch (NumberFormatException exc) {
            return MessageFactory.CellEditorValidator_NotANumberMessage;
        }
    }

}