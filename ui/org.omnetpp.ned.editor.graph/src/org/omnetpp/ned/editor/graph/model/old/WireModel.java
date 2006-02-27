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

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.eclipse.draw2d.Bendpoint;

public class WireModelX extends ModelElementBase {

//    static final long serialVersionUID = 1;
    static final String BENDPOINT = "bendpoint";
    static final String SOURCE = "source";
    static final String SOURCEGATE = "sourceGate";
    static final String TARGET = "target";
    static final String TARGETGATE = "targetGate";

    protected NedNodeModel source, target;
    protected String sourceGate, targetGate;
    protected List<Bendpoint> bendpoints = new ArrayList<Bendpoint>();
    protected String display;

    public void attachSource() {
        if (getSource() == null || getSource().getSourceConnections().contains(this)) return;
        getSource().connectOutput(this);
    }

    public void attachTarget() {
        if (getTarget() == null || getTarget().getTargetConnections().contains(this)) return;
        getTarget().connectInput(this);
    }

    public void detachSource() {
        if (getSource() == null) return;
        getSource().disconnectOutput(this);
    }

    public void detachTarget() {
        if (getTarget() == null) return;
        getTarget().disconnectInput(this);
    }

    public List<Bendpoint> getBendpoints() {
        return bendpoints;
    }

    public NedNodeModel getSource() {
        return source;
    }

    public String getSourceGate() {
        return sourceGate;
    }

    public NedNodeModel getTarget() {
        return target;
    }

    public String getTargetGate() {
        return targetGate;
    }

    public void insertBendpoint(int index, Bendpoint point) {
        getBendpoints().add(index, point);
        firePropertyChange(BENDPOINT, null, null);//$NON-NLS-1$
    }

    public void removeBendpoint(int index) {
        getBendpoints().remove(index);
        firePropertyChange(BENDPOINT, null, null);//$NON-NLS-1$
    }

    public void setBendpoint(int index, Bendpoint point) {
        getBendpoints().set(index, point);
        firePropertyChange(BENDPOINT, null, null);//$NON-NLS-1$
    }

    public void setBendpoints(Vector<Bendpoint> points) {
        bendpoints = points;
        firePropertyChange(BENDPOINT, null, null);//$NON-NLS-1$
    }

    public void setSource(NedNodeModel e) {
        Object old = source;
        source = e;
        firePropertyChange(SOURCE, old, source);//$NON-NLS-1$
    }

    public void setSourceGate(String s) {
        Object old = sourceGate;
        sourceGate = s;
        firePropertyChange(SOURCEGATE, old, sourceGate);//$NON-NLS-1$
    }

    public void setTarget(NedNodeModel e) {
        Object old = target;
        target = e;
        firePropertyChange(TARGET, old, target);//$NON-NLS-1$
    }

    public void setTargetGate(String s) {
        Object old = targetGate;
        targetGate = s;
        firePropertyChange(TARGETGATE, old, targetGate);//$NON-NLS-1$
    }

    public String toString() {
        return "Wire(" + getSource() + "," + getSourceGate() + "->" + getTarget() + "," + getTargetGate() + ")";//$NON-NLS-5$//$NON-NLS-4$//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
    }

}
