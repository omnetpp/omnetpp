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

        if (model instanceof WireModel)
            child = new WireEditPart();
        else if (model instanceof CompoundModuleModel)
            child = new CompoundModuleEditPart();
        else if (model instanceof SubmoduleModel)
            child = new SubmoduleEditPart();
        else if (model instanceof NedFileModel) child = new NedFileDiagramEditPart();

        child.setModel(model);
        return child;
    }

}
