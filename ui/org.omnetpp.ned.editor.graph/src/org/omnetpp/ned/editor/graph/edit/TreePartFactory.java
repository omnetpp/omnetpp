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
import org.omnetpp.ned.editor.graph.model.NedFile;

public class TreePartFactory implements EditPartFactory {

    public EditPart createEditPart(EditPart context, Object model) {
        if (model instanceof NedFile) return new ContainerTreeEditPart(model);
        return new TreeEditPart(model);
    }

}