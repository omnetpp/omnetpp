package org.omnetpp.scave.editors.ui;

import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_FILENAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_RUNNAME;
import static org.omnetpp.scave.model2.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.model2.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.model2.RunAttribute.REPLICATION;

import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * This dialog is called from CreateChartTemplateAction.
 * The user can enter the name of the new dataset/chart
 * and select the fields that identifies the run.
 *
 * @author tomi
 */
//FIXME checkboxes have to be replaced by a CheckboxTable that includes the "Name" and "Module Name" fields too
// Potential hint text: "Filter fields" / "Choose fields that identify the data; these fields will be used as
// data filter criteria when the chart is next opened." 
public class CreateChartTemplateDialog extends TitleAreaDialog {
	
	private static String RUNID_FIELD_KEY = "org.omnetpp.scave.editors.ui.CreateChartTemplateDialog.runidfield";
	
	String datasetName;
	String chartName;
	String[] runidFields;
	
	Text datasetnameText;
	Text chartnameText;
	Button[] runidButtons;

	public CreateChartTemplateDialog(Shell parentShell) {
		super(parentShell);
	}

	public String getChartName() {
		return chartName;
	}

	public String getDatasetName() {
		return datasetName;
	}

	public String[] getRunidFields() {
		return runidFields;
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
		group.setLayout(new GridLayout(3, false));
		group.setText("Filter Runs By");

		Label label2 = new Label(group, SWT.NONE);
		label2.setText(
				"In the new dataset, data will be selected from the inputs by name " +
				"and module name; \nhere you may choose to include additional filter fields " +
				"to identify the run.");
		label2.setLayoutData(new GridData());
		((GridData)label2.getLayoutData()).horizontalSpan = 3;

		int i = 0;
		runidButtons = new Button[5];
		Button runnameCb = runidButtons[i++] = createCheckbox(group, "run name", FIELD_RUNNAME);
		runidButtons[i++] = createCheckbox(group, "file name", FIELD_FILENAME);
		label = new Label(group, SWT.NONE); // placeholder
		Button experimentCb  = runidButtons[i++] = createCheckbox(group, "experiment", EXPERIMENT);
		Button measurementCb = runidButtons[i++] = createCheckbox(group, "measurement", MEASUREMENT);
		Button replicationCb = runidButtons[i++] = createCheckbox(group, "replication", REPLICATION);

		addDependency(runnameCb, true, new Button[] {experimentCb, measurementCb, replicationCb} , false);
		addDependency(measurementCb, true, new Button[] {experimentCb}, true);
		addDependency(replicationCb, true, new Button[] {experimentCb, measurementCb}, true);
		
		runnameCb.setSelection(true);
		return panel;
	}
	
	private Button createCheckbox(Composite parent, String label, String field) {
		Button cb = new Button(parent, SWT.CHECK);
		cb.setText(label);
		cb.setData(RUNID_FIELD_KEY, field);
		return cb;
	}
	
	private void addDependency(Button cb, final boolean state, final Button[] dependentCbs, final boolean depState)
	{
		cb.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				Button checkbox = (Button)e.widget;
				if (checkbox.getSelection() == state)
					for (Button btn : dependentCbs)
						btn.setSelection(depState);
			}
		});
	}

	@Override
	protected void okPressed() {
		datasetName = datasetnameText.getText();
		chartName = chartnameText.getText();
		int count = 0;
		for (Button btn : runidButtons)
			if (btn.getSelection())
				count++;
		runidFields = new String[count];
		int i = 0;
		for (Button btn : runidButtons)
			if (btn.getSelection())
				runidFields[i++] = (String)btn.getData(RUNID_FIELD_KEY);
		super.okPressed();
	}

	
}
