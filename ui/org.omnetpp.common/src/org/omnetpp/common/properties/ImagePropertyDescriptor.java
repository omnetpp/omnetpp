/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public class ImagePropertyDescriptor extends PropertyDescriptor {

    final IProject project; // scope of the image search

    public ImagePropertyDescriptor(Object id, String displayName, IProject project) {
        super(id, displayName);
        this.project = project;
    }

    public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new ImageCellEditor(parent, project);
        if (getValidator() != null)
            editor.setValidator(getValidator());
        return editor;
    }
}

