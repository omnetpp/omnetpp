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
package org.omnetpp.ned.editor.graph.model;

import org.eclipse.draw2d.Bendpoint;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

public class WireBendpoint implements java.io.Serializable, Bendpoint {

//    static final long serialVersionUID = 1;

    private float weight = 0.5f;
    private Dimension d1, d2;

    public WireBendpoint() {
    }

    public Dimension getFirstRelativeDimension() {
        return d1;
    }

    public Point getLocation() {
        return null;
    }

    public Dimension getSecondRelativeDimension() {
        return d2;
    }

    public float getWeight() {
        return weight;
    }

    public void setRelativeDimensions(Dimension dim1, Dimension dim2) {
        d1 = dim1;
        d2 = dim2;
    }

    public void setWeight(float w) {
        weight = w;
    }

}