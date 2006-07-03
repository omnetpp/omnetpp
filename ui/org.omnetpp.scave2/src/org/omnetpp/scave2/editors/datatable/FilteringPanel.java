package org.omnetpp.scave2.editors.datatable;

import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {
	
	private Label filterLabel;
	private Button showHideButton;
	
	private CCombo fileNameCombo;
	private CCombo runNameCombo;
	
	private CCombo moduleNameCombo;
	private CCombo scalarNameCombo;
	
	private CCombo experimentNameCombo;
	private CCombo measurementNameCombo;
	private CCombo replicationNameCombo;
	
	private Button filterButton;

	public FilteringPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public CCombo getFileNameCombo() {
		return fileNameCombo;
	}
	
	public CCombo getRunNameCombo() {
		return runNameCombo;
	}

	public CCombo getExperimentNameCombo() {
		return experimentNameCombo;
	}
	
	public CCombo getMeasurementNameCombo() {
		return measurementNameCombo;
	}

	public CCombo getReplicationNameCombo() {
		return replicationNameCombo;
	}

	public CCombo getModuleNameCombo() {
		return moduleNameCombo;
	}

	public CCombo getNameCombo() {
		return scalarNameCombo;
	}

	public Button getFilterButton() {
		return filterButton;
	}
	
	public void setFilterText(String text) {
		filterLabel.setText("Filter: " + text);
	}

	private void initialize() {
		this.setLayout(new GridLayout(2, false));
		
		filterLabel = new Label(this, SWT.WRAP);
		filterLabel.setText("Filter: ");
		filterLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		showHideButton = new Button(this, SWT.PUSH);
		showHideButton.setText("Change");
		
		final Composite filterParamsPanel = new Composite(this, SWT.NONE);
		filterParamsPanel.setVisible(false);
		filterParamsPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		filterParamsPanel.setLayout(new GridLayout(2, false));
		((GridData)filterParamsPanel.getLayoutData()).heightHint = 0;
		
		
		Composite leftPanel = new Composite(filterParamsPanel, SWT.NONE);
		leftPanel.setLayout(new GridLayout());
		leftPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		Composite group = createFilterGroup(leftPanel, 2);
		fileNameCombo = createLabelAndCombo(group, "File name:", "Filter displayed items by file. Wildcards *,? are OK.");
		runNameCombo = createLabelAndCombo(group, "Run name:", "Filter displayed items by run name. Wildcards *,? are OK.");
		
		group = createFilterGroup(leftPanel, 3);
		experimentNameCombo = createLabelAndCombo(group, "Experiment name:", "Filter displayed items by experiment name. Wildcards *,? are OK.");
		measurementNameCombo = createLabelAndCombo(group, "Measurement name:", "Filter displayed items by measurement name. Wildcards *,? are OK.");
		replicationNameCombo = createLabelAndCombo(group, "Replication name:", "Filter displayed items by replication name. Wildcards *,? are OK.");
		
		group = createFilterGroup(leftPanel, 2);
		moduleNameCombo = createLabelAndCombo(group, "Module name:", "Filter displayed items by module name. Wildcards *,? are OK.");
		scalarNameCombo = createLabelAndCombo(group, "Scalar name:", "Filter displayed items by scalar name. Wildcards *,? are OK.");
		
		filterButton = new Button(filterParamsPanel, SWT.NONE);
		filterButton.setText("Filter");
		filterButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
	
		showHideButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (filterParamsPanel.isVisible()) {
					filterParamsPanel.setVisible(false);
					showHideButton.setText("Change");
					((GridData)filterParamsPanel.getLayoutData()).heightHint = 0;
				}
				else {
					filterParamsPanel.setVisible(true);
					showHideButton.setText("Hide");
					((GridData)filterParamsPanel.getLayoutData()).heightHint = SWT.DEFAULT;
				}
				
				filterParamsPanel.getParent().getParent().layout(true, true);
			}
		});
	}
	
	private Composite createFilterGroup(Composite parent, int numOfFilterParams) {
		Composite group = new Composite(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		group.setLayout(new GridLayout(2 * numOfFilterParams, false));
		return group;
	}
	
	private CCombo createLabelAndCombo(Composite parent, String labelText, String tooltipText) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
		CCombo combo = new CCombo(parent, SWT.BORDER);
		combo.setVisibleItemCount(10);
		combo.setToolTipText(tooltipText);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		return combo;
	}
}
