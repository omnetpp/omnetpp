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

import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;

import org.eclipse.gef.EditPartViewer;

import org.omnetpp.ned.editor.graph.edit.CommentEditPart;

public class FileTransferDragSourceListener extends org.eclipse.gef.dnd.AbstractTransferDragSourceListener {

    public FileTransferDragSourceListener(EditPartViewer viewer) {
        super(viewer, TextTransfer.getInstance());
    }

    public FileTransferDragSourceListener(EditPartViewer viewer, Transfer xfer) {
        super(viewer, xfer);
    }

    public void dragSetData(DragSourceEvent event) {
        event.data = "Some text"; //$NON-NLS-1$
    }

    public void dragStart(DragSourceEvent event) {
        if (getViewer().getSelectedEditParts().get(0) instanceof CommentEditPart) return;
        event.doit = false;
    }

}
