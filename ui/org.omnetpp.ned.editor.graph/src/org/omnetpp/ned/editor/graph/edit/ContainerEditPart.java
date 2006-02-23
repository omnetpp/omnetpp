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

import java.beans.PropertyChangeListener;
import java.util.List;

import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.NodeEditPart;
import org.omnetpp.ned.editor.graph.edit.policies.NedContainerEditPolicy;
import org.omnetpp.ned.editor.graph.model.old.ContainerModel;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart extends org.eclipse.gef.editparts.AbstractGraphicalEditPart implements
 PropertyChangeListener  {

    /**
     * Installs the desired EditPolicies for this.
     */
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.CONTAINER_ROLE, new NedContainerEditPolicy());
    }

//    protected List getModelChildren() {
//    	return ((ContainerModel)getModel()).getChildren();
//    }
    
}
