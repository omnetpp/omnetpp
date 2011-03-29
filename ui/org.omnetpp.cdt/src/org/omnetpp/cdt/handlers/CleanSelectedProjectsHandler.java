package org.omnetpp.cdt.handlers;

import java.util.ArrayList;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.model.ICProject;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.core.commands.IHandlerListener;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.handlers.HandlerUtil;

/**
 * Implements the "Clean Local" action. This action cleans exactly the selected projects,
 * and not their referenced projects or projects that reference the selected ones.
 * 
 * @author rhornig
 */
public class CleanSelectedProjectsHandler implements IHandler {
	
	public Object execute(ExecutionEvent ev) throws ExecutionException {
		final String JOB_NAME = "Cleaning selected projects";
		final ExecutionEvent event = ev;
		WorkspaceJob cleanJob = new WorkspaceJob(JOB_NAME) {
            public boolean belongsTo(Object family) {
                return ResourcesPlugin.FAMILY_MANUAL_BUILD.equals(family);
            }
            
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
        		try {
        			ISelection selection = HandlerUtil.getCurrentSelectionChecked(event);
        			IProject[] selectedProjects = getSelectedCdtProjects(selection);
                    monitor.beginTask(JOB_NAME, selectedProjects.length);
                    for (int i = 0; i < selectedProjects.length; i++) {
                        ((IProject) selectedProjects[i]).build(
                                IncrementalProjectBuilder.CLEAN_BUILD,
                                new SubProgressMonitor(monitor, 1));
                    }
                } catch (ExecutionException e) {
					// just skip the command execution (no selection present)
				} finally {
                    monitor.done();
                }
                //see if a build was requested
                return Status.OK_STATUS;
            }
        };
        cleanJob.setRule(ResourcesPlugin.getWorkspace().getRuleFactory().buildRule());
        cleanJob.setUser(true);
        cleanJob.schedule();
		
		return null;
	}

	public static IProject[] getSelectedCdtProjects(ISelection selection) {
		ArrayList<IProject> projects = new ArrayList<IProject>();
		if (!selection.isEmpty() && selection instanceof IStructuredSelection) {
			Object[] selected = ((IStructuredSelection)selection).toArray();
			if (selected.length > 0) {
				for (Object sel : selected) {
					IProject prj = null;
					if (sel instanceof IProject)
						prj = (IProject)sel;
					else if (sel instanceof ICProject)
						prj = ((ICProject)sel).getProject();
	
					if (prj != null && CoreModel.getDefault().isNewStyleProject(prj)) {
						projects.add(prj);
					}
				}
			}
		}
		return projects.toArray(new IProject[projects.size()]);
	}

	public boolean isEnabled() {
		return true;
	}

	public boolean isHandled() {
		return true;
	}

	public void addHandlerListener(IHandlerListener handlerListener) {
	}

	public void removeHandlerListener(IHandlerListener handlerListener) {
	}

	public void dispose() {
	}

}
