package org.omnetpp.common.ui;

import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.TableItem;

public class EditableCheckboxList extends AbstractEditableList {
    private CheckboxTableViewer viewer;

    public EditableCheckboxList(Composite parent, int style) {
        super(parent, style);
    }

    @Override
    protected Control createContent() {
        viewer = CheckboxTableViewer.newCheckList(this, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL);
        return viewer.getTable();
    }

    @Override
    public void setEnabled(boolean enabled) {
        viewer.getTable().setEnabled(enabled);
        super.setEnabled(enabled);
    }

    @Override
    public void add(String value) {
        viewer.add(value);
        viewer.setChecked(value, true);
    }

    @Override
    public void set(int index, String value) {
        viewer.getTable().getItem(index).setText(value);
    }

    @Override
    public String[] getItems() {
        TableItem[] items = viewer.getTable().getItems();
        String[] values = new String[items.length];
        for (int i = 0; i < items.length; i++) {
            TableItem item = items[i];
            values[i] = item.getText();
        }

        return values;
    }

    @Override
    public void setItems(String[] values) {
        viewer.getTable().removeAll();
        viewer.add(values);
    }

    @Override
    public int[] getSelectionIndices() {
        return viewer.getTable().getSelectionIndices();
    }

    @Override
    public void removeAll() {
        viewer.getTable().removeAll();
    }

    @Override
    public void remove(int[] selectionIndices) {
        viewer.getTable().remove(selectionIndices);
    }

    public boolean getChecked(String item) {
        return viewer.getChecked(item);
    }

    public void setChecked(String item, boolean flag) {
        viewer.setChecked(item, flag);
    }
}
