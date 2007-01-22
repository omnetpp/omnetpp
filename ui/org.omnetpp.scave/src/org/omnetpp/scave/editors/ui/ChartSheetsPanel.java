package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class ChartSheetsPanel extends TreeWithButtonsPanel {
	
	private Button openChartSheetButton;
	private Button newChildButton;
	private Button newSiblingButton;
	private Button editChartSheetButton;
	private Button removeChartSheetButton;
	
	public ChartSheetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		openChartSheetButton = toolkit.createButton(buttonPanel, "Open Chart Sheet", SWT.NONE);
		newChildButton = toolkit.createButton(buttonPanel, "New child...", SWT.NONE);
		newSiblingButton = toolkit.createButton(buttonPanel, "New sibling...", SWT.NONE);
		editChartSheetButton = toolkit.createButton(buttonPanel, "Edit...", SWT.NONE);
		removeChartSheetButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}

	public Button getEditChartSheetButton() {
		return editChartSheetButton;
	}

	public Button getNewChildButton() {
		return newChildButton;
	}
	
	public Button getNewSiblingButton() {
		return newSiblingButton;
	}

	public Button getOpenChartSheetButton() {
		return openChartSheetButton;
	}

	public Button getRemoveChartSheetButton() {
		return removeChartSheetButton;
	}
}
