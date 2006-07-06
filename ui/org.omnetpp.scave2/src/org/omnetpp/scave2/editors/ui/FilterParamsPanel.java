package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave2.model.FilterHints;
import org.omnetpp.scave2.model.FilterParams;

/**
 * A composite for displaying and editing the filter parameters.
 * It is a passive component, needs to be configured
 * to do anything useful.
 * 
 * It is used in the BrowseDataPage and in the CreateDatasetDialog.
 *
 * @author tomi
 */
public class FilterParamsPanel extends Composite {

	private Composite composite1 = null;
	private Label fileLabel = null;
	private Label runLabel = null;
	private Composite composite2 = null;
	private Label experimentLabel = null;
	private Label measurementLabel = null;
	private Label replicationLabel = null;
	private CCombo fileCombo = null;
	private CCombo runCombo = null;
	private CCombo experimentCombo = null;
	private CCombo measurementCombo = null;
	private Composite composite3 = null;
	private Label moduleLabel = null;
	private Label dataLabel = null;
	private CCombo replicationCombo = null;
	private CCombo moduleCombo = null;
	private CCombo dataCombo = null;
	
	public FilterParamsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public CCombo getDataCombo() {
		return dataCombo;
	}

	public CCombo getExperimentCombo() {
		return experimentCombo;
	}

	public CCombo getFileCombo() {
		return fileCombo;
	}

	public CCombo getMeasurementCombo() {
		return measurementCombo;
	}

	public CCombo getModuleCombo() {
		return moduleCombo;
	}

	public CCombo getReplicationCombo() {
		return replicationCombo;
	}

	public CCombo getRunCombo() {
		return runCombo;
	}

	public FilterParams getFilterParams() {
		return new FilterParams(
				fileCombo.getText(),
				runCombo.getText(),
				experimentCombo.getText(),
				measurementCombo.getText(),
				replicationCombo.getText(),
				moduleCombo.getText(),
				dataCombo.getText());
	}
	
	public void setFilterParams(FilterParams params) {
		fileCombo.setText(params.getFileNamePattern());
		runCombo.setText(params.getRunNamePattern());
		experimentCombo.setText(params.getExperimentNamePattern());
		measurementCombo.setText(params.getMeasurementNamePattern());
		replicationCombo.setText(params.getReplicationNamePattern());
		moduleCombo.setText(params.getModuleNamePattern());
		dataCombo.setText(params.getDataNamePattern());
	}
	
	public void setFilterHints(FilterHints hints) {
		fileCombo.setItems(hints.getFileNameHints());
		runCombo.setItems(hints.getRunNameHints());
		experimentCombo.setItems(hints.getExperimentNameHints());
		measurementCombo.setItems(hints.getMeasurementNameHints());
		replicationCombo.setItems(hints.getReplicationNameHints());
		moduleCombo.setItems(hints.getModuleNameHints());
		dataCombo.setItems(hints.getDataNameHints());
	}

	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.verticalSpacing = 5;
		gridLayout.marginHeight = 5;
		gridLayout.marginWidth = 5;
		gridLayout.horizontalSpacing = 0;
		this.setLayout(gridLayout);
		createComposite1();
		createComposite2();
		createComposite3();
	}

	/**
	 * This method initializes composite1	
	 *
	 */
	private void createComposite1() {
		GridData gridData21 = new GridData();
		gridData21.grabExcessHorizontalSpace = true;
		gridData21.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData11 = new GridData();
		gridData11.grabExcessHorizontalSpace = true;
		gridData11.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridLayout gridLayout2 = new GridLayout();
		gridLayout2.numColumns = 4;
		gridLayout2.marginWidth = 0;
		gridLayout2.marginHeight = 0;
		GridData gridData2 = new GridData();
		gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData2.grabExcessHorizontalSpace = true;
		composite1 = new Composite(this, SWT.NONE);
		composite1.setLayoutData(gridData2);
		composite1.setLayout(gridLayout2);
		fileLabel = new Label(composite1, SWT.NONE);
		fileLabel.setText("File name:");
		fileCombo = new CCombo(composite1, SWT.BORDER);
		fileCombo.setLayoutData(gridData11);
		runLabel = new Label(composite1, SWT.NONE);
		runLabel.setText("Run name:");
		runCombo = new CCombo(composite1, SWT.BORDER);
		runCombo.setLayoutData(gridData21);
	}


	/**
	 * This method initializes composite	
	 *
	 */
	private void createComposite2() {
		GridData gridData31 = new GridData();
		gridData31.grabExcessHorizontalSpace = true;
		gridData31.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData5 = new GridData();
		gridData5.grabExcessHorizontalSpace = true;
		gridData5.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData6 = new GridData();
		gridData6.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridLayout gridLayout3 = new GridLayout();
		gridLayout3.numColumns = 6;
		gridLayout3.marginWidth = 0;
		gridLayout3.marginHeight = 0;
		composite2 = new Composite(this, SWT.NONE);
		composite2.setLayout(gridLayout3);
		composite2.setLayoutData(gridData6);
		experimentLabel = new Label(composite2, SWT.NONE);
		experimentLabel.setText("Experiment:");
		experimentCombo = new CCombo(composite2, SWT.BORDER);
		experimentCombo.setLayoutData(gridData);
		measurementLabel = new Label(composite2, SWT.NONE);
		measurementLabel.setText("Measurement:");
		measurementCombo = new CCombo(composite2, SWT.BORDER);
		measurementCombo.setLayoutData(gridData5);
		replicationLabel = new Label(composite2, SWT.NONE);
		replicationLabel.setText("Replication:");
		replicationCombo = new CCombo(composite2, SWT.BORDER);
		replicationCombo.setLayoutData(gridData31);
	}

	/**
	 * This method initializes composite3	
	 *
	 */
	private void createComposite3() {
		GridData gridData8 = new GridData();
		gridData8.grabExcessHorizontalSpace = true;
		gridData8.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData7 = new GridData();
		gridData7.grabExcessHorizontalSpace = true;
		gridData7.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridData gridData10 = new GridData();
		gridData10.grabExcessHorizontalSpace = true;
		gridData10.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		GridLayout gridLayout4 = new GridLayout();
		gridLayout4.numColumns = 4;
		gridLayout4.marginWidth = 0;
		gridLayout4.marginHeight = 0;
		composite3 = new Composite(this, SWT.NONE);
		composite3.setLayout(gridLayout4);
		composite3.setLayoutData(gridData10);
		moduleLabel = new Label(composite3, SWT.NONE);
		moduleLabel.setText("Module name:");
		moduleCombo = new CCombo(composite3, SWT.BORDER);
		moduleCombo.setLayoutData(gridData7);
		dataLabel = new Label(composite3, SWT.NONE);
		dataLabel.setText("Data name:");
		dataCombo = new CCombo(composite3, SWT.BORDER);
		dataCombo.setLayoutData(gridData8);
	}
}
