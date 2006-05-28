package org.omnetpp.experimental.seqchart.editors;

import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.SelectionDialog;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;

/**
 * Dialog to add items to a given dataset.
 * 
 * @author andras
 */
public class ModuleTreeDialog extends SelectionDialog {

	private CheckboxTreeViewer treeViewer;
	private Object input;
	
	/**
	 * This method initializes the dialog
	 */
	public ModuleTreeDialog(Shell parentShell, ModuleTreeItem moduleItemTree, List<ModuleTreeItem> checkedModuleItems) {
		super(parentShell);
		setShellStyle(getShellStyle()|SWT.RESIZE);
        setTitle("Choose Modules");
        setMessage("Hello message");
        input = moduleItemTree;
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

        treeViewer = new CheckboxTreeViewer(composite, SWT.CHECK | SWT.BORDER);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 300;
        gridData.heightHint = 300;
        treeViewer.getTree().setLayoutData(gridData);

        treeViewer.setContentProvider(new ITreeContentProvider() {
			public void dispose() { }
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) { }
			public Object[] getChildren(Object parentElement) {
				ModuleTreeItem[] submods = ((ModuleTreeItem)parentElement).getSubmodules(); 
				Arrays.sort(submods, 0, submods.length);
				return submods;
			}
			public Object getParent(Object element) {
				return ((ModuleTreeItem)element).getParentModule();
			}
			public boolean hasChildren(Object element) {
				return ((ModuleTreeItem)element).getSubmodules().length>0;
			}
			public Object[] getElements(Object inputElement) {
				return getChildren(inputElement);
			}
        });
        
        treeViewer.setLabelProvider(new ILabelProvider() {
			public void addListener(ILabelProviderListener listener) { }
			public void removeListener(ILabelProviderListener listener) { }
			public void dispose() { }
			public boolean isLabelProperty(Object element, String property) {
				return false;
			}
			public Image getImage(Object element) {
				return null;
			}
			public String getText(Object element) {
				ModuleTreeItem mod = (ModuleTreeItem)element;
				String text = mod.getModuleName();
				if (mod.getModuleClassName()!=null)
					text += " ("+mod.getModuleClassName()+")";
				if (mod.getModuleId()!=-1)
					text += " (id="+mod.getModuleId()+")";
				return text;
			}
        	
        });

        // configure dialog behaviour according to the following rules:
        // - if a compound module is checked, its submodules and parents get unchecked 
        treeViewer.addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
				if (event.getChecked()) {
					ModuleTreeItem e = (ModuleTreeItem)event.getElement();
					treeViewer.setSubtreeChecked(e, false);
					treeViewer.setChecked(e, true);
					// all parents should be unchecked
				    for (ModuleTreeItem current=e.getParentModule(); current!=null; current=current.getParentModule())
				    	treeViewer.setChecked(current, false);
				}
				else {
					// when unchecked, expand and check all submodules
					ModuleTreeItem e = (ModuleTreeItem)event.getElement();
					treeViewer.setSubtreeChecked(e, false);
					treeViewer.setExpandedState(e, true);
					for (ModuleTreeItem i : e.getSubmodules())
						treeViewer.setChecked(i, true);
				}
			}
        });
        
        treeViewer.setInput(input);
        treeViewer.expandAll();

        // set initial selection
        if (getInitialElementSelections()!=null) 
        	treeViewer.setCheckedElements(getInitialElementSelections().toArray());
        return composite;

    }
    
    protected void okPressed() {
		Object[] checkedItems = treeViewer.getCheckedElements();
		setSelectionResult(checkedItems);
        super.okPressed();
    }
}
