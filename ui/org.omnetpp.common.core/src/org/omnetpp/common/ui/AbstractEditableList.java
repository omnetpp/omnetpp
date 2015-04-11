/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

/**
 * A list control with Add, Edit and Remove buttons for editing list of strings.
 * The actual list control is created by a factory method. The labels, Add/Edit dialog
 * titles and messages, input validation etc are all customizable.
 * Selection and content changes can be listened to.
 *
 * @author Levy, Andras
 */
public abstract class AbstractEditableList extends Composite implements ISelectionProvider {
    protected String addDialogTitle = "Add";
    protected String addDialogMessage = "Enter value:";
    protected String editDialogTitle = "Edit";
    protected String editDialogMessage = "Edit value:";
    protected IInputValidator inputValidator;
    protected boolean emptyStringAllowed = false;
    protected boolean forceUnique = true;
    protected Table table;
    protected Button addButton;
    protected Button editButton;
    protected Button removeButton;
    protected ListenerList selectionChangedListeners = new ListenerList();

    /**
     * Creates the compound control. Style refers to the composite that contains all controls of the widget.
     */
    public AbstractEditableList(Composite parent, int style) {
        super(parent, style);
        GridLayout gridLayout = new GridLayout(2, false);
        gridLayout.marginWidth = 0;
        gridLayout.marginHeight = 0;
        setLayout(gridLayout);

        table = createTable();
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 3));
        table.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                onSelectionChange();
            }
            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                onEditButton();
            }
        });

        addButton = new Button(this, SWT.NONE);
        addButton.setText("Add...");
        addButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        addButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                onAddButton();
            }
        });

        editButton = new Button(this, SWT.NONE);
        editButton.setText("Edit...");
        editButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        editButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                onEditButton();
            }
        });

        removeButton = new Button(this, SWT.NONE);
        removeButton.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        removeButton.setText("Remove");
        removeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                onRemoveButton();
            }
        });

        inputValidator = new IInputValidator() {
            public String isValid(String newText) {
                return isValidNewItem(newText);
            }
        };

        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                onSelectionChange(); // by the time the list has already been filled with content
            }
        });

    }

    /**
     * Creates the table control.
     */
    protected abstract Table createTable();

    public Table getTable() {
        return table;
    }

    public String getAddDialogTitle() {
        return addDialogTitle;
    }

    public void setAddDialogTitle(String addDialogTitle) {
        this.addDialogTitle = addDialogTitle;
    }

    public String getAddDialogMessage() {
        return addDialogMessage;
    }

    public void setAddDialogMessage(String addDialogMessage) {
        this.addDialogMessage = addDialogMessage;
    }

    public String getEditDialogTitle() {
        return editDialogTitle;
    }

    public void setEditDialogTitle(String editDialogTitle) {
        this.editDialogTitle = editDialogTitle;
    }

    public String getEditDialogMessage() {
        return editDialogMessage;
    }

    public void setEditDialogMessage(String editDialogMessage) {
        this.editDialogMessage = editDialogMessage;
    }

    public boolean isEmptyStringAllowed() {
        return emptyStringAllowed;
    }

    public void setEmptyStringAllowed(boolean emptyStringAllowed) {
        this.emptyStringAllowed = emptyStringAllowed;
    }

    public boolean isForceUnique() {
        return forceUnique;
    }

    public void setForceUnique(boolean forceUnique) {
        this.forceUnique = forceUnique;
    }

    public IInputValidator getInputValidator() {
        return inputValidator;
    }

    public void setInputValidator(IInputValidator inputValidator) {
        this.inputValidator = inputValidator;
    }

    public void add(String value) {
        doAdd(value);
    }

    protected TableItem doAdd(String value) {
        TableItem item = new TableItem(table, SWT.NONE);
        item.setText(value);
        return item;
    }

    public void set(int index, String value) {
        table.getItem(index).setText(value);
    }

    public String[] getItems() {
        TableItem[] items = table.getItems();
        String[] values = new String[items.length];
        for (int i = 0; i < items.length; i++) {
            TableItem item = items[i];
            values[i] = item.getText();
        }
        return values;
    }

    public void setItems(String[] values) {
        table.removeAll();
        for (String value : values)
            doAdd(value);
    }

    public int[] getSelectionIndices() {
        return table.getSelectionIndices();
    }

    public void removeAll() {
        table.removeAll();
    }

    public void remove(int[] selectionIndices) {
        table.remove(selectionIndices);
    }

    protected TableItem find(String value) {
        for (TableItem item : table.getItems())
            if (item.getText().equals(value))
                return item;
        return null;
    }

    @Override
    public void setEnabled(boolean enabled) {
        table.setEnabled(enabled);
        addButton.setEnabled(enabled);
        editButton.setEnabled(enabled);
        removeButton.setEnabled(enabled);
        if (enabled)
            onSelectionChange(); // update button states
    }

    public ISelection getSelection() {
        TableItem[] selectedItems = table.getSelection();
        String[] strings = new String[selectedItems.length];
        for (int i=0; i<selectedItems.length; i++)
            strings[i] = selectedItems[i].getText();
        return new StructuredSelection(strings);
    }

    public void setSelection(ISelection selection) {
        List<TableItem> l = new ArrayList<TableItem>();
        for (Object e : ((IStructuredSelection)selection).toList()) {
            TableItem item = find((String)e);
            if (item != null)
                l.add(item);
        }
        table.setSelection(l.toArray(new TableItem[]{}));
    }


    /**
     * Adds a selection changed listener. It will be notified not only on selection changes
     * but also on content changes.
     */
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    /**
     * Removes the given selection changed listener.
     */
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    protected void fireSelectionChanged() {
        final SelectionChangedEvent event = new SelectionChangedEvent(this, getSelection());
        Object[] listeners = selectionChangedListeners.getListeners();
        for (int i = 0; i < listeners.length; ++i) {
            final ISelectionChangedListener l = (ISelectionChangedListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.selectionChanged(event);
                }
            });
        }
    }

    /**
     * Override for custom Add button behavior.
     */
    protected void onAddButton() {
        InputDialog dialog = createAddDialog("");

        if (dialog.open() == Window.OK) {
            TableItem item = doAdd(dialog.getValue());
            table.setSelection(item);
            onSelectionChange();
        }
    }

    /**
     * Override for custom Edit button behavior.
     */
    protected void onEditButton() {
        int[] indices = getSelectionIndices();

        if (indices.length == 1) {
            int index = indices[0];
            InputDialog dialog = createEditDialog(getItems()[index]);

            if (dialog.open() == Window.OK) {
                set(index, dialog.getValue());
                onSelectionChange();
            }
        }
    }

    /**
     * Override if you need a different Remove button behavior
     */
    protected void onRemoveButton() {
        remove(getSelectionIndices());
        onSelectionChange();  // table does not fire selectionChange when list becomes empty
    }

    /**
     * Fired not only on selection changes but also on content changes.
     */
    protected void onSelectionChange() {
        if (table.getEnabled()) {
            int numSelected = getSelectionIndices().length;
            setEnabled(editButton, numSelected == 1);
            setEnabled(removeButton, numSelected > 0);
        }
        fireSelectionChanged();
    }

    protected void setEnabled(Control control, boolean enabled) {
        if (control.getEnabled() != enabled)
            control.setEnabled(enabled);
    }

    /**
     * Override if you need a different Add dialog.
     */
    protected InputDialog createAddDialog(String defaultValue) {
        return new InputDialog(Display.getCurrent().getActiveShell(), getAddDialogTitle(), getAddDialogMessage(), defaultValue, inputValidator);
    }

    /**
     * Override if you need a different Edit dialog.
     */
    protected InputDialog createEditDialog(String value) {
        return new InputDialog(Display.getCurrent().getActiveShell(), getEditDialogTitle(), getEditDialogMessage(), value, inputValidator);
    }

    /**
     * Override if you need different validation of input strings.
     */
    protected String isValidNewItem(String newText) {
        newText = newText.trim();
        if (!emptyStringAllowed && newText.equals(""))
            return "Text cannot be empty";
        if (forceUnique && Arrays.asList(getItems()).contains(newText))
            return "Item is already in the list";
        return null;
    }

    /**
     * Replaces the list control contents with the given elements
     */
    public void setItemsAsList(List<String> strings) {
        removeAll();
        for (String s : strings)
            add(s);
    }

    /**
     * Returns the list control contents
     */
    public List<String> getItemsAsList() {
        return Arrays.asList(getItems());
    }
}
