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

import java.util.Vector;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.model.IWorkbenchAdapter;
import org.eclipse.ui.views.properties.IPropertySource;

/**
 * This class represents a marked location in the Readme
 * file text.  
 *
 * TIP: By implementing the <code>IWorkbenchAdapter</code> interface,
 * we can easily add objects of this type to viewers and parts in
 * the workbench.  When a viewer contains <code>IWorkbenchAdapter</code>,
 * the generic <code>WorkbenchContentProvider</code> and
 * <code>WorkbenchLabelProvider</code> can be used to provide
 * navigation and display for that viewer.
 */
public class NedElement implements IWorkbenchAdapter, IAdaptable {
    private String headingName;

    private IAdaptable parent;

    private int offset;

    private int numberOfLines;

    private int length;

    private Vector children;

    /**
     * Creates a new NedElement and stores parent element and
     * location in the text.
     *
     * @param parent  the parent of this element
     * @param heading text corresponding to the heading
     * @param offset  the offset into the Readme text
     * @param length  the length of the element
     */
    public NedElement(IAdaptable parent, String heading, int offset, int length) {
        this.parent = parent;
        if (parent instanceof NedElement) {
            ((NedElement) parent).addChild(this);
        }
        this.headingName = heading;
        this.offset = offset;
        this.length = length;
    }

    /**
     * Adds a child to this element
     */
    private void addChild(NedElement child) {
        if (children == null) {
            children = new Vector();
        }
        children.add(child);
    }

    /* (non-Javadoc)
     * Method declared on IAdaptable
     */
    public Object getAdapter(Class adapter) {
        if (adapter == IWorkbenchAdapter.class)
            return this;
        if (adapter == IPropertySource.class)
            return new NedElementProperties(this);
        return null;
    }

    /* (non-Javadoc)
     * Method declared on IWorkbenchAdapter
     */
    public Object[] getChildren(Object object) {
        if (children != null) {
            return children.toArray();
        }
        return new Object[0];
    }

    /* (non-Javadoc)
     * Method declared on IWorkbenchAdapter
     */
    public ImageDescriptor getImageDescriptor(Object object) {
        if (parent == null) return null;
        IWorkbenchAdapter parentElement = (IWorkbenchAdapter) parent
                .getAdapter(IWorkbenchAdapter.class);
        if (parentElement != null) {
            return parentElement.getImageDescriptor(object);
        }
        return null;
    }

    /* (non-Javadoc)
     * Method declared on IWorkbenchAdapter
     */
    public String getLabel(Object o) {
        return headingName;
    }

    /**
     * Returns the number of characters in this section.
     */
    public int getLength() {
        return length;
    }

    /**
     * Returns the number of lines in the element.
     *
     * @return the number of lines in the element
     */
    public int getNumberOfLines() {
        return numberOfLines;
    }

    /* (non-Javadoc)
     * Method declared on IWorkbenchAdapter
     */
    public Object getParent(Object o) {
        return null;
    }

    /**
     * Returns the offset of this section in the file.
     */
    public int getStart() {
        return offset;
    }

    /**
     * Sets the number of lines in the element
     *
     * @param newNumberOfLines  the number of lines in the element
     */
    public void setNumberOfLines(int newNumberOfLines) {
        numberOfLines = newNumberOfLines;
    }
}