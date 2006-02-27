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
package org.omnetpp.ned.editor.graph.model.old;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.PositionConstants;
import org.omnetpp.ned.editor.graph.misc.MessageFactory;

/**
 * Object that can contain other NedElements in its content area as child entities
 * Can set the connection routing for the childs and other geometric features.
 * Rulers, guides, grid etc. Used as the root element of the model
 * @author rhornig
 *
 */
public class ContainerModelX extends ModelElementBase {
//    static final long serialVersionUID = 1;
    public static final String PROP_CHILDREN = "children"; //$NON-NLS-1$

    protected List<ModelElementBase> children = new ArrayList<ModelElementBase>();

    public void addChild(ModelElementBase child) {
        addChild(child, -1);
    }

    public void addChild(ModelElementBase child, int index) {
        if (index >= 0)
            children.add(index, child);
        else
            children.add(child);
        fireChildAdded(PROP_CHILDREN, child, new Integer(index));
    }

    public List getChildren() {
        return children;
    }

    private void readObject(java.io.ObjectInputStream s) throws IOException, ClassNotFoundException {
        s.defaultReadObject();
    }

    public void removeChild(ModelElementBase child) {
        children.remove(child);
        fireChildRemoved(PROP_CHILDREN, child);
    }

    public String toString() {
        return MessageFactory.LogicDiagram_LabelText;
    }

}