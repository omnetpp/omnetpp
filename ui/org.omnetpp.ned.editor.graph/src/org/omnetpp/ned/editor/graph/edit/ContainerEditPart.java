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

import java.util.List;

import org.eclipse.gef.AccessibleEditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.swt.accessibility.AccessibleEvent;
import org.omnetpp.ned.editor.graph.edit.policies.NedContainerEditPolicy;
import org.omnetpp.ned.editor.graph.model.Container;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart extends BaseEditPart {
    protected AccessibleEditPart createAccessible() {
        return new AccessibleGraphicalEditPart() {
            public void getName(AccessibleEvent e) {
//                e.result = getContainerModel().toString();
            }
        };
    }

    /**
     * Installs the desired EditPolicies for this.
     */
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.CONTAINER_ROLE, new NedContainerEditPolicy());
    }

    /**
     * Returns the model of this as a Container.
     * 
     * @return Container of this.
     */
//    protected Container getContainerModel() {
//        return (Container) getModel();
//    }

    /**
     * Returns the children of this through the model.
     * 
     * @return Children of this as a List.
     */
//    protected List getModelChildren() {
//        return getContainerModel().getChildren();
//    }

}
