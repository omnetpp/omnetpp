package org.omnetpp.scave2.editors.datatable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave2.editors.ui.FilterParamsPanel;
import org.omnetpp.scave2.model.FilterParams;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {
	
	private Label filterLabel;
	private Button showHideButton;
	private FilterParamsPanel leftPanel;

	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public CCombo getFileNameCombo() {
		return leftPanel.getFileCombo();
	}
	
	public CCombo getRunNameCombo() {
		return leftPanel.getRunCombo();
	}

	public CCombo getExperimentNameCombo() {
		return leftPanel.getExperimentCombo();
	}
	
	public CCombo getMeasurementNameCombo() {
		return leftPanel.getMeasurementCombo();
	}

	public CCombo getReplicationNameCombo() {
		return leftPanel.getReplicationCombo();
	}

	public CCombo getModuleNameCombo() {
		return leftPanel.getModuleCombo();
	}

	public CCombo getNameCombo() {
		return leftPanel.getDataCombo();
	}

	public Button getFilterButton() {
		return filterButton;
	}
	
	public void setFilterText(String text) {
		filterLabel.setText("Filter: " + text);
	}
	
	public FilterParams getFilterParams() {
		return leftPanel.getFilterParams();
	}
	
	public void setFilterParams(FilterParams params) {
		leftPanel.setFilterParams(params);
	}

	private void initialize() {
		this.setLayout(new GridLayout(2, false));
		
		filterLabel = new Label(this, SWT.WRAP);
		filterLabel.setText("Filter: ");
		filterLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		showHideButton = new Button(this, SWT.PUSH);
		showHideButton.setText("Change");
		
		final Composite filterParamsPanel = new Composite(this, SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.exclude = true;
		filterParamsPanel.setVisible(false);
		filterParamsPanel.setLayoutData(gridData);
		filterParamsPanel.setLayout(new GridLayout(2, false));
		
		leftPanel = new FilterParamsPanel(filterParamsPanel, SWT.NONE);
		leftPanel.setLayout(new GridLayout());
		leftPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		filterButton = new Button(filterParamsPanel, SWT.NONE);
		filterButton.setText("Filter");
		filterButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
	
		showHideButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (filterParamsPanel.isVisible()) {
					filterParamsPanel.setVisible(false);
					((GridData)filterParamsPanel.getLayoutData()).exclude = true;
					showHideButton.setText("Change");
				}
				else {
					filterParamsPanel.setVisible(true);
					((GridData)filterParamsPanel.getLayoutData()).exclude = false;
					showHideButton.setText("Hide");
				}
				
				filterParamsPanel.getParent().getParent().layout(true, true);
			}
		});
	}
}
