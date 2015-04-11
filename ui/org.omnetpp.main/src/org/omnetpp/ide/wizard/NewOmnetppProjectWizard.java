/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide.wizard;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IContainer;
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
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedWizard;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * "New OMNeT++ Project" wizard. (This wizard only appears on the UI if the org.omnetpp.cdt
 * plug-in and the similar wizard contributed by it is absent.)
 *
 * @author Andras
 */
public class NewOmnetppProjectWizard extends TemplateBasedWizard implements INewWizard {
    private WizardNewProjectCreationPage projectCreationPage;

    public static class NewOmnetppProjectCreationPage extends WizardNewProjectCreationPage {
        public NewOmnetppProjectCreationPage() {
            super("omnetpp project");
            setTitle("OMNeT++ Project");
            setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/full/wizban/newoprj_wiz.png"));
        }
    }

    public NewOmnetppProjectWizard() {
        setWindowTitle("New OMNeT++ Project");
        setWizardType("project");
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    @Override
    public void addPages() {
        addPage(projectCreationPage = createProjectCreationPage());
        projectCreationPage.setTitle(isImporting() ? "Import into OMNeT++ Project" : "New OMNeT++ Project");
        projectCreationPage.setDescription(isImporting() ? "Import into a new OMNeT++ project" : "Create a new OMNeT++ Project");

        super.addPages();
    }

    /**
     * Override if you want to customize the first page
     */
    protected WizardNewProjectCreationPage createProjectCreationPage() {
        return new NewOmnetppProjectCreationPage();
    }

    protected WizardNewProjectCreationPage getProjectCreationPage() {
        return projectCreationPage;
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        return getProjectCreationPage().getProjectHandle();
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        context.setVariableIfMissing("nedPackageName", "${projectname}"); // identifier, with all lowercase

        // variables to help support project, simulation and file wizards with the same template code.
        // Intention: targetTypeName and targetMainFile are the NED type and file of "the network"
        // in this project; these variables are to be used only in projects where it makes sense.
        context.setVariableIfMissing("targetTypeName", "${projectName}"); // identifier, with upper case first letter
        context.setVariableIfMissing("targetMainFile", "${targetTypeName}.ned"); // let targetTypeName be edited on pages
        return context;
    }

    @Override
    public boolean performFinish() {
        boolean ok = createNewProject(); // create empty project
        if (!ok)
            return false;
        return super.performFinish(); // add templated files
    }

    protected boolean createNewProject() {
        String projectName = getProjectCreationPage().getProjectName();
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        final IProject projectHandle = workspace.getRoot().getProject(projectName);

        // get a project descriptor
        IPath defaultPath = Platform.getLocation();
        IPath newPath = getProjectCreationPage().getLocationPath();
        if (defaultPath.equals(newPath))
            newPath = null;

        final IProjectDescription description = workspace.newProjectDescription(projectName);
        description.setLocation(newPath);

        // define the operation to create a new project
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                try {
                    monitor.beginTask("", 2000);

                    // create project description. Note: we'add the nature after project creation,
                    // so that builders get properly configured (Project.create() doesn't do it).
                    projectHandle.create(description, new SubProgressMonitor(monitor, 1000));
                    if (monitor.isCanceled())
                        throw new OperationCanceledException();

                    projectHandle.open(IResource.BACKGROUND_REFRESH, new SubProgressMonitor(monitor, 1000));

                    // add the project nature after now, after project creation, so that builders
                    // get properly configured (Project.create() doesn't do it).
                    ProjectUtils.addOmnetppNature(projectHandle, monitor);
                }
                finally {
                    monitor.done();
                }
            }
        };

        // run the operation to create a new project
        try {
            getContainer().run(true, true, op);
        } catch (InterruptedException e) {
            return false;
        } catch (InvocationTargetException e) {
            Throwable t = e.getTargetException();
            if (t instanceof CoreException) {
                if (((CoreException) t).getStatus().getCode() == IResourceStatus.CASE_VARIANT_EXISTS) {
                    MessageDialog.openError(getShell(), "Creation problems",
                            "The project contents directory specified is being used by another project. " +
                            "Please enter a new project contents directory.");
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
            return false;
        }
        return true;
    }
}
