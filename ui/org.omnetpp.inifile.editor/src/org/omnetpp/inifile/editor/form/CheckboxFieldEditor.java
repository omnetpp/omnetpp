package org.omnetpp.inifile.editor.form;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Checkbox-based editor for boolean inifile entries.
 * 
 * @author Andras
 */
public class CheckboxFieldEditor extends FieldEditor {
	private Button checkbox;
	private Label label;
	private Button resetButton;
	private boolean isEdited;

	public CheckboxFieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, style, entry, inifile);

		Assert.isTrue(entry.getType()==ConfigurationEntry.Type.CFG_BOOL);

		GridLayout gridLayout = new GridLayout(3, false);
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);
		
		checkbox = new Button(this, SWT.CHECK);
		checkbox.setBackground(BGCOLOR);
		label = createLabel(entry, labelText);
		resetButton = createResetButton();


		checkbox.setLayoutData(new GridData());
		label.setLayoutData(new GridData());
		resetButton.setLayoutData(new GridData());

		reread();

		checkbox.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				isEdited = true;
				resetButton.setEnabled(true);
			}
		});
	}

	@Override
	public void reread() {
		String value = getValueFromFile();
		if (value==null) {
			boolean defaultValue = entry.getDefaultValue()==null ? false : (Boolean)entry.getDefaultValue(); 
			checkbox.setSelection(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			checkbox.setSelection(parseAsBool(value));
			resetButton.setEnabled(true);
		}
		isEdited = false;
	}

	private boolean parseAsBool(String value) {
		boolean boolValue;
		if (value.equals("yes") || value.equals("true") || value.equals("on") || value.equals("1"))
	    	boolValue = true;
	    else if (value.equals("no") || value.equals("false") || value.equals("off") || value.equals("0"))
	    	boolValue = false;
	    else
	    	boolValue = false; //XXX something invalid, we take it as false; probably should warn the user or something
		return boolValue;
	}
	
	@Override
	public void commit() {
		if (isEdited) {
			boolean value = checkbox.getSelection();
			setValueInFile(value ? "true" : "false");
			isEdited = false;
		}
	}
}
