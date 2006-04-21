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

import org.eclipse.swt.dnd.Transfer;

import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.Request;
import org.eclipse.gef.dnd.AbstractTransferDropTargetListener;

import org.omnetpp.ned.editor.graph.misc.NativeDropRequest;

public class TextTransferDropTargetListener extends AbstractTransferDropTargetListener {

    public TextTransferDropTargetListener(EditPartViewer viewer, Transfer xfer) {
        super(viewer, xfer);
    }

    @Override
    protected Request createTargetRequest() {
        return new NativeDropRequest();
    }

    protected NativeDropRequest getNativeDropRequest() {
        return (NativeDropRequest) getTargetRequest();
    }

    @Override
    protected void updateTargetRequest() {
        getNativeDropRequest().setData(getCurrentEvent().data);
    }

}
