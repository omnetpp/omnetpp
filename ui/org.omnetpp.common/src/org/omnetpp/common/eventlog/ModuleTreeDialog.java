package org.omnetpp.common.eventlog;

import java.util.List;

import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.SelectionDialog;

/**
 * Dialog to add items to a given dataset.
 * 
 * @author andras
 */
public class ModuleTreeDialog extends SelectionDialog {

	private CheckboxTreeViewer moduleTreeViewer;

	private ModuleTreeItem root;
	
	/**
	 * This method initializes the dialog
	 */
	public ModuleTreeDialog(Shell parentShell, ModuleTreeItem moduleItemTree, List<ModuleTreeItem> checkedModuleItems) {
		super(parentShell);
		setShellStyle(getShellStyle()|SWT.RESIZE);
        setTitle("Choose Modules");
        setMessage("Choose Modules For Axes"); // XXX this does not appear anywhere on the GUI 
        root = moduleItemTree;
        setInitialElementSelections(checkedModuleItems);
	}

    @Override
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        //PlatformUI.getWorkbench().getHelpSystem().setHelp(newShell, IReadmeConstants.SECTIONS_DIALOG_CONTEXT);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite) super.createDialogArea(parent);
        
		Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        label.setText("Choose modules for chart axes:");

        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 500;
        gridData.heightHint = 400;
        moduleTreeViewer.getTree().setLayoutData(gridData);

        moduleTreeViewer = new ModuleTreeViewer(composite, root);

        // set initial selection
        if (getInitialElementSelections() != null) 
        	moduleTreeViewer.setCheckedElements(getInitialElementSelections().toArray());

        return composite;
    }
    
    protected void okPressed() {
		Object[] checkedItems = moduleTreeViewer.getCheckedElements();
		setSelectionResult(checkedItems);
        super.okPressed();
    }
}
