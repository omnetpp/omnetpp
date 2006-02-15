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

import org.eclipse.gef.EditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.NedContainerEditPolicy;

/**
 * Provides support for Container EditParts.
 */
abstract public class ContainerEditPart extends BaseEditPart {

    /**
     * Installs the desired EditPolicies for this.
     */
    protected void createEditPolicies() {
        super.createEditPolicies();
        installEditPolicy(EditPolicy.CONTAINER_ROLE, new NedContainerEditPolicy());
    }

}
