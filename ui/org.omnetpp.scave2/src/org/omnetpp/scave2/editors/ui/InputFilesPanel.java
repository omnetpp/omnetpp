package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

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
