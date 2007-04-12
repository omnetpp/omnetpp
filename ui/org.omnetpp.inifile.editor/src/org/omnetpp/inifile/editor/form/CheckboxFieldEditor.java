package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

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
	private String section = GENERAL;

	public CheckboxFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, SWT.NONE, entry, inifile);

		Assert.isTrue(entry.getType()==ConfigurationEntry.Type.CFG_BOOL);

		GridLayout gridLayout = new GridLayout(3, false); 
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		checkbox = new Button(this, SWT.CHECK);
		checkbox.setBackground(BGCOLOR);
		label = createLabel(entry, labelText);
		resetButton = createResetButton();

		checkbox.setLayoutData(new GridData());
		label.setLayoutData(new GridData());
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		checkbox.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (!isEdited) {
					isEdited = true;
					resetButton.setEnabled(true);
				}
			}
		});
		
		// commit on losing focus, etc.
		addFocusListenerTo(checkbox);
	}

	@Override
	public void reread() {
		String value = getValueFromFile(section);
		if (value==null) {
			boolean defaultValue = entry.getDefaultValue()==null ? false : (Boolean)entry.getDefaultValue(); 
			checkbox.setSelection(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			checkbox.setSelection(InifileUtils.parseAsBool(value));
			resetButton.setEnabled(true);
		}
		isEdited = false;
	}

	@Override
	public void commit() {
		if (isEdited) {
			boolean value = checkbox.getSelection();
			setValueInFile(section, value ? "true" : "false");
			isEdited = false;
		}
	}
}
