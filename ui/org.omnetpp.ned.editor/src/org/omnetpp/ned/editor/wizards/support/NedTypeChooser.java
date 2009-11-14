package org.omnetpp.ned.editor.wizards.support;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.wizard.IWidgetAdapter;
import org.omnetpp.ned.core.ui.misc.NedTypeSelectionDialog;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * A control for selecting a NED type. Implemented as a Composite 
 * with a single-line Text and a Browse button.
 * 
 * Does not replicate all the zillion methods of the Text class;
 * rather, it exposes the internal Text widget so that it can
 * be manipulated directly.
 * 
 * @author Andras
 */
public class NedTypeChooser extends Composite implements IWidgetAdapter {
	private Text text;
	private Button browseButton;

	public NedTypeChooser(Composite parent, int style) {
		super(parent, style);
		GridLayout layout = new GridLayout(2,false);
		layout.marginHeight = layout.marginWidth = 0;
		setLayout(layout);
		
		text = new Text(this, SWT.SINGLE|SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
		browseButton = new Button(this, SWT.PUSH);
		browseButton.setText("Browse...");
		
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				browse();
			}
		});
	}

	protected void browse() {
        // pop up a chooser dialog
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        NedTypeSelectionDialog dialog = new NedTypeSelectionDialog(window.getShell());
        dialog.setMessage("Select NED type to open:");
        dialog.setTitle("Open NED Type");
        if (dialog.open() == ListDialog.OK) {
            INEDTypeInfo component = dialog.getResult()[0];
            text.setText(component.getFullyQualifiedName());
            text.selectAll();
        }
	}

    public String getNedName() {
        return text.getText();
    }

    public void setNedName(String file) {
        text.setText(file);
        text.selectAll();
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
		return control instanceof NedTypeChooser; 
	}

	/**
	 * Adapter interface.
	 */
	public Object getValueFromControl(Control control) {
		return ((NedTypeChooser)control).getNedName();
	}

	/**
	 * Adapter interface.
	 */
	public void writeValueIntoControl(Control control, Object value) {
	    String nedTypeName = value instanceof INEDTypeInfo ? ((INEDTypeInfo)value).getFullyQualifiedName() : value.toString();
		((NedTypeChooser)control).setNedName(nedTypeName);
	}

}
