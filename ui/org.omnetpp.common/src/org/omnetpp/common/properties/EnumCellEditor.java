/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;

public class EnumCellEditor extends ComboBoxCellEditor {

    Object[] values;

    public EnumCellEditor(Composite parent, String[] names, Object[] values) {
        super(parent, names, SWT.READ_ONLY);
        this.values = values;
    }

    @Override
    protected Object doGetValue() {
        int index = (Integer)super.doGetValue();
        return 0 <= index && index < values.length ? values[index] : null;
    }

    @Override
    protected void doSetValue(Object value) {
        for (int i = 0; i < values.length; ++i)
            if (value == null && values[i] == null ||
                value != null && value.equals(values[i])) {
                super.doSetValue(i);
                return;
            }
    }
}
