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
package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.*;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.ned.editor.graph.misc.ColorFactory;

/**
 *  This command is responsible for highlighting the background of the 
 *  container figures during specific operations and the returning to the original color    
 */
public class ContainerHighlightEditPolicy extends org.eclipse.gef.editpolicies.GraphicalEditPolicy {

    private Color revertColor;

    public void showTargetFeedback(Request request) {
        if (request.getType().equals(RequestConstants.REQ_MOVE)
                || request.getType().equals(RequestConstants.REQ_ADD)
                || request.getType().equals(RequestConstants.REQ_CLONE)
                || request.getType().equals(RequestConstants.REQ_CONNECTION_START)
                || request.getType().equals(RequestConstants.REQ_CONNECTION_END)
                || request.getType().equals(RequestConstants.REQ_CREATE)) showHighlight();
    }

    public void eraseTargetFeedback(Request request) {
        if (revertColor != null) {
            setContainerBackground(revertColor);
            revertColor = null;
        }
    }

    private Color getContainerBackground() {
        return getContainerFigure().getBackgroundColor();
    }

    private IFigure getContainerFigure() {
        return ((GraphicalEditPart) getHost()).getFigure();
    }

    public EditPart getTargetEditPart(Request request) {
        return request.getType().equals(RequestConstants.REQ_SELECTION_HOVER) ? getHost() : null;
    }

    private void setContainerBackground(Color c) {
        getContainerFigure().setBackgroundColor(c);
    }

    protected void showHighlight() {
        if (revertColor == null) {
            revertColor = getContainerBackground();
            setContainerBackground(ColorFactory.backgroundBlue);
        }
    }
    
}