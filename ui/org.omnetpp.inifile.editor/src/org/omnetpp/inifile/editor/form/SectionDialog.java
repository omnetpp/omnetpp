package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

/**
 * Edit section name and its description. 
 * @author Andras
 */
//XXX make it prettier...
//XXX also: error message field should not be a tab stop
public class SectionDialog extends InputDialog {
	private String description;
	private Text descriptionText;

	public SectionDialog(Shell parentShell, String dialogTitle, String dialogMessage, String initialValue, IInputValidator validator) {
		super(parentShell, dialogTitle, dialogMessage, initialValue, validator);
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite) super.createDialogArea(parent);

		Label label = new Label(composite, SWT.WRAP);
		label.setText("Description (optional):");
		label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
		label.setFont(parent.getFont());

		descriptionText = new Text(composite, SWT.SINGLE | SWT.BORDER);
		descriptionText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
		return composite;
	}
	
    protected void buttonPressed(int buttonId) {
        description = (buttonId == IDialogConstants.OK_ID) ? descriptionText.getText() : null;
        super.buttonPressed(buttonId);
    }
	
	public void setDescription(String quotedText) {
		descriptionText.setText(quotedText);
	}

	public String getDescription() {
		return description;
	}
}
