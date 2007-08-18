package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.widgets.Composite;

public class EditableComboBoxCellEditor extends ComboBoxCellEditor {

    public EditableComboBoxCellEditor() {
        super();
        setStyle(SWT.DROP_DOWN);
    }

    public EditableComboBoxCellEditor(Composite parent, String[] values) {
        super(parent, values, SWT.DROP_DOWN);
     }

    public EditableComboBoxCellEditor(Composite parent, String[] items, int style) {
        super(parent, items, style);
    }

    // we have to use the content of the editor as value (not the selection index)
    @Override
    protected Object doGetValue() {
        return ((CCombo)getControl()).getText();
    }

    @Override
    protected void doSetValue(Object value) {
        String newValue = (value == null) ? "" : value.toString();
        if (getControl() != null)
            ((CCombo)getControl()).setText(newValue);
    }

}
