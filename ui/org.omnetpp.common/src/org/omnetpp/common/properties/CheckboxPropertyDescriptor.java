/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public class CheckboxPropertyDescriptor extends PropertyDescriptor
{
    public CheckboxPropertyDescriptor(Object id, String displayName) {
        super(id, displayName);
    }

    public CellEditor createPropertyEditor(Composite parent) {
        CellEditor editor = new CheckboxControlCellEditor(parent);
        if (getValidator() != null)
            editor.setValidator(getValidator());
        return editor;
    }
}
