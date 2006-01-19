/*******************************************************************************
 * Copyright (c) 2000, 2003 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.text.parser;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;

/**
 * This class encapsulates property sheet properties
 * for NedElement.  This will display properties for
 * the NedElement when selected in the readme editor's
 * content outline.
 */
public class NedElementProperties implements IPropertySource {
    protected NedElement element;

    protected static final String PROPERTY_LINECOUNT = "lineno"; //$NON-NLS-1$

    protected static final String PROPERTY_START = "start"; //$NON-NLS-1$

    protected static final String PROPERTY_LENGTH = "length"; //$NON-NLS-1$

    /**
     * Creates a new NedElementProperties.
     *
     * @param element  the element whose properties this instance represents
     */
    public NedElementProperties(NedElement element) {
        super();
        this.element = element;
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public Object getEditableValue() {
        return this;
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public IPropertyDescriptor[] getPropertyDescriptors() {
        // Create the property vector.
        IPropertyDescriptor[] propertyDescriptors = new IPropertyDescriptor[3];

        // Add each property supported.
        PropertyDescriptor descriptor;

        descriptor = new PropertyDescriptor(PROPERTY_LINECOUNT, "Line_count"); //$NON-NLS-1$
        propertyDescriptors[0] = descriptor;
        descriptor = new PropertyDescriptor(PROPERTY_START, "Title_start"); //$NON-NLS-1$
        propertyDescriptors[1] = descriptor;
        descriptor = new PropertyDescriptor(PROPERTY_LENGTH, "Title_length"); //$NON-NLS-1$
        propertyDescriptors[2] = descriptor;

        // Return it.
        return propertyDescriptors;
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public Object getPropertyValue(Object name) {
        if (name.equals(PROPERTY_LINECOUNT))
            return new Integer(element.getNumberOfLines());
        if (name.equals(PROPERTY_START))
            return new Integer(element.getStart());
        if (name.equals(PROPERTY_LENGTH))
            return new Integer(element.getLength());
        return null;
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public boolean isPropertySet(Object property) {
        return false;
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public void resetPropertyValue(Object property) {
        // do nothing
    }

    /* (non-Javadoc)
     * Method declared on IPropertySource
     */
    public void setPropertyValue(Object name, Object value) {
        // do nothing
    }
}