package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave2.model.FilterHints;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.ScaveModelUtil.RunIdKind;

/**
 * This panel displayed in the DatasetDialog.
 * 
 * TODO: merge with DatasetDialog
 *
 * @author tomi
 */
public class DataItemsPanel extends Composite {

	private Group group = null;
	private Button useFilterRadio = null;
	private Button useSelectionRadio = null;
	private FilterParamsPanel filterParamsPanel = null;
	private Composite composite2 = null;
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
	
	public RunIdKind getRunIdKind() {
		return useRunNameRadio.getSelection() ?
				RunIdKind.FILE_RUN : RunIdKind.EXPERIMENT_MEASUREMENT_REPLICATION;
	}
	
	public FilterParams getFilterParams() {
		return filterParamsPanel.getFilterParams();
	}
	
	public void setFilterParams(FilterParams params) {
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
	private void createComposite2() {
		GridData gridData5 = new GridData();
		gridData5.horizontalIndent = 15;
		GridData gridData2 = new GridData();
		gridData2.horizontalIndent = 15;
		GridLayout gridLayout2 = new GridLayout();
		gridLayout2.numColumns = 1;
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData3.horizontalIndent = 10;
		composite2 = new Composite(group, SWT.NONE);
		composite2.setLayoutData(gridData3);
		composite2.setLayout(gridLayout2);
		label = new Label(composite2, SWT.NONE);
		label.setText("Data items identified by");
		useRunNameRadio = new Button(composite2, SWT.RADIO);
		useRunNameRadio.setText("file path, run name, module name and data name");
		useRunNameRadio.setSelection(true);
		useRunNameRadio.setLayoutData(gridData2);
		useReplicationRadio = new Button(composite2, SWT.RADIO);
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
		useFilterRadio.setSelection(true);
		createFilterPanel();
		useSelectionRadio = new Button(group, SWT.RADIO);
		useSelectionRadio.setText("Add selected data items");
		createComposite2();
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
