package org.omnetpp.common.eventlog;

import java.util.Arrays;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;

public class ModuleTreeViewer extends CheckboxTreeViewer {

	private ModuleTreeItem root;

	public ModuleTreeViewer(Composite parent, ModuleTreeItem root) {
		this(parent, SWT.CHECK | SWT.BORDER, root);
	}

	public ModuleTreeViewer(Composite parent, int style, ModuleTreeItem root) {
		super(parent, style);
		this.root = root;
		initialize(parent);
	}

	private void initialize(Composite parent) {
        setContentProvider(new ITreeContentProvider() {
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
        
        setLabelProvider(new ILabelProvider() {
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
				text += " -";
				if (mod.getModuleClassName()!=null)
					text += " (" + mod.getModuleClassName() + ")";
				text += " "+mod.getModuleFullPath();
				if (mod.getModuleId()!=-1)
					text += " (id=" + mod.getModuleId() + ")";
				return text;
			}
        	
        });

        // configure checkbox behaviour
        addCheckStateListener(new ICheckStateListener() {
			public void checkStateChanged(CheckStateChangedEvent event) {
				if (event.getChecked()) {
			        // when a compound module is checked, uncheck its subtree and all parents up to the root  
					ModuleTreeItem e = (ModuleTreeItem)event.getElement();
					setSubtreeChecked(e, false);
					setChecked(e, true);
					// all parents should be unchecked
				    for (ModuleTreeItem current=e.getParentModule(); current!=null; current=current.getParentModule())
				    	setChecked(current, false);
				}
				else {
					// when unchecked, expand and check all submodules
					ModuleTreeItem e = (ModuleTreeItem)event.getElement();
					setSubtreeChecked(e, false);
					setExpandedState(e, true);
					for (ModuleTreeItem i : e.getSubmodules())
						setChecked(i, true);
				}
			}
        });
        
        setInput(root);
        expandAll();

        if (((ModuleTreeItem)root).getSubmodules().length > 0)
        	reveal(((ModuleTreeItem)root).getSubmodules()[0]); // scroll to top
	}
}
