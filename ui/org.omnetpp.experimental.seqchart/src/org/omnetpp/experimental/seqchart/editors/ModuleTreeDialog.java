package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;

/**
 * Dialog to add items to a given dataset.
 * 
 * @author andras
 */
public class ModuleTreeDialog extends Dialog {

	TreeViewer moduleTree;
	
	/**
	 * This method initializes the dialog
	 */
	public ModuleTreeDialog(Shell parentShell) {
		super(parentShell);
		setShellStyle(getShellStyle()|SWT.RESIZE);
	}

	public void dispose() {
		//XXX
	}

    @Override
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        newShell.setText("Choose modules");
        //PlatformUI.getWorkbench().getHelpSystem().setHelp(newShell, IReadmeConstants.SECTIONS_DIALOG_CONTEXT);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);
        //((GridLayout)composite.getLayout()).numColumns = 2;

        moduleTree = new TreeViewer(composite, SWT.CHECK);
        moduleTree.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        return composite;
    }

    @Override
	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, 1, "OK", true);
		createButton(parent, 2, "Cancel", false);
	}

    @Override
	protected void buttonPressed(int buttonId) {
		if (buttonId==1) {
			close(); //XXX and...?
		} else if (buttonId==2) {
			close(); //XXX and...?
		}
	}

}
