package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class DatasetPanel extends TreeWithButtonsPanel {
	
	private Button addButton;
	private Button removeButton;
	private Button editButton;
	private Button groupButton;
	private Button ungroupButton;
	//private Button createChartButton;
	private Button openChartButton;
	
	public DatasetPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel(); 
		FormToolkit toolkit = getFormToolkit();
		addButton = toolkit.createButton(buttonPanel, "Add...", SWT.NONE);
		removeButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
		editButton = toolkit.createButton(buttonPanel, "Edit...", SWT.NONE);
		groupButton = toolkit.createButton(buttonPanel, "Group", SWT.NONE);
		ungroupButton = toolkit.createButton(buttonPanel, "Ungroup", SWT.NONE);
		//createChartButton = toolkit.createButton(buttonPanel, "Create chart...", SWT.NONE);
		openChartButton = toolkit.createButton(buttonPanel, "Open chart", SWT.NONE);
	}

	public Button getAddButton() {
		return addButton;
	}

	public Button getRemoveButton() {
		return removeButton;
	}

	public Button getEditButton() {
		return editButton;
	}

//	public Button getCreateChartButton() {
//		return createChartButton;
//	}
	
	public Button getOpenChartButton() {
		return openChartButton;
	}

	public Button getGroupButton() {
		return groupButton;
	}

	public Button getUngroupButton() {
		return ungroupButton;
	}
}
