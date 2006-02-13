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
package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.ui.IEditorPart;

import org.eclipse.draw2d.geometry.Point;

import org.eclipse.gef.requests.CreationFactory;

import org.omnetpp.ned.editor.graph.model.ElementFactory;

/**
 * @author Eric Bordeau
 */
public class ModulePasteTemplateAction extends PasteTemplateAction {

    /**
     * Constructor for ModulePasteTemplateAction.
     * 
     * @param editor
     */
    public ModulePasteTemplateAction(IEditorPart editor) {
        super(editor);
    }

    /**
     * @see org.eclipse.gef.ui.actions.PasteTemplateAction#getFactory(java.lang.Object)
     */
    protected CreationFactory getFactory(Object template) {
        if (template instanceof String) return new ElementFactory((String) template);
        return null;
    }

    /**
     * 
     * @see org.omnetpp.ned.editor.graph.actions.PasteTemplateAction#getPasteLocation()
     */
    protected Point getPasteLocation() {
        return new Point(10, 10);
    }

}
