package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

public class TextFieldEditor extends FieldEditor {
	private Text textField;
	private Label label;
	private Button resetButton;
	private boolean isEdited;
	

	public TextFieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, style, entry, inifile);

		label = createLabel(entry, labelText);
		textField = new Text(this, SWT.SINGLE | SWT.BORDER);
		resetButton = createResetButton();

		setLayout(new GridLayout(3, false));
		label.setLayoutData(new GridData());
		//textField.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
		textField.setLayoutData(new GridData(200, SWT.DEFAULT));
		resetButton.setLayoutData(new GridData());

		reread();

		textField.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				isEdited = true;
				resetButton.setEnabled(true);
			}
		});
	}

	@Override
	public void reread() {
		String value = getValueFromFile();
		if (value==null) {
			String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString(); 
			textField.setText(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			textField.setText(value);
			resetButton.setEnabled(true);
		}
		isEdited = false;
	}

	@Override
	public void commit() {
		if (isEdited) {
			String value = textField.getText();
			setValueInFile(value); //XXX
			isEdited = false;
		}
	}
}
