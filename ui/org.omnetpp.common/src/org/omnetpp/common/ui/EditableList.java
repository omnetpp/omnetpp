package org.omnetpp.common.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.List;

public class EditableList extends AbstractEditableList {
    private List list;

    public EditableList(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected Control createContent() {
        list = new List(this, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL);
        return list;
    }
    
    @Override
    public void setEnabled(boolean enabled) {
        list.setEnabled(enabled);
        super.setEnabled(enabled);
    }

    @Override
    public void add(String value) {
        list.add(value);
    }

    @Override
    public void set(int index, String value) {
        list.setItem(index, value);
    }

    @Override
    public String[] getItems() {
        return list.getItems();
    }

    @Override
    public void setItems(String[] values) {
        list.setItems(values);
    }

    @Override
    public int[] getSelectionIndices() {
        return list.getSelectionIndices();
    }

    @Override
    public void removeAll() {
        list.removeAll();
    }

    @Override
    public void remove(int[] selectionIndices) {
        list.remove(selectionIndices);
    }
}
