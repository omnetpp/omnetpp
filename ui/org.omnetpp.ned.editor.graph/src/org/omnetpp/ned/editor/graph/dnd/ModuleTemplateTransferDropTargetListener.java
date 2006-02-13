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
package org.omnetpp.ned.editor.graph.dnd;

import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.dnd.TemplateTransferDropTargetListener;
import org.eclipse.gef.requests.CreationFactory;

import org.omnetpp.ned.editor.graph.model.ElementFactory;

/**
 * This listener handles logic templates that are dropped onto the logic editor.
 * 
 * @author Eric Bordeau
 */
public class ModuleTemplateTransferDropTargetListener extends TemplateTransferDropTargetListener {

    public ModuleTemplateTransferDropTargetListener(EditPartViewer viewer) {
        super(viewer);
    }

    protected CreationFactory getFactory(Object template) {
        if (template instanceof String) return new ElementFactory((String) template);
        return null;
    }

}
