package org.omnetpp.common.properties;

import org.eclipse.jface.util.Assert;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * @author rhornig
 * A boolean property editor using an SWT checkbox as the editor
 */
public class CheckboxControlCellEditor extends CellEditor {

    Button chkBtn;
    
    public CheckboxControlCellEditor(Composite parent) {
        super(parent);
    }

    @Override
    protected Control createControl(Composite parent) {
        chkBtn = new Button(parent, SWT.CHECK);
        return chkBtn;
    }

    @Override
    protected Object doGetValue() {
        return chkBtn.getSelection();
    }

    @Override
    protected void doSetFocus() {
        chkBtn.setFocus();
    }

    @Override
    protected void doSetValue(Object value) {
        Assert.isTrue(value != null && (value instanceof Boolean));
        chkBtn.setSelection(((Boolean)value));
    }

}
