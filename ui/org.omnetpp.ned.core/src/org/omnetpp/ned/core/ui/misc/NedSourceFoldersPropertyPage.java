/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.ui.misc;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.project.NedSourceFoldersConfiguration;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.ned.core.NedResourcesPlugin;

/**
 * This property page is shown for OMNeT++ Projects (projects with the
 * omnetpp nature), and lets the user edit the contents of the ".nedfolders"
 * file.
 *
 * @author Andras
 */
public class NedSourceFoldersPropertyPage extends PropertyPage {
    private CheckboxTreeViewer folderTree;
    private EditableList excludedPackagesList;
    private NedSourceFoldersConfiguration editedConfig;

    /**
     * Constructor.
     */
    public NedSourceFoldersPropertyPage() {
        super();
    }

    /**
     * @see PreferencePage#createContents(Composite)
     */
    protected Control createContents(Composite parent) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1, false));

        final IProject project = getProject();

        createWrapLabel(composite,
                "NED Source Folders are root folders for the NED package hierarchy. " +
                "A NED file with the package declaration 'org.foo.bar' must be in the " +
                "'org/foo/bar' subdirectory, unless the NED Source Folder directly " +
                "contains a 'package.ned' file with a package declaration. In the latter " +
                "case, 'package.ned' defines what package the NED Source Folder root " +
                "corresponds to. Other NED files in that folder and subfolders must be " +
                "consistent with that declaration (i.e. only the root 'package.ned' has " +
                "special meaning.)",
                2, 300);

        Composite upperPart = new Composite(composite, SWT.NONE);
        upperPart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        upperPart.setLayout(new GridLayout(1, false));

        // Source folders tree
        Label label = new Label(upperPart, SWT.NONE);
        label.setText("&NED Source Folders for project '" + project.getName() + "':");
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        folderTree = new CheckboxTreeViewer(upperPart, SWT.BORDER);
        folderTree.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)folderTree.getTree().getLayoutData()).heightHint = convertHeightInCharsToPixels(8);

        Composite lowerPart = new Composite(composite, SWT.NONE);
        lowerPart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        lowerPart.setLayout(new GridLayout(1,false));

        // Excluded packages
        label = new Label(lowerPart, SWT.NONE);
        label.setText("&Excluded package subtrees:");
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        excludedPackagesList = new EditableList(lowerPart, SWT.NONE);
        excludedPackagesList.setAddDialogMessage("Package name to exclude:");
        excludedPackagesList.setEditDialogMessage("Package name to exclude:");
        excludedPackagesList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)excludedPackagesList.getLayoutData()).heightHint = convertHeightInCharsToPixels(8);

        // configure package tree
        folderTree.setLabelProvider(new WorkbenchLabelProvider());

        folderTree.setContentProvider(new WorkbenchContentProvider() {
            @Override
            public Object[] getChildren(Object element) {
                List<Object> filteredChildren = new ArrayList<Object>();
                for (Object child : super.getChildren(element)) {
                    if ((child==project || child instanceof IFolder) && !((IResource)child).getName().startsWith("."))
                        filteredChildren.add(child);
                }
                return filteredChildren.toArray();
            }
        });

        folderTree.addCheckStateListener(new ICheckStateListener() {
            public void checkStateChanged(CheckStateChangedEvent event) {
                if (event.getChecked()) {
                    // when a folder is checked, uncheck its subtree and all parents up to the root
                    IResource e = (IResource)event.getElement();
                    folderTree.setSubtreeChecked(e, false);
                    folderTree.setChecked(e, true);
                    for (IResource current=e.getParent(); current!=null; current=current.getParent())
                        folderTree.setChecked(current, false);
                }

                // update working copy
                IContainer[] folders = (IContainer[]) ArrayUtils.addAll(new IContainer[]{}, folderTree.getCheckedElements());
                editedConfig.setSourceFolders(folders);
            }
        });

        folderTree.setInput(project.getParent());

        excludedPackagesList.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                String[] excludedPackages = excludedPackagesList.getItems();
                editedConfig.setExcludedPackages(excludedPackages);
            }
        });

        // load configuration and populate page
        try {
            editedConfig = NedResourcesPlugin.getNedSourceFoldersEditingManager().getConfig(getContainer(), getProject());
        }
        catch (Exception e) {
            errorDialog("Cannot read NED Source Folders list: ", e);
            editedConfig = new NedSourceFoldersConfiguration(new IContainer[0], new String[0]);
        }

        for (IContainer folder : editedConfig.getSourceFolders()) {
            folderTree.setChecked(folder, true);
            for (IResource current=folder.getParent(); current!=null; current=current.getParent())
                folderTree.setExpandedState(current, true);
        }
        if (folderTree.getCheckedElements().length == 0)
            folderTree.setChecked(getProject(), true);

        String[] excludedPackages = editedConfig.getExcludedPackages();
        Arrays.sort(excludedPackages);
        excludedPackagesList.setItems(excludedPackages);

        return composite;
    }

    private IProject getProject() {
        return (IProject) getElement().getAdapter(IProject.class);
    }

    protected Label createWrapLabel(Composite parent, String text, int hspan, int wrapwidth) {
        // code from SWTFactory (debug.internal.ui)
        Label l = new Label(parent, SWT.NONE | SWT.WRAP);
        l.setFont(parent.getFont());
        l.setText(text);
        GridData gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.horizontalSpan = hspan;
        gd.widthHint = wrapwidth;
        l.setLayoutData(gd);
        return l;
    }

    protected Button createButton(Composite parent, String text, String tooltip) {
        Button button = new Button(parent, SWT.PUSH);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData());
        return button;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        if (visible == true) {
            // editedConfig may have been changed by other pages
            folderTree.setCheckedElements(editedConfig.getSourceFolders());
            excludedPackagesList.setItems(editedConfig.getExcludedPackages());
        }
    }

    protected void performDefaults() {
        folderTree.setCheckedElements(new Object[] { getProject() });
        editedConfig.setSourceFolders(new IContainer[] { getProject() });
    }

    public boolean performOk() {
        if (WorkspaceJob.getJobManager().find(ResourcesPlugin.FAMILY_MANUAL_BUILD).length != 0) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot update settings while build is in progress.");
            return false;
        }
        try {
            ProjectUtils.saveNedFoldersFile(getProject(), editedConfig);
            return true;
        }
        catch (CoreException e) {
            errorDialog("Could not store NED Source Folders list: ", e);
            return false;
        }
    }

    protected void errorDialog(String message, Throwable e) {
        IStatus status = new Status(IMarker.SEVERITY_ERROR, NedResourcesPlugin.PLUGIN_ID, e.getMessage(), e);
        ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
    }
}