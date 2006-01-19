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
package org.omnetpp.ned.editor.graph.edit;

import java.beans.PropertyChangeEvent;

import org.eclipse.swt.accessibility.AccessibleControlEvent;
import org.eclipse.swt.accessibility.AccessibleEvent;

import org.eclipse.jface.viewers.TextCellEditor;

import org.eclipse.draw2d.IFigure;

import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.tools.DirectEditManager;

import org.omnetpp.ned.editor.graph.edit.policies.CommentDirectEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.CommentEditPolicy;
import org.omnetpp.ned.editor.graph.figures.CommentFigure;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;
import org.omnetpp.ned.editor.graph.model.Comment;

public class CommentEditPart extends BaseEditPart {

    protected AccessibleEditPart createAccessible() {
        return new AccessibleGraphicalEditPart() {
            public void getValue(AccessibleControlEvent e) {
                e.result = getComment().getLabelContents();
            }

            public void getName(AccessibleEvent e) {
                e.result = MessageFactory.LogicPlugin_Tool_CreationTool_LogicLabel;
            }
        };
    }

    private DirectEditManager manager;

    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.GRAPHICAL_NODE_ROLE, null);
        installEditPolicy(EditPolicy.DIRECT_EDIT_ROLE, new CommentDirectEditPolicy());
        installEditPolicy(EditPolicy.COMPONENT_ROLE, new CommentEditPolicy());
    }

    protected IFigure createFigure() {
        CommentFigure label = new CommentFigure();
        return label;
    }

    private Comment getComment() {
        return (Comment) getModel();
    }

    private void performDirectEdit() {
        if (manager == null)
            manager = new CommentEditManager(this, TextCellEditor.class, new CommentCellEditorLocator(
                    (CommentFigure) getFigure()));
        manager.show();
    }

    public void performRequest(Request request) {
        if (request.getType() == RequestConstants.REQ_DIRECT_EDIT) performDirectEdit();
    }

    public void propertyChange(PropertyChangeEvent evt) {
        if (evt.getPropertyName().equalsIgnoreCase("labelContents"))//$NON-NLS-1$
            refreshVisuals();
        else
            super.propertyChange(evt);
    }

    protected void refreshVisuals() {
        ((CommentFigure) getFigure()).setText(getComment().getLabelContents());
        super.refreshVisuals();
    }

}