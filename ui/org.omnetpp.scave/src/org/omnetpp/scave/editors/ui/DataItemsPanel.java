package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.ScaveModelUtil.RunIdKind;

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
	private Button useFilterRadio = null;
	private Button useSelectionRadio = null;
	private FilterParamsPanel filterParamsPanel = null;
	private Composite selectIdentifierPanel = null;
	private Button useRunNameRadio = null;
	private Button useReplicationRadio = null;
	private Label label = null;
	
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
	
	public RunIdKind getRunIdKind() {
		return useRunNameRadio.getSelection() ?
				RunIdKind.FILE_RUN : RunIdKind.EXPERIMENT_MEASUREMENT_REPLICATION;
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

	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.verticalSpacing = 20;
		this.setLayout(gridLayout);
		createGroup();
	}

	/**
	 * This method initializes composite2	
	 *
	 */
	private void createSelectIdentifierPanel() {
		GridData gridData5 = new GridData();
		gridData5.horizontalIndent = 15;
		GridData gridData2 = new GridData();
		gridData2.horizontalIndent = 15;
		GridLayout gridLayout2 = new GridLayout();
		gridLayout2.numColumns = 1;
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData3.horizontalIndent = 10;
		selectIdentifierPanel = new Composite(group, SWT.NONE);
		selectIdentifierPanel.setLayoutData(gridData3);
		selectIdentifierPanel.setLayout(gridLayout2);
		label = new Label(selectIdentifierPanel, SWT.NONE);
		label.setText("Data items identified by");
		useRunNameRadio = new Button(selectIdentifierPanel, SWT.RADIO);
		useRunNameRadio.setText("file path, run name, module name and data name");
		useRunNameRadio.setSelection(true);
		useRunNameRadio.setLayoutData(gridData2);
		useReplicationRadio = new Button(selectIdentifierPanel, SWT.RADIO);
		useReplicationRadio.setText("experiment, measurement, replication, module name and data name");
		useReplicationRadio.setLayoutData(gridData5);
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
		useFilterRadio = new Button(group, SWT.RADIO);
		useFilterRadio.setText("Add data items matching with filter");
		useFilterRadio.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				filterParamsPanel.setEnabled(useFilterRadio.getSelection());
			}
		});
		createFilterPanel();
		useSelectionRadio = new Button(group, SWT.RADIO);
		useSelectionRadio.setText("Add selected data items");
		useSelectionRadio.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				selectIdentifierPanel.setEnabled(useSelectionRadio.getSelection());
			}
		});
		createSelectIdentifierPanel();
	}
	
	private void createFilterPanel() {
		filterParamsPanel = new FilterParamsPanel(group, SWT.NONE);
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.horizontalIndent = 10;
		gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		filterParamsPanel.setLayoutData(gridData);
	}
}
