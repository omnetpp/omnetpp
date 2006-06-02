package org.omnetpp.scave2.editors.ui;

import java.util.ArrayList;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.CreateChildCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ScaveModelFactory;

public class InputFilesPanel extends TreeWithButtonsPanel {
	
	Button addFileButton;
	Button addWildcardButton;
	Button removeFileButton;
	
	public InputFilesPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		addFileButton = toolkit.createButton(buttonPanel, "Add...", SWT.NONE);
		addWildcardButton = toolkit.createButton(buttonPanel, "Add wildcard...", SWT.NONE);
		removeFileButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
		
		removeFileButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				// use the EMF.Edit Framework's command interface to do the job (undoable)
//XXX we'll need a reference to scaveEditor for this to work
//XXX assert that the selection is within Inputs? otherwise it'll be willing to delete datasets, charts etc if they are selected
//				InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
//				Command command = RemoveCommand.create(scaveEditor.getEditingDomain(), scaveEditor.getSelection());
//				scaveEditor.getEditingDomain().getCommandStack().execute(command);
			}
		});
	}
}
