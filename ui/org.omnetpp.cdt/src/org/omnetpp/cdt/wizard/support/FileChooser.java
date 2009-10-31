package org.omnetpp.cdt.wizard.support;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.cdt.wizard.IWidgetAdapter;

/**
 * A control for selecting a file from the workspace.
 * Implemented as a Composite with a single-line Text and a Browse button.
 * 
 * Does not replicate all the zillion methods of the Text class;
 * rather, it exposes the internal Text widget so that it can
 * be manipulated directly.
 * 
 * @author Andras
 */
public class FileChooser extends Composite implements IWidgetAdapter {
	private Text text;
	private Button browseButton;

	public FileChooser(Composite parent, int style) {
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
        ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(), new WorkbenchContentProvider());
        dialog.setTitle("Select File");
        dialog.setMessage("Select file");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        dialog.setAllowMultiple(false);
        if (dialog.open() == IDialogConstants.OK_ID) {
        	Object[] result = dialog.getResult();
        	if (result.length > 0)
        		text.setText(((IResource)result[0]).getFullPath().toString());
        }
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
		return control instanceof FileChooser; 
	}

	/**
	 * Adapter interface.
	 */
	public Object getValueFromControl(Control control) {
		return ((FileChooser)control).getFileName();
	}

	/**
	 * Adapter interface.
	 */
	public void writeValueIntoControl(Control control, Object value) {
	    String fileName = value instanceof IResource ? ((IResource)value).getFullPath().toString() : value.toString();
		((FileChooser)control).getTextControl().setText(fileName);
	}

}
