package org.omnetpp.common.ui;

import java.util.Arrays;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
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

/**
 * A compound control which contains a list and Add/Remove/Edit buttons
 * @author Levy
 */
//TODO potentially needs Move Up/Down buttons; see also CDT's FileControlList
public abstract class AbstractEditableList extends Composite {
    protected String addDialogTitle = "Add element";
    protected String addDialogMessage = "Please enter the data";
    protected IInputValidator inputValidator;
    protected boolean emptyStringAllowed = false;
    protected boolean forceUnique = true;
    protected Button add;
    protected Button edit;
    protected Button remove;
    
	public AbstractEditableList(Composite parent, int style) {
		super(parent, style);
		GridLayout gridLayout = new GridLayout(2, false);
		gridLayout.marginWidth = 0;
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		Control control = createContent();
        control.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 3));

		add = new Button(this, SWT.NONE);
		add.setText("Add");
		add.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		add.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				onAddButton();
			}
		});

        edit = new Button(this, SWT.NONE);
        edit.setText("Edit");
        edit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
        edit.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                onEditButton();
            }
        });

		remove = new Button(this, SWT.NONE);
		remove.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		remove.setText("Remove");
		remove.addSelectionListener(new SelectionAdapter() {
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

	}

    public abstract void add(String value);

    public abstract void set(int i, String value);

    public abstract String[] getItems();

    public abstract void setItems(String[] items);

    public abstract int[] getSelectionIndices();

    public abstract void removeAll();

    public abstract void remove(int[] selectionIndices);

    protected abstract Control createContent();

    @Override
    public void setEnabled(boolean enabled) {
        add.setEnabled(enabled);
        edit.setEnabled(enabled);
        remove.setEnabled(enabled);
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

	/**
	 * Override if you need a different Add dialog.
	 */
    protected void onAddButton() {
        InputDialog dialog = getInputDialog("");

        if (dialog.open() == Window.OK)
            add(dialog.getValue());
    }

    protected void onEditButton() {
        int[] indices = getSelectionIndices();
        
        if (indices.length == 1) {
            int index = indices[0];
            InputDialog dialog = getInputDialog(getItems()[index]);
    
            if (dialog.open() == Window.OK)
                set(index, dialog.getValue());
        }
    }

    protected InputDialog getInputDialog(String value) {
        return new InputDialog(Display.getCurrent().getActiveShell(), addDialogTitle, addDialogMessage, value, inputValidator);
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

    protected void onRemoveButton() {
        remove(getSelectionIndices());
    }

    public IInputValidator getInputValidator() {
        return inputValidator;
    }

    public void setInputValidator(IInputValidator inputValidator) {
        this.inputValidator = inputValidator;
    }

    public void setItemsAsList(java.util.List<String> strings) {
        removeAll();
        for (String s : strings)
            add(s);
    }

    public java.util.List<String> getItemsAsList() {
	    return Arrays.asList(getItems());
    }
}
