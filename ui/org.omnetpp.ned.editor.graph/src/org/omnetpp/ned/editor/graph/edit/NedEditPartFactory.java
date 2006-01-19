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

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;

import org.omnetpp.ned.editor.graph.model.*;

public class NedEditPartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        EditPart child = null;

        if (model instanceof Wire)
            child = new WireEditPart();
        else if (model instanceof Comment)
            child = new CommentEditPart();
        else if (model instanceof Module)
            child = new ModuleEditPart();
        else if (model instanceof SimpleModule)
            child = new SimpleModuleEditPart();
        // Module (which is also a container) was already handled so this one
        // must be the root object (which is only a container)
        // FIXME this looks really weird that the root element of the model is a Conainer only
        // some subclass could have been introduced to make it nicer
        else if (model instanceof Container) child = new DiagramEditPart();
        child.setModel(model);
        return child;
    }

}
