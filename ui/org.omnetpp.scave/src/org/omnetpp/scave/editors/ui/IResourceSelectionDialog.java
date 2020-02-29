package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.collections.Predicate;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.util.StringUtils;

public class IResourceSelectionDialog extends ElementTreeSelectionDialog {

    private static final int PROJECT_OR_FOLDER = IResource.PROJECT | IResource.FOLDER;

    private int resourceType;
    private boolean allowCreateFolder;

    public IResourceSelectionDialog(Shell shell, int resourceType, boolean allowCreateFolder) {
        super(shell, new WorkbenchLabelProvider(), new TreeContentProvider(resourceType));
        Assert.isLegal(resourceType == IResource.FILE || resourceType == IResource.FOLDER ||
                       resourceType == IResource.PROJECT || resourceType == PROJECT_OR_FOLDER);
        this.resourceType = resourceType;
        this.allowCreateFolder = allowCreateFolder;

        String resourceTypeStr = resourceTypeAsString(resourceType);
        setTitle("Select " + resourceTypeStr);
        setMessage("Select " + resourceTypeStr.toLowerCase());
        setInput(ResourcesPlugin.getWorkspace().getRoot());
        setComparator(new ResourceComparator(ResourceComparator.NAME));
        setAllowMultiple(false);
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        if (allowCreateFolder) {
            Button newFolderButton = createButton(parent, -1, "New Folder...", false);
            newFolderButton.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    createFolder();
                }
            });
        }
        super.createButtonsForButtonBar(parent);
    }

    private static String resourceTypeAsString(int resourceType) {
        switch (resourceType) {
        case IResource.FILE: return "File";
        case IResource.FOLDER: return "Folder";
        case IResource.PROJECT: return "Project";
        case PROJECT_OR_FOLDER: return "Folder";
        default: Assert.isTrue(false); return null;
        }
    }

    public void setInitialSelection(IResource resource) {
        if (resourceType == IResource.FOLDER)
            while (!resource.exists())
                resource = resource.getParent();

        if (resource.exists() && resource.getType() == resourceType)
            super.setInitialSelection(resource);
    }

    public void setInitialSelection(String resourcePath) {
      if (!StringUtils.isEmpty(resourcePath)) {
          try {
              IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
              Path path = new Path(resourcePath);
              IResource resource = null;
              switch (resourceType) {
              case IResource.FILE: resource = root.getFile(path); break;
              case IResource.FOLDER: resource = root.getFolder(path); break;
              case IResource.PROJECT: resource = root.getProject(path.segment(0)); break;
              case PROJECT_OR_FOLDER: resource = path.segmentCount() < 2 ? root.getProject(path.segment(0)) : root.getFolder(path); break;
              }
              if (resource != null)
                  setInitialSelection(resource);
          }
          catch (IllegalArgumentException e) { } // on bad file name syntax
      }
    }

    public IResource getSelectedResource() {
      Object[] result = getResult();
      return result.length > 0 && result[0] instanceof IResource ? (IResource)result[0] : null;
    }

    private static class TreeContentProvider extends WorkbenchContentProvider {
        private final int resourceType;

        public TreeContentProvider(int resourceType) {
            this.resourceType = resourceType;
        }

        @Override
        public Object[] getChildren(Object element) {
            Object[] children = super.getChildren(element);
            if (element instanceof IFolder || element instanceof IProject) {
                List<Object> list = new ArrayList<Object>(Arrays.asList(children));
                CollectionUtils.filter(list, new Predicate() {
                    public boolean evaluate(Object object) {
                        switch (resourceType) {
                        case IResource.FILE: return object instanceof IContainer || object instanceof IFile;
                        case IResource.FOLDER:
                        case PROJECT_OR_FOLDER:
                            return object instanceof IContainer;
                        case IResource.PROJECT: return object instanceof IProject;
                        }
                        Assert.isTrue(false);
                        return false;
                    }
                });
                children = list.toArray();
            }
            return children;
        }
    }

    protected void createFolder() {
        IResource res = getSelectedResource();
        if (!(res instanceof IContainer)) {
            MessageDialog.openError(getShell(), "Error", "Select parent folder.");
            return;
        }

        IContainer parentFolder = (IContainer)res;
        if (!parentFolder.isAccessible()) {
            MessageDialog.openError(getShell(), "Error", "Selected folder is not accessible. (Project closed?)");
            return;
        }

        InputDialog inputDialog = new InputDialog(getShell(), "New Folder", "Folder name:", "folder", new IInputValidator() {
            @Override
            public String isValid(String newText) {
                if (!Path.isValidPosixSegment(newText))
                    return "Invalid name";
                return null;
            }
        });

        if (inputDialog.open() == Dialog.OK) {
            String name = inputDialog.getValue();
            try {
                parentFolder.getFolder(new Path(name)).create(true, true, null);
            } catch (Exception e) {
                MessageDialog.openError(getShell(), "Error", "Could not create folder '" + name + "' under '" + parentFolder.getFullPath() + "': " + e.getMessage());
            }
        }
    }
}
