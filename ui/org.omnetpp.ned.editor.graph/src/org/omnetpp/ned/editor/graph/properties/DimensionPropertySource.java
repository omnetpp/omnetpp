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
package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.ui.views.properties.*;
import org.omnetpp.ned.editor.graph.misc.NumberCellEditorValidator;

public class DimensionPropertySource implements IPropertySource {

    public static String PROP_WIDTH = "width"; //$NON-NLS-1$
    public static String PROP_HEIGHT = "height";//$NON-NLS-1$
    protected static IPropertyDescriptor[] descriptors;

    static {
        PropertyDescriptor widthProp = new TextPropertyDescriptor(PROP_WIDTH, "Width");
        widthProp.setValidator(NumberCellEditorValidator.instance());
        PropertyDescriptor heightProp = new TextPropertyDescriptor(PROP_HEIGHT, "Height");
        heightProp.setValidator(NumberCellEditorValidator.instance());
        descriptors = new IPropertyDescriptor[] { widthProp, heightProp };
    }

    protected Dimension dimension = null;

    public DimensionPropertySource(Dimension dimension) {
        this.dimension = dimension.getCopy();
    }

    public Object getEditableValue() {
        return dimension.getCopy();
    }

    public Object getPropertyValue(Object propName) {
        return getPropertyValue((String) propName);
    }

    public Object getPropertyValue(String propName) {
        if (PROP_HEIGHT.equals(propName)) { return new String(new Integer(dimension.height).toString()); }
        if (PROP_WIDTH.equals(propName)) { return new String(new Integer(dimension.width).toString()); }
        return null;
    }

    public void setPropertyValue(Object propName, Object value) {
        setPropertyValue((String) propName, value);
    }

    public void setPropertyValue(String propName, Object value) {
        if (PROP_HEIGHT.equals(propName)) {
            Integer newInt = new Integer((String) value);
            dimension.height = newInt.intValue();
        }
        if (PROP_WIDTH.equals(propName)) {
            Integer newInt = new Integer((String) value);
            dimension.width = newInt.intValue();
        }
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    public void resetPropertyValue(String propName) {
    }

    public void resetPropertyValue(Object propName) {
    }

    public boolean isPropertySet(Object propName) {
        return true;
    }

    public boolean isPropertySet(String propName) {
        if (PROP_HEIGHT.equals(propName) || PROP_WIDTH.equals(propName)) return true;
        return false;
    }

    public String toString() {
        return new String("(" + dimension.width + "," + dimension.height + ")");//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
    }

}