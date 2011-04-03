package org.omnetpp.cdt.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
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
        IWorkbenchPart part = workbenchPage == null ? null : workbenchPage.getActivePart();
        Object selection = null;
        if (part instanceof IEditorPart) {
            selection = ((IEditorPart) part).getEditorInput();
        } else {
            ISelection sel = workbenchWindow.getSelectionService().getSelection();
            if ((sel != null) && (sel instanceof IStructuredSelection))
                selection = ((IStructuredSelection) sel).getFirstElement();
        }
        if (selection == null)
            return null;
        if (!(selection instanceof IAdaptable))
            return null;
        IResource resource = (IResource) ((IAdaptable) selection).getAdapter(IResource.class);
        if (resource == null)
            return null;
        return resource.getProject();
    }
    
}
