package org.omnetpp.common.ui;

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

public class EditableList extends Composite {
	private List list;

	private Button add;
	
	private Button remove;

	public EditableList(Composite parent, int style) {
		super(parent, SWT.NONE);
		GridLayout gridLayout = new GridLayout(2, false);
		gridLayout.marginWidth = 0;
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		list = new List(this, style);
		list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 2));

		add = new Button(this, SWT.NONE);
		add.setText("Add");
		add.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		add.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				String addDialogTitle = "Add element";
				String addDialogMessage = "Please enter the data";
				InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(), addDialogTitle, addDialogMessage, "", null);

				if (dialog.open() == Window.OK) {
					list.add(dialog.getValue());
				}
			}
		});

		remove = new Button(this, SWT.NONE);
		remove.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		remove.setText("Remove");
		remove.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				list.remove(list.getSelectionIndices());
			}
		});
	}
	
	public List getList() {
		return list;
	}
	
	@Override
	public void setEnabled(boolean enabled) {
		list.setEnabled(enabled);
		add.setEnabled(enabled);
		remove.setEnabled(enabled);
	}
}
