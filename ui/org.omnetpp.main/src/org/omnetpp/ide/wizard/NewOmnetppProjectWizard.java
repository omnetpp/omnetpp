package org.omnetpp.ide.wizard;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceStatus;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

public class NewOmnetppProjectWizard extends Wizard implements INewWizard {

	protected NewOmnetppProjectCreationPage projectPage;

	public static class NewOmnetppProjectCreationPage extends WizardNewProjectCreationPage {

	    public NewOmnetppProjectCreationPage() {
	        super("OMNeT++ Project");
	        setTitle("OMNeT++ Project");
	        setDescription("Creates a new OMNeT++ Project.");
	        setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newoprj_wiz.png"));
	    }
	}

	public NewOmnetppProjectWizard() {
		setWindowTitle("New OMNeT++ Project");
	}

	public void init(IWorkbench workbench, IStructuredSelection selection) {
	}

	@Override
	public void addPages() {
		projectPage = new NewOmnetppProjectCreationPage();
		addPage(projectPage);
	}

	@Override
	public boolean performFinish() {
        IProject project = createNewProject();
        return project != null;
	}

	protected IProject createNewProject() {
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		String projectName = projectPage.getProjectName();
        final IProject projectHandle =  workspace.getRoot().getProject(projectName);

        // get a project descriptor
        IPath defaultPath = Platform.getLocation();
        IPath newPath = projectPage.getLocationPath();
        if (defaultPath.equals(newPath))
			newPath = null;

        // create project description. Note: we'add the nature after project creation,
        // so that builders get properly configured (Project.create() doesn't do it).
        final IProjectDescription description = workspace.newProjectDescription(projectName);
        description.setLocation(newPath);

        // define the operation to create a new project
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                createOmnetppProject(description, projectHandle, monitor);
            }
        };

        // run the operation to create a new project
        try {
            getContainer().run(true, true, op);
        } catch (InterruptedException e) {
            return null;
        } catch (InvocationTargetException e) {
            Throwable t = e.getTargetException();
            if (t instanceof CoreException) {
                if (((CoreException) t).getStatus().getCode() == IResourceStatus.CASE_VARIANT_EXISTS) {
                    MessageDialog.openError(getShell(), "Creation problems",
                    		"The project contents directory specified is being used by another project. Please enter a new project contents directory.");
                }
                else {
                    ErrorDialog.openError(getShell(), "Creation problems", null, ((CoreException) t).getStatus());
                }
            }
            else {
                // Unexpected runtime exceptions and errors may still occur.
            	OmnetppMainPlugin.logError(t);
                MessageDialog.openError(getShell(), "Creation problems", "Internal error: " + t.getMessage());
            }
            return null;
        }

        return projectHandle;
	}

    private void createOmnetppProject(IProjectDescription description, IProject projectHandle, IProgressMonitor monitor) throws CoreException, OperationCanceledException {
        try {
            monitor.beginTask("", 2000);

            projectHandle.create(description, new SubProgressMonitor(monitor, 1000));
            if (monitor.isCanceled())
				throw new OperationCanceledException();

            projectHandle.open(IResource.BACKGROUND_REFRESH, new SubProgressMonitor(monitor, 1000));

            // add the project nature after now, after project creation, so that builders
            // get properly configured (Project.create() doesn't do it).
            ProjectUtils.addOmnetppNature(projectHandle);
        }
        finally {
            monitor.done();
        }
    }

}
