/*******************************************************************************
 * Copyright (c) 2003, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.model;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.rulers.RulerProvider;

/**
 * @author Pratik Shah
 */
public class Ruler implements Serializable {

    public static final String PROPERTY_CHILDREN = "children changed"; //$NON-NLS-1$
    public static final String PROPERTY_UNIT = "units changed"; //$NON-NLS-1$

//    static final long serialVersionUID = 1;

    protected PropertyChangeSupport listeners = new PropertyChangeSupport(this);
    private int unit;
    private boolean horizontal;
    private List guides = new ArrayList();

    public Ruler(boolean isHorizontal) {
        this(isHorizontal, RulerProvider.UNIT_PIXELS);
    }

    public Ruler(boolean isHorizontal, int unit) {
        horizontal = isHorizontal;
        setUnit(unit);
    }

    public void addGuide(Guide guide) {
        if (!guides.contains(guide)) {
            guide.setHorizontal(!isHorizontal());
            guides.add(guide);
            listeners.firePropertyChange(PROPERTY_CHILDREN, null, guide);
        }
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        listeners.addPropertyChangeListener(listener);
    }

    // the returned list should not be modified
    public List getGuides() {
        return guides;
    }

    public int getUnit() {
        return unit;
    }

    public boolean isHidden() {
        return false;
    }

    public boolean isHorizontal() {
        return horizontal;
    }

    public void removeGuide(Guide guide) {
        if (guides.remove(guide)) {
            listeners.firePropertyChange(PROPERTY_CHILDREN, null, guide);
        }
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        listeners.removePropertyChangeListener(listener);
    }

    public void setHidden(boolean isHidden) {
    }

    public void setUnit(int newUnit) {
        if (unit != newUnit) {
            int oldUnit = unit;
            unit = newUnit;
            listeners.firePropertyChange(PROPERTY_UNIT, oldUnit, newUnit);
        }
    }

}