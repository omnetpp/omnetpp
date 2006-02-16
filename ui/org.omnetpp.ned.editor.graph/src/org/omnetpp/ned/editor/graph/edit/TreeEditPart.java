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
import java.beans.PropertyChangeListener;
import java.util.Collections;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.ned.editor.graph.edit.policies.NedComponentEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedTreeEditPolicy;
import org.omnetpp.ned.editor.graph.model.Container;
import org.omnetpp.ned.editor.graph.model.NedNode;

/**
 * EditPart for Logic components in the Tree.
 */
public class TreeEditPart extends org.eclipse.gef.editparts.AbstractTreeEditPart implements
        PropertyChangeListener {

    /**
     * Constructor initializes this with the given model.
     * 
     * @param model
     *            Model for this.
     */
    public TreeEditPart(Object model) {
        super(model);
    }

    public void activate() {
        super.activate();
        getLogicSubpart().addPropertyChangeListener(this);
    }

    /**
     * Creates and installs pertinent EditPolicies for this.
     */
    protected void createEditPolicies() {
        EditPolicy component;
        component = new NedComponentEditPolicy();
        installEditPolicy(EditPolicy.COMPONENT_ROLE, component);
        installEditPolicy(EditPolicy.PRIMARY_DRAG_ROLE, new NedTreeEditPolicy());
    }

    public void deactivate() {
        getLogicSubpart().removePropertyChangeListener(this);
        super.deactivate();
    }

    /**
     * Returns the model of this as a LogicSubPart.
     * 
     * @return Model of this.
     */
    protected NedNode getLogicSubpart() {
        return (NedNode) getModel();
    }

    /**
     * Returns <code>null</code> as a Tree EditPart holds no children under
     * it.
     * 
     * @return <code>null</code>
     */
    protected List getModelChildren() {
        return Collections.EMPTY_LIST;
    }

    public void propertyChange(PropertyChangeEvent change) {
        if (change.getPropertyName().equals(Container.PROP_CHILDREN)) {
            if (change.getOldValue() instanceof Integer)
                // new child
                addChild(createChild(change.getNewValue()), ((Integer) change.getOldValue()).intValue());
            else
                // remove child
                removeChild((EditPart) getViewer().getEditPartRegistry().get(change.getOldValue()));
        } else
            refreshVisuals();
    }

    /**
	 * Refreshes the visual properties of the TreeItem for this part.
     */
    protected void refreshVisuals() {
        if (getWidget() instanceof Tree) return;
        TreeItem item = (TreeItem) getWidget();
        setWidgetText(getLogicSubpart().toString());
        // TODO here we should create an image for the tree element
//        Image image = getLogicSubpart().getIcon();
//        if (image != null) image.setBackground(item.getParent().getBackground());
//        setWidgetImage(image);
    }

}