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
import java.io.ObjectInputStream;
import java.util.*;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.editor.graph.properties.DimensionPropertySource;
import org.omnetpp.ned.editor.graph.properties.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.LocationPropertySource;

/**
 * Abstract NED node, that has a size, location, icon, label and can be connected to
 * other similar nodes. Can be attached to visual guides
 * @author rhornig
 *
 */
abstract public class NedNodeModelX extends ContainerModel {

    protected static int count;
    private String name = "";
    protected Hashtable<String, WireModel> inputs = new Hashtable<String, WireModel>(7);
    protected Point location = new Point(0, 0);
    protected Vector<WireModel> outputs = new Vector<WireModel>(4, 4);
//    static final long serialVersionUID = 1;
    protected Dimension size = new Dimension(-1, -1);
    protected String display;
    
    transient protected DisplayPropertySource displayPS = new DisplayPropertySource(display);

    protected static PropertyDescriptor[] descriptors = null;
    public static final String PROP_SIZE = "size"; //$NON-NLS-1$
    public static final String PROP_LOCATION = "location"; //$NON-NLS-1$
    public static final String PROP_NAME = "name"; //$NON-NLS-1$
    public static final String PROP_INPUTS = "inputs"; //$NON-NLS-1$
    public static final String PROP_OUTPUTS = "outputs"; //$NON-NLS-1$

    static {
        descriptors = new PropertyDescriptor[] {
                new TextPropertyDescriptor(PROP_NAME, "Name"),
                new PropertyDescriptor(PROP_SIZE, "Size (transient)"),
                new TextPropertyDescriptor(PROP_LOCATION, "Location (transient)"),
                new TextPropertyDescriptor(DisplayPropertySource.ID, "Display") };
    }

    private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        in.defaultReadObject();
        // init the display property parser
        displayPS = new DisplayPropertySource(display);
    }

    public NedNodeModel() {
        setName(getNewID());
    }

	public String getNewID() {
	    return Integer.toString(count++);
	}

    public String getName() {
        return name;
    }

    public void setName(String s) {
        String oldName = name;
        name = s;
        firePropertyChange(PROP_NAME, oldName, name); //$NON-NLS-1$
    }

    public void connectInput(WireModel w) {
        inputs.put(w.getTargetGate(), w);
        fireStructureChange(PROP_INPUTS, w);
    }

    public void connectOutput(WireModel w) {
        outputs.addElement(w);
        fireStructureChange(PROP_OUTPUTS, w);
    }

    public void disconnectInput(WireModel w) {
        inputs.remove(w.getTargetGate());
        fireStructureChange(PROP_INPUTS, w);
    }

    public void disconnectOutput(WireModel w) {
        outputs.removeElement(w);
        fireStructureChange(PROP_OUTPUTS, w);
    }

    public Vector getConnections() {
        Vector v = (Vector) outputs.clone();
        Enumeration ins = inputs.elements();
        while (ins.hasMoreElements())
            v.addElement(ins.nextElement());
        return v;
    }

    /**
     * Returns useful property descriptors for the use in property sheets. this
     * supports location and size.
     * 
     * @return Array of property descriptors.
     */
    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    public Vector getSourceConnections() {
        return (Vector) outputs.clone();
    }

    public Vector getTargetConnections() {
        Enumeration elements = inputs.elements();
        Vector v = new Vector(inputs.size());
        while (elements.hasMoreElements())
            v.addElement(elements.nextElement());
        return v;
    }

    public boolean isPropertySet() {
        return true;
    }

    /**
     * Returns an Object which represents the appropriate value for the property
     * name supplied.
     * 
     * @param propName
     *            Name of the property for which the the values are needed.
     * @return Object which is the value of the property.
     */
    public Object getPropertyValue(Object id) {
        String propName = (String)id;
        if (PROP_NAME.equals(propName)) return name;
        else if (PROP_SIZE.equals(propName)) return new DimensionPropertySource(getSize());
        else if (PROP_LOCATION.equals(propName)) return new LocationPropertySource(getLocation());
        else if (DisplayPropertySource.ID.equals(propName)) return displayPS;
        // delegate the getter to the parser object
        else if (propName.startsWith(DisplayPropertySource.ID))
            displayPS.getPropertyValue(id);
        return null;
    }

    /**
     * Sets the value of a given property with the value supplied. Also fires a
     * property change if necessary.
     * 
     * @param name
     *            Name of the parameter to be changed.
     * @param value
     *            Value to be set to the given parameter.
     */
    public void setPropertyValue(Object id, Object value) {
        String propName = (String)id;
        if (PROP_NAME.equals(propName)) 
            setName((String) value);
        else if (PROP_SIZE.equals(propName))
            setSize((Dimension) value);
        else if (PROP_LOCATION.equals(propName)) 
            setLocation((Point) value);
        else if (DisplayPropertySource.ID.equals(propName))
            setDisplay((String) value);
        // delegate the setter to the parser object
        else if (propName.startsWith(DisplayPropertySource.ID))
            displayPS.setPropertyValue(id, value);
    }

    public String getDisplay() {
        return display;
    }

    public void setDisplay(String newVal) {
        if(display != null && display.equals(newVal)) return;
        
        String oldVal = this.display;
        this.display = newVal;

        // parse the new display string again
        displayPS.setValue(newVal);
        firePropertyChange(DisplayPropertySource.ID, oldVal, newVal); 
    }

    // helper to get the size
    public Dimension getSize() {
        return new Dimension(displayPS.getIntPropertyDef(DisplayPropertySource.PROP_W,-1),
                             displayPS.getIntPropertyDef(DisplayPropertySource.PROP_H,-1));
    }
    
    // helper to set the size
    public void setSize(Dimension d) {
        if (size.equals(d)) return;
        size = d;
        displayPS.setPropertyValue(DisplayPropertySource.PROP_W, d.width);
        displayPS.setPropertyValue(DisplayPropertySource.PROP_H, d.height);
        setDisplay(displayPS.getValue());
    }

    // helper to get the location
    public Point getLocation() {
        Point constraint = location.getCopy();
        Integer x = displayPS.getIntegerProperty(DisplayPropertySource.PROP_X);
        Integer y = displayPS.getIntegerProperty(DisplayPropertySource.PROP_Y);
        if (x != null) constraint.x = x.intValue();
        if (y != null) constraint.y = y.intValue();
        return constraint;
    }

    // helper to set the location
    public void setLocation(Point p) {
        if (location.equals(p)) return;
        location = p;
        displayPS.setPropertyValue(DisplayPropertySource.PROP_X, p.x);
        displayPS.setPropertyValue(DisplayPropertySource.PROP_Y, p.y);
        setDisplay(displayPS.getValue());
    }

}
