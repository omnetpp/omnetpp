/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

/**
 * CheckboxTable control with Add, Edit and Remove buttons for editing a list of strings.
 * The labels, Add/Edit dialog titles and messages, input validation etc are all customizable.
 *
 * @author Levy
 */
public class EditableCheckboxList extends AbstractEditableList {
    /**
     * Creates the compound control. Style refers to the composite that contains all controls of the widget.
     */
    public EditableCheckboxList(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected Table createTable() {
       return new Table(this, SWT.CHECK | SWT.MULTI | SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL);
    }

    @Override
    public void add(String value) {
        super.add(value);
        setChecked(value, true);
    }

    public boolean getChecked(String value) {
        TableItem item = find(value);
        return item==null ? false : item.getChecked();
    }

    public void setChecked(String value, boolean checked) {
        TableItem item = find(value);
        if (item != null)
            item.setChecked(checked);
    }
}
