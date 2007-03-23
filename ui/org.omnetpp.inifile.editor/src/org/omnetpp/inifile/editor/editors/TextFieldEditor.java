package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

public class TextFieldEditor extends FieldEditor {
	private Text textField;
	private Label label;
	private Button resetButton;
	private String oldValue;
	

	public TextFieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style, entry, inifile);

		label = new Label(this, SWT.NONE);
		label.setText("["+entry.getSection()+(entry.isGlobal() ? "" : "] or [Run X")+"] "+entry.getName());
		label.setToolTipText(StringUtils.breakLines(entry.getDescription(),60));
		label.setBackground(BGCOLOR);
		textField = new Text(this, SWT.SINGLE | SWT.BORDER);
		resetButton = new Button(this, SWT.PUSH);
		resetButton.setText("Reset");
		resetButton.setToolTipText("Reset to default, and remove line from ini file");

		setLayout(new GridLayout(3, false));
		label.setLayoutData(new GridData());
		//textField.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
		textField.setLayoutData(new GridData(200, SWT.DEFAULT));
		resetButton.setLayoutData(new GridData());

		reread();

		textField.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				resetButton.setEnabled(true);
			}
		});
		resetButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				resetToDefault();
			}
		});
		
//		SelectionListener(new SelectionAdapter() {
//			public void widgetDefaultSelected(SelectionEvent e) {
//				setAsString(textField.getText());  //XXX rather, on focusOut or something like that?
//			}
//		});
		
	}

	@Override
	public void reread() {
		String value = getValueAsString();
		if (value==null) {
			String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString(); 
			textField.setText(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			textField.setText(value);
			oldValue = value;
			resetButton.setEnabled(true);
		}
	}

	@Override
	public void commit() {
		String value = textField.getText();
		if (!value.equals(oldValue)) {
			setValueAsString(value); //XXX
		}
	}

	@Override
	public void resetToDefault() {
		resetValue();
		reread();
	}
}
