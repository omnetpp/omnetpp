package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class DatasetPanel extends TreeWithButtonsPanel {
	
	private Button addButton;
	private Button removeButton;
	private Button groupButton;
	private Button ungroupButton;
	private Button createChartButton;
	
	public DatasetPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Label label = getLabel();
		Composite buttonPanel = getButtonPanel(); 
		FormToolkit toolkit = getFormToolkit();
		label.setText("Here you can edit the dataset.");
		addButton = toolkit.createButton(buttonPanel, "Add...", SWT.NONE);
		removeButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
		groupButton = toolkit.createButton(buttonPanel, "Group", SWT.NONE);
		ungroupButton = toolkit.createButton(buttonPanel, "Ungroup", SWT.NONE);
		createChartButton = toolkit.createButton(buttonPanel, "Create chart...", SWT.NONE);
	}
}
