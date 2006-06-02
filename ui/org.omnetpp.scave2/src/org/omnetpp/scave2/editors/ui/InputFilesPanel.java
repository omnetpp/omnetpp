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
	}

	public Button getAddFileButton() {
		return addFileButton;
	}

	public Button getAddWildcardButton() {
		return addWildcardButton;
	}

	public Button getRemoveFileButton() {
		return removeFileButton;
	}
}
