package org.omnetpp.cdt.wizard.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.wizard.IWidgetAdapter;

/**
 * A control for selecting a file from the file system.
 * Implemented as a Composite with a single-line Text and a Browse button.
 * 
 * Does not replicate all the zillion methods of the Text class;
 * rather, it exposes the internal Text widget so that it can
 * be manipulated directly.
 * 
 * @author Andras
 */
public class ExternalFileChooser extends Composite implements IWidgetAdapter {
	private Text text;
	private Button browseButton;

	public ExternalFileChooser(Composite parent, int style) {
		super(parent, style);
		GridLayout layout = new GridLayout(2,false);
		layout.marginHeight = layout.marginWidth = 0;
		setLayout(layout);
		
		text = new Text(this, SWT.SINGLE|SWT.BORDER);
		browseButton = new Button(this, SWT.PUSH);
        browseButton.setText("Browse...");
		
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				browse();
			}
		});
	}

	protected void browse() {
        // filesystem selection:		
		FileDialog dialog = new FileDialog(getShell());
		dialog.setFileName(text.getText());
		String result = dialog.open();
		if (result != null)
			text.setText(result);
	}

	public String getFileName() {
		return text.getText();
	}

	public void setFileName(String file) {
		text.setText(file);
	}

	public Text getTextControl() {
		return text;
	}

	public Button getBrowseButton() {
		return browseButton;
	}

	/**
	 * Adapter interface.
	 */
	public boolean supportsControl(Control control) {
		return control instanceof ExternalFileChooser; 
	}

	/**
	 * Adapter interface.
	 */
	public Object getValueFromControl(Control control) {
		return ((ExternalFileChooser)control).getFileName();
	}

	/**
	 * Adapter interface.
	 */
	public void writeValueIntoControl(Control control, Object value) {
		((ExternalFileChooser)control).setFileName(value.toString());
	}

}
