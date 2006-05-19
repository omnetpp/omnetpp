package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class ChartSheetsPanel extends TreeWithButtonsPanel {
	
	private Button openChartSheetButton;
	private Button newChartSheetButton;
	private Button removeChartSheetButton;
	
	public ChartSheetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		openChartSheetButton = toolkit.createButton(buttonPanel, "Open",	SWT.NONE);
		newChartSheetButton = toolkit.createButton(buttonPanel, "New...", SWT.NONE);
		removeChartSheetButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}
}
