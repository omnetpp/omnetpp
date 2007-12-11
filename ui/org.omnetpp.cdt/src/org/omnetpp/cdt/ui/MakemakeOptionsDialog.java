package org.omnetpp.cdt.ui;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.common.util.StringUtils;


/**
 *XXX 
 * @author Andras
 */
public class MakemakeOptionsDialog extends TitleAreaDialog {
	private String title;
	private MakemakeOptionsPanel contents;

    /**
     * Creates the dialog.
     */
    public MakemakeOptionsDialog(Shell parentShell, MakemakeOptions initialValue) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Makemake Options";
    }

	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (title != null)
			shell.setText(title);
	}

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        setTitle("Makemake Options");
        setMessage("Options for makefile creation");
    	
        // page group
        Composite dialogArea = (Composite) super.createDialogArea(parent);
        contents = new MakemakeOptionsPanel(dialogArea, SWT.NONE, null);
        return dialogArea;
    }
    
	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}
	
	protected void okPressed() {
        //TODO
        super.okPressed();
    }
	
	/**
	 * Returns the result of the dialog.
	 */
	public MakemakeOptions getResult() {
		return contents.getResult();
	}
}
