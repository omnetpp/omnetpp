/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.project.ProjectUtils;

/**
 * Add/remove OMNeT++ nature to the selected project.
 *
 * @author Andras
 */
public class ToggleNatureAction implements IObjectActionDelegate {
    private ISelection selection;

    /* (non-Javadoc)
     * @see org.eclipse.ui.IActionDelegate#run(org.eclipse.jface.action.IAction)
     */
    public void run(IAction action) {
        try {
            List<IProject> projects = getSelectedOpenProjects();
            if (anyProjectMissesNature(projects)) {
                // add OMNeT++ Nature
                for (IProject project : projects)
                    if (!ProjectUtils.hasOmnetppNature(project))
                        ProjectUtils.addOmnetppNature(project, null);
            }
            else {
                // remove OMNeT++ Nature
                for (IProject project : projects)
                    if (ProjectUtils.hasOmnetppNature(project))
                        ProjectUtils.removeOmnetppNature(project);
            }
        } catch (CoreException e) {
            OmnetppMainPlugin.logError(e);
            ErrorDialog.openError(
                    Display.getCurrent().getActiveShell(),
                    "Error",
                    "Could not add or remove OMNeT++ nature.",
                    e.getStatus());
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IActionDelegate#selectionChanged()org.eclipse.jface.action.IAction,
     *      org.eclipse.jface.viewers.ISelection)
     */
    public void selectionChanged(IAction action, ISelection selection) {
        this.selection = selection;
        List<IProject> projects = getSelectedOpenProjects();

        if (action.isEnabled() == projects.isEmpty())
            action.setEnabled(!projects.isEmpty());

        if (anyProjectMissesNature(projects))
            action.setText("Add OMNeT++ Support");
        else
            action.setText("Remove OMNeT++ Support");
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IObjectActionDelegate#setActivePart(org.eclipse.jface.action.IAction,
     *      org.eclipse.ui.IWorkbenchPart)
     */
    public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    }

    @SuppressWarnings("rawtypes")
    private List<IProject> getSelectedOpenProjects() {
        List<IProject> projects = new ArrayList<IProject>();
        if (selection instanceof IStructuredSelection) {
            for (Iterator it = ((IStructuredSelection) selection).iterator(); it.hasNext();) {
                Object element = it.next();
                IProject project = null;
                if (element instanceof IProject)
                    project = (IProject) element;
                else if (element instanceof IAdaptable)
                    project = (IProject) ((IAdaptable) element).getAdapter(IProject.class);
                if (project != null && project.isOpen())
                    projects.add(project);
            }
        }
        return projects;
    }

    private boolean anyProjectMissesNature(List<IProject> projects) {
        for (IProject project : projects)
            if (!ProjectUtils.hasOmnetppNature(project))
                return true;
        return false;
    }
}
