package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;
import java.util.Collection;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;

/**
 * Dialog to add items to a given dataset.
 * 
 * @author andras
 */
public class ModuleTreeDialog extends Dialog {

	private TreeViewer treeViewer;
	private ModuleTreeItem moduleTree;
	private Collection<ModuleTreeItem> selection;
	
	/**
	 * This method initializes the dialog
	 */
	public ModuleTreeDialog(Shell parentShell, ModuleTreeItem moduleTree, Collection<ModuleTreeItem> selection) {
		super(parentShell);
		setShellStyle(getShellStyle()|SWT.RESIZE);
		this.moduleTree = moduleTree;
		this.selection = selection;
	}

	public void dispose() {
		//XXX
	}

    @Override
    protected void configureShell(Shell newShell) {
        super.configureShell(newShell);
        newShell.setText("Choose Modules");
        //PlatformUI.getWorkbench().getHelpSystem().setHelp(newShell, IReadmeConstants.SECTIONS_DIALOG_CONTEXT);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);

        Label label = new Label(composite, SWT.NONE);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        label.setText("Choose modules for chart axes:");

        treeViewer = new TreeViewer(composite, SWT.CHECK | SWT.BORDER);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 300;
        gridData.heightHint = 300;
        treeViewer.getTree().setLayoutData(gridData);

        treeViewer.setContentProvider(new ITreeContentProvider() {
			public void dispose() { }
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) { }
			public Object[] getChildren(Object parentElement) {
				return ((ModuleTreeItem)parentElement).getSubmodules(); //XXX sort by name
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
					text = "("+mod.getModuleClassName()+") "+text;
				if (mod.getModuleId()!=-1)
					text += "  (id="+mod.getModuleId()+")";
				return text;
			}
        	
        });

        treeViewer.setInput(moduleTree);
        
        // set initial selection
        if (selection!=null) {
        	ArrayList<TreePath> paths = new ArrayList<TreePath>();
        	for (ModuleTreeItem sel : selection)
        		paths.add(new TreePath(sel.getPath()));
        	treeViewer.setSelection(new TreeSelection(paths.toArray(new TreePath[0])), true);
        }
        return composite;

    }

    public Collection<ModuleTreeItem> getSelection() {
    	return null;  //FIXME todo...
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
