package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
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
		Label label = getLabel();
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		label.setText("Add or drag&drop output files that should by used in this analysis.");
		addFileButton = toolkit.createButton(buttonPanel, "Add...", SWT.NONE);
		addWildcardButton = toolkit.createButton(buttonPanel, "Add wildcard...", SWT.NONE);
		removeFileButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}
}
