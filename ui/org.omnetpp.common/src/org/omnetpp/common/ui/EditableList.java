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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.List;

/**
 * A compound control which contains a listbox and Add/Remove buttons
 * @author Levy
 */
//TODO needs an Edit button as well, potentially also Move Up/Down buttons; see also CDT's FileControlList
public class EditableList extends Composite {
    protected String addDialogTitle = "Add element";
    protected String addDialogMessage = "Please enter the data";
    protected IInputValidator inputValidator;
    protected boolean emptyStringAllowed = false;
    protected boolean forceUnique = true;
    protected List list;
    protected Button add;
    protected Button remove;

	public EditableList(Composite parent, int style) {
		super(parent, style);
		GridLayout gridLayout = new GridLayout(2, false);
		gridLayout.marginWidth = 0;
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		list = new List(this, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL | SWT.H_SCROLL);
		list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 2));

		add = new Button(this, SWT.NONE);
		add.setText("Add");
		add.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		add.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				onAddButton();
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

    @Override
    public void setEnabled(boolean enabled) {
        list.setEnabled(enabled);
        add.setEnabled(enabled);
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
        InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(), addDialogTitle, addDialogMessage, "", inputValidator);
        if (dialog.open() == Window.OK)
            list.add(dialog.getValue());
    }

    /**
     * Override if you need different validation of input strings.
     */
    protected String isValidNewItem(String newText) {
        newText = newText.trim();
        if (!emptyStringAllowed && newText.equals(""))
            return "Text cannot be empty";
        if (forceUnique && Arrays.asList(list.getItems()).contains(newText))
            return "Item is already in the list";
        return null;
    }

    protected void onRemoveButton() {
        list.remove(list.getSelectionIndices());
    }

    public IInputValidator getInputValidator() {
        return inputValidator;
    }

    public void setInputValidator(IInputValidator inputValidator) {
        this.inputValidator = inputValidator;
    }

	public List getList() {
		return list;
	}

    public void setItemsAsList(java.util.List<String> strings) {
        list.removeAll();
        for (String s : strings)
            list.add(s);
    }

    public java.util.List<String> getItemsAsList() {
	    return Arrays.asList(list.getItems());
    }


}
