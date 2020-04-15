package org.omnetpp.cdt.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.PreferencesUtil;

/**
 * Implements the "Project Features" menu item, which opens the Project Properties dialog
 * with the Project Features page.
 *
 * @author Andras
 */
public class ProjectFeaturesHandler extends AbstractHandler {
    public static final String PROJECTFEATURES_ID = "org.omnetpp.cdt.ProjectFeatures";
    private static final String ID_CDT_PROJECTS = "org.eclipse.cdt.ui.CView"; // "C/C++ Projects" view

    public Object execute(ExecutionEvent event) throws ExecutionException {
        execute();
        return null;
    }

    public static void execute() {
        IProject project = getProject();
        if (project != null && project.isAccessible()) {
            PreferenceDialog dialog = PreferencesUtil.createPropertyDialogOn(Display.getDefault().getActiveShell(), project, PROJECTFEATURES_ID, null, null, 0);
            dialog.open();
            return;
        }
        MessageDialog.openError(Display.getDefault().getActiveShell(), "Project Features", "Please select an open OMNeT++ project first.");
    }

    /**
     * Returns a project from the selection of the active part. Method copied from
     * ProjectPropertyDialogAction (with minor editing).
     */
    private static IProject getProject() {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        IWorkbenchPage workbenchPage = workbenchWindow == null ? null : workbenchWindow.getActivePage();
        if (workbenchPage == null)
            return null;

        IWorkbenchPart part = workbenchPage.getActivePart();
        IProject project = null;

        if (part instanceof IEditorPart)
            project = getProjectFrom(((IEditorPart) part).getEditorInput());
        if (project == null)
            project = getProjectFrom(workbenchWindow.getSelectionService().getSelection());
        if (project == null) {
            IEditorPart activeEditor = workbenchPage.getActiveEditor();
            if (activeEditor != null)
                project = getProjectFrom(activeEditor.getEditorInput());
        }
        if (project == null)
            project = getProjectFrom(workbenchPage.getSelection(IPageLayout.ID_PROJECT_EXPLORER));
        if (project == null)
            project = getProjectFrom(workbenchPage.getSelection(ID_CDT_PROJECTS));
        if (project == null)
            project = getProjectFrom(workbenchPage.getSelection(IPageLayout.ID_RES_NAV));
        return project;
    }

    private static IProject getProjectFrom(Object selection) {
        if (selection == null)
            return null;
        if (selection instanceof IStructuredSelection)
            selection = ((IStructuredSelection) selection).getFirstElement();
        IResource resource = null;
        if (selection instanceof IResource)
            resource = (IResource) selection;
        if (resource == null && selection instanceof IAdaptable)
            resource = (IResource) ((IAdaptable) selection).getAdapter(IResource.class);
        if (resource == null && selection instanceof IAdaptable) {
            IMarker marker = (IMarker) ((IAdaptable) selection).getAdapter(IMarker.class);
            if (marker != null)
                resource = marker.getResource();
        }
        if (resource == null)
            return null;
        return resource.getProject();
    }

}
