package org.omnetpp.scave.SCRAP;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.scave.model2.Filter;

/**
 * The user can select the fields used to identify the runs of the selected
 * data items.
 *
 * @author tomi
 */
public class RunSelectionPanel extends Composite {

	private Button useRunNameRadio = null;
	private Button useReplicationRadio = null;
	private Button[] runidFieldCheckboxes = null;

	private static final String[] runidFields = new String[] {
		Filter.FIELD_FILENAME,
		Filter.FIELD_EXPERIMENT,
		Filter.FIELD_MEASUREMENT,
		Filter.FIELD_REPLICATION,
	};
	
	private static final String[] runidExact = new String[] {
		Filter.FIELD_FILENAME, // XXX until no runname generated
		Filter.FIELD_RUNNAME,
	};

	
	public RunSelectionPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	public String[] getRunIdFields() {
		if (useRunNameRadio.getSelection()) {
			return runidExact;
		}
		else {
			int count = 0;
			for (Button cb : runidFieldCheckboxes)
				if (cb.getSelection())
					count++;
			String[] fields = new String[count];
			int i=0;
			for (int j = 0; i < runidFieldCheckboxes.length; ++j)
				if (runidFieldCheckboxes[j].getEnabled())
					fields[i++] = runidFields[j];
			return fields;
		}
	}

	/**
	 * This method initializes composite2	
	 *
	 */
	private void initialize() {
		GridData gridData;
		GridLayout gridLayout;
		//RowLayout rowLayout;

		gridLayout = new GridLayout();
		gridLayout.numColumns = 1;
		this.setLayout(gridLayout);
		
		gridData = new GridData();
		gridData.horizontalIndent = 15;
		useRunNameRadio = new Button(this, SWT.RADIO);
		useRunNameRadio.setText("from exactly matching runs");
		useRunNameRadio.setSelection(true);
		useRunNameRadio.setLayoutData(gridData);

		gridData = new GridData();
		gridData.horizontalIndent = 15;
		useReplicationRadio = new Button(this, SWT.RADIO);
		useReplicationRadio.setText("from any runs with matching ");
		useReplicationRadio.setLayoutData(gridData);

		gridData = new GridData();
		gridData.horizontalIndent = 30;
		gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.makeColumnsEqualWidth = true;
		Composite filterFieldsPanel = new Composite(this, SWT.NONE);
		filterFieldsPanel.setLayoutData(gridData);
		filterFieldsPanel.setLayout(gridLayout);
		runidFieldCheckboxes = new Button[runidFields.length];
		int i = 0;
		for (String field : runidFields) {
			Button checkbox = new Button(filterFieldsPanel, SWT.CHECK);
			checkbox.setText(field);
			runidFieldCheckboxes[i++] = checkbox;
		}
	}
	
}
