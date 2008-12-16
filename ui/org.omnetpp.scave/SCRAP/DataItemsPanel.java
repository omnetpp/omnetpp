/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;

/**
 * This panel displayed in the DatasetDialog.
 * 
 * TODO: merge with DatasetDialog
 *
 * @author tomi
 */
// TODO: gray out disabled composites
public class DataItemsPanel extends Composite {

	private Group group = null;
	private Button useSelectionRadio = null;
	private Button useFilterRadio = null;
	private DataItemsPanelFilterPanel filterParamsPanel = null;
	private RunSelectionPanel selectIdentifierPanel = null;
	
	public DataItemsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public boolean useFilter() {
		return useFilterRadio.getSelection();
	}
	
	public void setUseFilter(boolean flag) {
		useFilterRadio.setSelection(flag);
		useSelectionRadio.setSelection(!flag);
	}
	
	public Filter getFilterParams() {
		return filterParamsPanel.getFilterParams();
	}
	
	public void setFilterParams(Filter params) {
		filterParamsPanel.setFilterParams(params);
	}
	
	public void setFilterHints(FilterHints hints) {
		filterParamsPanel.setFilterHints(hints);
	}
	
	public String[] getRunIdFields() {
		return selectIdentifierPanel.getRunIdFields();
	}

	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.verticalSpacing = 20;
		this.setLayout(gridLayout);
		createGroup();
	}

	/**
	 * This method initializes group	
	 *
	 */
	private void createGroup() {
		GridLayout gridLayout1 = new GridLayout();
		gridLayout1.numColumns = 1;
		GridData gridData4 = new GridData();
		gridData4.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData4.horizontalSpan = 2;
		gridData4.grabExcessVerticalSpace = false;
		gridData4.verticalAlignment = org.eclipse.swt.layout.GridData.CENTER;
		gridData4.grabExcessHorizontalSpace = true;
		group = new Group(this, SWT.NONE);
		group.setText("Data items");
		group.setLayout(gridLayout1);
		group.setLayoutData(gridData4);

		useSelectionRadio = new Button(group, SWT.RADIO);
		useSelectionRadio.setText("Add selected data items");
		useSelectionRadio.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				selectIdentifierPanel.setEnabled(useSelectionRadio.getSelection());
			}
		});
		createSelectIdentifierPanel();

		useFilterRadio = new Button(group, SWT.RADIO);
		useFilterRadio.setText("Add data items matching with filter");
		useFilterRadio.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				filterParamsPanel.setEnabled(useFilterRadio.getSelection());
			}
		});
		createFilterPanel();
	}
	
	/**
	 * This method initializes composite2	
	 *
	 */
	private void createSelectIdentifierPanel() {
		GridData gridData = new GridData();
		gridData.horizontalAlignment = GridData.FILL;
		gridData.horizontalIndent = 10;
		selectIdentifierPanel = new RunSelectionPanel(group, SWT.NONE);
		selectIdentifierPanel.setLayoutData(gridData);
	}
	
	private void createFilterPanel() {
		filterParamsPanel = new DataItemsPanelFilterPanel(group, SWT.NONE);
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.horizontalIndent = 10;
		gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		filterParamsPanel.setLayoutData(gridData);
	}
}
