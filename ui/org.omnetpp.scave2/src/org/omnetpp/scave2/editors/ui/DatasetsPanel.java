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
	private Button newDatasetButton;
	private Button removeDatasetButton;
	
	public DatasetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		openDatasetButton = toolkit.createButton(buttonPanel, "Open", SWT.NONE);
		openDatasetButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				new Action() {
					public void run() {
						OpenDatasetActionDelegate delegate = new OpenDatasetActionDelegate();
						delegate.run(this);
					}
				}.run();
			}
		});
		newDatasetButton = toolkit.createButton(buttonPanel, "New...", SWT.NONE);
		removeDatasetButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}
}
