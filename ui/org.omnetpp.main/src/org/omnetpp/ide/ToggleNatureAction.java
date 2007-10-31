package org.omnetpp.ide;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;

/**
 * Add/remove OMNeT++ nature to the selected project.
 * 
 * @author Andras
 */
//FIXME "Add Nature" should rather be a wizard? Like CDT's "Convert to C++ project" wizard. What about "Remove Nature"?
public class ToggleNatureAction implements IObjectActionDelegate {
    private ISelection selection;

    /* (non-Javadoc)
     * @see org.eclipse.ui.IActionDelegate#run(org.eclipse.jface.action.IAction)
     */
    @SuppressWarnings("unchecked")
    public void run(IAction action) {
        List<IProject> projects = getSelectedOpenProjects();
        if (anyProjectMissesNature(projects)) {
            // add OMNeT++ Nature
            for (IProject project : projects)
                if (!hasNature(project))
                    addNature(project);
        }
        else {
            // remove OMNeT++ Nature
            for (IProject project : projects)
                if (hasNature(project))
                    removeNature(project);
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
            action.setText("Add OMNeT++ Nature");
        else
            action.setText("Remove OMNeT++ Nature");
    }

    /* (non-Javadoc)
     * @see org.eclipse.ui.IObjectActionDelegate#setActivePart(org.eclipse.jface.action.IAction,
     *      org.eclipse.ui.IWorkbenchPart)
     */
    public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    }

    private boolean hasNature(IProject project) {
        try {
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            return ArrayUtils.contains(natures, OmnetppNature.NATURE_ID);
        } 
        catch (CoreException e) {
            Activator.getDefault().logError(e);
            return false;
        }
    }
    
    @SuppressWarnings("unchecked")
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
            if (!hasNature(project))
                return true;
        return false;
    }

    private void addNature(IProject project) {
        try {
            Assert.isTrue(!hasNature(project));
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            description.setNatureIds((String[])ArrayUtils.add(natures, OmnetppNature.NATURE_ID));
            project.setDescription(description, null);
        } 
        catch (CoreException e) {
            Activator.getDefault().logError(e);
        }
    }

    private void removeNature(IProject project) {
        try {
            Assert.isTrue(hasNature(project));
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            description.setNatureIds((String[])ArrayUtils.removeElement(natures, OmnetppNature.NATURE_ID));
            project.setDescription(description, null);
        } 
        catch (CoreException e) {
            Activator.getDefault().logError(e);
        }
    }
}
