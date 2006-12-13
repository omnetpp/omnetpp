package org.omnetpp.scave2.editors.datatable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave2.editors.ui.FilterParamsPanel;
import org.omnetpp.scave2.model2.FilterHints;
import org.omnetpp.scave2.model2.FilterParams;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
// XXX layout of "Filter" label
public class FilteringPanel extends Composite {
	
	private Label filterLabel;
	private Button showHideButton;
	private FilterParamsPanel upperPanel;
	private FilterParamsPanel lowerPanel;

	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public CCombo getModuleNameCombo() {
		return upperPanel.getModuleCombo();
	}

	public CCombo getNameCombo() {
		return upperPanel.getDataCombo();
	}

	public CCombo getFileNameCombo() {
		return lowerPanel.getFileCombo();
	}
	
	public CCombo getRunNameCombo() {
		return lowerPanel.getRunCombo();
	}

	public CCombo getExperimentNameCombo() {
		return lowerPanel.getExperimentCombo();
	}
	
	public CCombo getMeasurementNameCombo() {
		return lowerPanel.getMeasurementCombo();
	}

	public CCombo getReplicationNameCombo() {
		return lowerPanel.getReplicationCombo();
	}

	public Button getFilterButton() {
		return filterButton;
	}
	
	public void setFilterText(String text) {
		filterLabel.setText("Filter: " + text);
	}
	
	public FilterParams getFilterParams() {
		FilterParams upperParams = upperPanel.getFilterParams();
		FilterParams lowerParams = lowerPanel.getFilterParams();
		return new FilterParams(
			lowerParams.getFileNamePattern(),
			lowerParams.getRunNamePattern(),
			lowerParams.getExperimentNamePattern(),
			lowerParams.getMeasurementNamePattern(),
			lowerParams.getReplicationNamePattern(),
			upperParams.getModuleNamePattern(),
			upperParams.getDataNamePattern());
	}
	
	public void setFilterParams(FilterParams params) {
		upperPanel.setFilterParams(params);
		lowerPanel.setFilterParams(params);
	}
	
	public void setFilterHints(FilterHints hints) {
		upperPanel.setFilterHints(hints);
		lowerPanel.setFilterHints(hints);
	}

	private void initialize() {
		GridLayout gridLayout;
		
		gridLayout = new GridLayout();
		gridLayout.marginHeight = 0;
		this.setLayout(gridLayout);
		
		Composite firstRow = new Composite(this, SWT.NONE);
		firstRow.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		gridLayout = new GridLayout(4, false);
		gridLayout.marginHeight = 0;
		firstRow.setLayout(gridLayout);
		
		filterLabel = new Label(firstRow, SWT.WRAP);
		filterLabel.setText("Filter: ");
		filterLabel.setLayoutData(new GridData(200, SWT.DEFAULT));
		
		upperPanel = new FilterParamsPanel(firstRow, SWT.NONE, FilterParamsPanel.MODULE_NAME_ROW);
		upperPanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		filterButton = new Button(firstRow, SWT.NONE);
		filterButton.setText("Filter");
		filterButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
	
		showHideButton = new Button(firstRow, SWT.PUSH);
		showHideButton.setText("More");
		showHideButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
		
		final Composite showHidePanel = new Composite(this, SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.exclude = true;
		showHidePanel.setVisible(false);
		showHidePanel.setLayoutData(gridData);
		showHidePanel.setLayout(new GridLayout(2, false));
		
		lowerPanel = new FilterParamsPanel(showHidePanel, SWT.NONE,
				FilterParamsPanel.FILE_RUN_ROW | FilterParamsPanel.EXPERIMENT_MEASUREMENT_REPLICATION_ROW);
		lowerPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		showHideButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (showHidePanel.isVisible()) {
					showHidePanel.setVisible(false);
					((GridData)showHidePanel.getLayoutData()).exclude = true;
					showHideButton.setText("More");
				}
				else {
					showHidePanel.setVisible(true);
					((GridData)showHidePanel.getLayoutData()).exclude = false;
					showHideButton.setText("Hide");
				}
				
				showHidePanel.getParent().getParent().layout(true, true);
			}
		});
	}
}
