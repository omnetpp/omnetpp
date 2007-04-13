package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

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
import org.omnetpp.inifile.editor.model.ConfigurationEntry.Type;

/**
 * Text-based editor for inifile entries.
 * 
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [Genera] section. When the setting is present outside
 * [General], the table-based field editor has to be used.
 * 
 * @author Andras
 */
public class TextFieldEditor extends FieldEditor {
	private Text textField;
	private Label label;
	private Button resetButton;
	private boolean isEdited;
	private String section = GENERAL;

	public TextFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, String labelText) {
		super(parent, SWT.NONE, entry, inifile);

		GridLayout gridLayout = new GridLayout(3, false);
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);

		label = createLabel(entry, labelText+":");
		textField = new Text(this, SWT.SINGLE | SWT.BORDER);
		tooltipSupport.adapt(textField);

		resetButton = createResetButton();
		label.setLayoutData(new GridData());
		int width = (entry.getType()==Type.CFG_STRING || entry.getType()==Type.CFG_FILENAME || entry.getType()==Type.CFG_FILENAMES) ? 250 : 50;
		textField.setLayoutData(new GridData(width, SWT.DEFAULT));
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		textField.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				if (!isEdited) {
					isEdited = true;
					resetButton.setEnabled(true);
				}
			}
		});
		
		// commit on losing focus, etc.
		addFocusListenerTo(textField);
	}

	@Override
	public void reread() {
		String value = getValueFromFile(section);
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
			setValueInFile(section, value);
			isEdited = false;
		}
	}
}
