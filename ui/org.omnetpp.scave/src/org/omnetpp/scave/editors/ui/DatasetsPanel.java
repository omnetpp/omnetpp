package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class DatasetsPanel extends TreeWithButtonsPanel {
	
	private Button openDatasetButton;
	private Button newChildButton;
	private Button newSiblingButton;
	private Button editNodeButton;
	private Button removeNodeButton;
	
	public DatasetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
//		Composite buttonPanel = getButtonPanel();
//		FormToolkit toolkit = getFormToolkit();
//		openDatasetButton = toolkit.createButton(buttonPanel, "Open Dataset", SWT.NONE);
//		newChildButton = toolkit.createButton(buttonPanel, "New child...", SWT.NONE);
//		newSiblingButton = toolkit.createButton(buttonPanel, "New sibling...", SWT.NONE);
//		editNodeButton = toolkit.createButton(buttonPanel, "Edit...", SWT.NONE);
//		removeNodeButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}

	public Button getEditNodeButton() {
		return editNodeButton;
	}

	public Button getNewChildButton() {
		return newChildButton;
	}

	public Button getNewSiblingButton() {
		return newSiblingButton;
	}

	public Button getOpenDatasetButton() {
		return openDatasetButton;
	}

	public Button getRemoveNodeButton() {
		return removeNodeButton;
	}
}
