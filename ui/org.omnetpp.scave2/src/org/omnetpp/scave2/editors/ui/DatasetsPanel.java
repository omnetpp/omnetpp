package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.action.Action;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.omnetpp.scave2.actions.OpenDatasetActionDelegate;

public class DatasetsPanel extends TreeWithButtonsPanel {
	
	private Button openDatasetButton;
	private Button newNodeButton;
	private Button editNodeButton;
	private Button removeNodeButton;
	
	public DatasetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		openDatasetButton = toolkit.createButton(buttonPanel, "Open Dataset", SWT.NONE);
		newNodeButton = toolkit.createButton(buttonPanel, "New...", SWT.NONE);
		editNodeButton = toolkit.createButton(buttonPanel, "Edit...", SWT.NONE);
		removeNodeButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}

	public Button getEditNodeButton() {
		return editNodeButton;
	}

	public Button getNewNodeButton() {
		return newNodeButton;
	}

	public Button getOpenDatasetButton() {
		return openDatasetButton;
	}

	public Button getRemoveNodeButton() {
		return removeNodeButton;
	}
}
