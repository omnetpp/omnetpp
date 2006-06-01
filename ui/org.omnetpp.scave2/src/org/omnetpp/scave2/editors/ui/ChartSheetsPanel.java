package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.action.Action;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.omnetpp.scave2.actions.OpenChartSheetActionDelegate;

public class ChartSheetsPanel extends TreeWithButtonsPanel {
	
	private Button openChartSheetButton;
	private Button newChartSheetButton;
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
		newChartSheetButton = toolkit.createButton(buttonPanel, "New...", SWT.NONE);
		editChartSheetButton = toolkit.createButton(buttonPanel, "Edit...", SWT.NONE);
		removeChartSheetButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
		
		openChartSheetButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				OpenChartSheetActionDelegate delegate = new OpenChartSheetActionDelegate();
				delegate.run(null); //XXX
			}
		});
		
	}
}
