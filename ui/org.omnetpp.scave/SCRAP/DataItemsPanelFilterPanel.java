/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;

/**
 * A composite for displaying and editing the filter parameters.
 * It is a passive component, needs to be configured
 * to do anything useful.
 * 
 * It is used in the CreateDatasetDialog (in DataItemsPanel).
 *
 * @author tomi
 */
public class DataItemsPanelFilterPanel extends Composite {

	public static final int
		MODULE_NAME_ROW = 1,
		FILE_RUN_ROW = 2,
		EXPERIMENT_MEASUREMENT_REPLICATION_ROW = 4;
	public static final int
		ALL_ROWS = MODULE_NAME_ROW | FILE_RUN_ROW | EXPERIMENT_MEASUREMENT_REPLICATION_ROW;

	/**
	 * Number of visible items in combos.
	 */
	private static final int VISIBLE_ITEM_COUNT = 15;

	private int rows;
	private Composite composite1 = null;
	private Label fileLabel = null;
	private Label runLabel = null;
	private Composite composite2 = null;
	private Label experimentLabel = null;
	private Label measurementLabel = null;
	private Label replicationLabel = null;
	private Combo fileCombo = null;
	private Combo runCombo = null;
	private Combo experimentCombo = null;
	private Combo measurementCombo = null;
	private Composite composite3 = null;
	private Label moduleLabel = null;
	private Label dataLabel = null;
	private Combo replicationCombo = null;
	private Combo moduleCombo = null;
	private Combo dataCombo = null;

	public DataItemsPanelFilterPanel(Composite parent, int style) {
		this(parent, style, ALL_ROWS);
	}

	private DataItemsPanelFilterPanel(Composite parent, int style, int rows) { // TODO
		super(parent, style);
		this.rows = rows;
		initialize();
	}

	public Combo getDataCombo() {
		return dataCombo;
	}

	public Combo getExperimentCombo() {
		return experimentCombo;
	}

	public Combo getFileCombo() {
		return fileCombo;
	}

	public Combo getMeasurementCombo() {
		return measurementCombo;
	}

	public Combo getModuleCombo() {
		return moduleCombo;
	}

	public Combo getReplicationCombo() {
		return replicationCombo;
	}

	public Combo getRunCombo() {
		return runCombo;
	}

	public Filter getFilterParams() {
		return new Filter(
				fileCombo != null ? fileCombo.getText() : null,
				runCombo != null ? runCombo.getText() : null,
				experimentCombo != null ? experimentCombo.getText() : null,
				measurementCombo != null ? measurementCombo.getText() : null,
				replicationCombo != null ? replicationCombo.getText() : null,
				moduleCombo != null ? moduleCombo.getText() : null,
				dataCombo != null ? dataCombo.getText() : null);
	}

	public void setFilterParams(Filter params) {
		if (fileCombo != null) fileCombo.setText(params.getField(Filter.FIELD_FILENAME));
		if (runCombo != null) runCombo.setText(params.getField(Filter.FIELD_RUNNAME));
		if (experimentCombo != null) experimentCombo.setText(params.getField(Filter.FIELD_EXPERIMENT));
		if (measurementCombo != null) measurementCombo.setText(params.getField(Filter.FIELD_MEASUREMENT));
		if (replicationCombo != null) replicationCombo.setText(params.getField(Filter.FIELD_REPLICATION));
		if (moduleCombo != null) moduleCombo.setText(params.getField(Filter.FIELD_MODULENAME));
		if (dataCombo != null) dataCombo.setText(params.getField(Filter.FIELD_DATANAME));
	}

	public void setFilterHints(FilterHints hints) {
		if (fileCombo != null) fileCombo.setItems(hints.getHints(Filter.FIELD_FILENAME));
		if (runCombo != null) runCombo.setItems(hints.getHints(Filter.FIELD_RUNNAME));
		if (experimentCombo != null) experimentCombo.setItems(hints.getHints(Filter.FIELD_EXPERIMENT));
		if (measurementCombo != null) measurementCombo.setItems(hints.getHints(Filter.FIELD_MEASUREMENT));
		if (replicationCombo != null) replicationCombo.setItems(hints.getHints(Filter.FIELD_REPLICATION));
		if (moduleCombo != null) moduleCombo.setItems(hints.getHints(Filter.FIELD_MODULENAME));
		if (dataCombo != null) dataCombo.setItems(hints.getHints(Filter.FIELD_DATANAME));
	}

	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.verticalSpacing = 5;
		gridLayout.marginHeight = 5;
		gridLayout.marginWidth = 5;
		gridLayout.horizontalSpacing = 0;
		this.setLayout(gridLayout);
		if ((rows & MODULE_NAME_ROW) != 0)
			createComposite1();
		if ((rows & FILE_RUN_ROW) != 0)
			createComposite2();
		if ((rows & EXPERIMENT_MEASUREMENT_REPLICATION_ROW) != 0)
			createComposite3();
	}

	/**
	 * This method initializes composite3
	 *
	 */
	private void createComposite1() {
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
		moduleLabel.setText("Module:");
		moduleCombo = new CCombo(composite3, SWT.BORDER);
		moduleCombo.setLayoutData(gridData7);
		moduleCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		dataLabel = new Label(composite3, SWT.NONE);
		dataLabel.setText("Statistic:");
		dataCombo = new CCombo(composite3, SWT.BORDER);
		dataCombo.setLayoutData(gridData8);
		dataCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
	}

	/**
	 * This method initializes composite1
	 *
	 */
	private void createComposite2() {
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
		fileCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		runLabel = new Label(composite1, SWT.NONE);
		runLabel.setText("Run id:");
		runCombo = new CCombo(composite1, SWT.BORDER);
		runCombo.setLayoutData(gridData21);
		runCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
	}

	/**
	 * This method initializes composite
	 *
	 */
	private void createComposite3() {
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
		experimentCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		measurementLabel = new Label(composite2, SWT.NONE);
		measurementLabel.setText("Measurement:");
		measurementCombo = new CCombo(composite2, SWT.BORDER);
		measurementCombo.setLayoutData(gridData5);
		measurementCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		replicationLabel = new Label(composite2, SWT.NONE);
		replicationLabel.setText("Replication:");
		replicationCombo = new CCombo(composite2, SWT.BORDER);
		replicationCombo.setLayoutData(gridData31);
		replicationCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
	}
}
