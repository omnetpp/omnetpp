package org.omnetpp.common.wizard.support;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * Allows the user to select a workspace folder, using a tree control.
 *
 * @author Andras
 */
public class FolderSelectionTree extends Composite implements IWidgetAdapter {
    private TreeViewer treeViewer;

    public FolderSelectionTree(Composite parent, int treeStyle) {
        super(parent, SWT.NONE);
        setLayout(new FillLayout());
        treeViewer = new TreeViewer(this, treeStyle);

        treeViewer.setLabelProvider(new WorkbenchLabelProvider());
        treeViewer.setContentProvider(new WorkbenchContentProvider() {
            @SuppressWarnings({"rawtypes", "unchecked"})
            @Override
            public Object[] getChildren(Object element) {
                Object[] children = super.getChildren(element);
                List result = new ArrayList();
                for (Object child : children)
                    if (!(child instanceof IFile)) // skip files
                        if (!(child instanceof IProject) || ((IProject)child).isOpen()) // skip closed projects
                            result.add(child);
                return result.toArray();
            }
        });
        treeViewer.setInput(ResourcesPlugin.getWorkspace().getRoot());
        treeViewer.setComparator(new ResourceComparator(ResourceComparator.NAME));
    }

    @Override
    public boolean setFocus() {
        return treeViewer.getTree().setFocus();
    }

    public TreeViewer getTreeViewer() {
        return treeViewer;
    }

    /**
     * Adapter interface.
     */
    public Object getValue() {
        IStructuredSelection selection = (IStructuredSelection)treeViewer.getSelection();
        IContainer container = (IContainer) selection.getFirstElement();
        return container == null ? "" : container.getFullPath().toString();
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value) {
        IContainer container;
        if (value instanceof IContainer) {
            container = (IContainer)value;
        }
        else {
            Path path = new Path(value.toString());
            IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
            container = path.segmentCount()==1 ? root.getProject(path.toString()) : root.getFolder(path);
        }
        if (container.exists()) {
            StructuredSelection selection = new StructuredSelection(container);
            treeViewer.setSelection(selection);
        }
        else {
            treeViewer.setSelection(new StructuredSelection());
        }
    }

}
