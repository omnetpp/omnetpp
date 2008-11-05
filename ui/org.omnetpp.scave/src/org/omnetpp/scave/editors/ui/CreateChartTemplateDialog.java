package org.omnetpp.scave.editors.ui;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScavePlugin;

/**
 * This dialog is called from CreateChartTemplateAction.
 * The user can enter the name of the new dataset/chart
 * and select the fields that identifies the run.
 *
 * @author tomi
 */
public class CreateChartTemplateDialog extends TitleAreaDialog {
	
	private static String RUNID_FIELD_KEY = "org.omnetpp.scave.editors.ui.CreateChartTemplateDialog.runidfield";
	
	String datasetName;
	String chartName;
	String[] filterFields;
	
	Text datasetnameText;
	Text chartnameText;
	Table filterFieldsTable;
	
	public CreateChartTemplateDialog(Shell parentShell) {
		super(parentShell);
	}
	
	@Override
	protected IDialogSettings getDialogBoundsSettings() {
	    return UIUtils.getDialogSettings(ScavePlugin.getDefault(), getClass().getName());
	}

	public String getChartName() {
		return chartName;
	}

	public String getDatasetName() {
		return datasetName;
	}

	public String[] getFilterFields() {
		return filterFields;
	}
	
	@Override
	protected void configureShell(Shell newShell) {
		newShell.setText("Create chart template");
		super.configureShell(newShell);
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		this.setTitle("New Chart Template");
		this.setMessage("Add a new dataset containing the chart to the analysis file.");
		Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayoutData(new GridData(GridData.FILL_BOTH));
        panel.setLayout(new GridLayout(2, false));
        
        Label label = new Label(panel, SWT.NONE);
        label.setText("Dataset name:");
		datasetnameText = new Text(panel, SWT.BORDER);
		datasetnameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		label = new Label(panel, SWT.NONE);
		label.setText("Chart name:");
		chartnameText = new Text(panel, SWT.BORDER);
		chartnameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		Group group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		group.setLayout(new GridLayout(1, false));
		group.setText("Filter fields");

		Label label2 = new Label(group, SWT.WRAP);
		label2.setText("Choose fields that identify the data; these fields will be used as data filter criteria when the chart is next opened." );
		label2.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

		filterFieldsTable = new Table(group, SWT.CHECK | SWT.BORDER | SWT.HIDE_SELECTION);
		filterFieldsTable.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		createFilterFieldItem(group, "By Run Id", RUN);
		createFilterFieldItem(group, "By File Name", FILE);
		createFilterFieldItem(group, "Experiment", EXPERIMENT);
		createFilterFieldItem(group, "Measurement", MEASUREMENT);
		createFilterFieldItem(group, "Replication", REPLICATION);
		TableItem moduleNameItem = createFilterFieldItem(group, "Module name", MODULE);
		TableItem dataNameItem = createFilterFieldItem(group, "Data name", NAME);
		moduleNameItem.setChecked(true);
		dataNameItem.setChecked(true);

		return panel;
	}
	
	private TableItem createFilterFieldItem(Composite parent, String label, String field) {
		TableItem item = new TableItem(filterFieldsTable, SWT.NONE);
		item.setText(label);
		item.setData(RUNID_FIELD_KEY, field);
		return item;
	}
	
	@Override
	protected void okPressed() {
		datasetName = datasetnameText.getText();
		chartName = chartnameText.getText();
		int count = 0;
		for (TableItem item : filterFieldsTable.getItems())
			if (item.getChecked())
				count++;
		int i = 0;
		filterFields = new String[count];
		for (TableItem item : filterFieldsTable.getItems())
			if (item.getChecked())
				filterFields[i++] = (String)item.getData(RUNID_FIELD_KEY);
		super.okPressed();
	}
}
