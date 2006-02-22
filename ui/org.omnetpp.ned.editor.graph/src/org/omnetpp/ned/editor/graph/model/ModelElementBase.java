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

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;

/**
 * Provides property listener  / change notification and prop. manipulation support
 * @author rhornig
 *
 */
abstract public class ModelElementBase implements IPropertySource, Cloneable, Serializable {


    transient protected PropertyChangeSupport listeners = new PropertyChangeSupport(this);

    public void addPropertyChangeListener(PropertyChangeListener l) {
        listeners.addPropertyChangeListener(l);
    }

    public void removePropertyChangeListener(PropertyChangeListener l) {
        listeners.removePropertyChangeListener(l);
    }

    protected void firePropertyChange(String prop, Object old, Object newValue) {
        listeners.firePropertyChange(prop, old, newValue);
    }

    protected void fireChildAdded(String prop, ModelElementBase child, int index) {
        listeners.firePropertyChange(prop, index, child);
    }

    protected void fireChildRemoved(String prop, ModelElementBase child) {
        listeners. firePropertyChange(prop, child, null);
    }

    protected void fireStructureChange(String prop, ModelElementBase child) {
        listeners.firePropertyChange(prop, null, child);
    }

    public Object getEditableValue() {
        return this;
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return new IPropertyDescriptor[0];
    }

    public Object getPropertyValue(Object propName) {
        return null;
    }

    public boolean isPropertySet(Object propName) {
        return true;
    }

    public void resetPropertyValue(Object propName) {
    }

    public void setPropertyValue(Object propName, Object val) {
    }

//  private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
//  in.defaultReadObject();
//  listeners = new PropertyChangeSupport(this);
//  }

}
