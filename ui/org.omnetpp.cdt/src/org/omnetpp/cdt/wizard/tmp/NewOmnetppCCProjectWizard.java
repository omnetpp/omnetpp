package org.omnetpp.cdt.wizard.tmp;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionManager;
import org.eclipse.cdt.core.settings.model.extension.CConfigurationData;
import org.eclipse.cdt.managedbuilder.core.IBuilder;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.core.IProjectType;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.internal.core.Configuration;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedProject;
import org.eclipse.cdt.managedbuilder.internal.core.ToolChain;
import org.eclipse.cdt.managedbuilder.ui.wizards.CfgHolder;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.text.ICCompletionProposal;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceStatus;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

import org.omnetpp.common.IConstants;
import org.omnetpp.ide.OmnetppMainPlugin;

public class NewOmnetppCCProjectWizard extends Wizard implements INewWizard {
	
	NewOmnetppCCProjectWizardPage page;

	public NewOmnetppCCProjectWizard() {
		setWindowTitle("New OMNeT++ project");
	}

	public void init(IWorkbench workbench, IStructuredSelection selection) {
	}

	@Override
	public void addPages() {
		page = new NewOmnetppCCProjectWizardPage();
		addPage(page);
	}

	@Override
	public boolean performFinish() {
        IProject project = createNewProject();
        try {
//            IProjectType pt = ManagedBuildManager.getProjectType("cdt.managedbuild.target.gnu.mingw.exe");
//            IManagedProject mp = ManagedBuildManager.createManagedProject(project, pt);
            CCorePlugin.getDefault().convertProjectToNewCC(project, ManagedBuildManager.CFG_DATA_PROVIDER_ID, new NullProgressMonitor());
            ICProjectDescription des = CCorePlugin.getDefault().getProjectDescription(project);
            ICConfigurationDescription aconf = des.getActiveConfiguration();
            aconf.setName("Release");
            aconf.setDescription("An omnet++ release configuration");
            
            CCorePlugin.getDefault().setProjectDescription(project, des);
            
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        return project != null;
	}
	
//    public void createStandardProject(IProject project)  throws CoreException {
//        ICProjectDescriptionManager mngr = CoreModel.getDefault().getProjectDescriptionManager();
//        ICProjectDescription des = mngr.createProjectDescription(project, false, true);
//        ManagedBuildInfo info = ManagedBuildManager.createBuildInfo(project);
//        ManagedProject mProj = new ManagedProject(des);
//        info.setManagedProject(mProj);
//
//        ICConfigurationDescription acfg = des.getActiveConfiguration();
//
//        CfgHolder cfgs[] = new CfgHolder()[] { new CfgHolder()};
//        cfgs = CfgHolder.unique(fConfigPage.getCfgItems(defaults));
//        cfgs = CfgHolder.reorder(cfgs);
//            
//        for (int i=0; i<cfgs.length; i++) {
//            String s = (cfgs[i].getToolChain() == null) ? "0" : ((ToolChain)(cfgs[i].getToolChain())).getId();  //$NON-NLS-1$
//            Configuration cfg = new Configuration(mProj, (ToolChain)cfgs[i].getToolChain(), ManagedBuildManager.calculateChildId(s, null), cfgs[i].getName());
//            IBuilder bld = cfg.getEditableBuilder();
//            if (bld != null) {
//                if(bld.isInternalBuilder()){
//                    IConfiguration prefCfg = ManagedBuildManager.getPreferenceConfiguration(false);
//                    IBuilder prefBuilder = prefCfg.getBuilder();
//                    cfg.changeBuilder(prefBuilder, ManagedBuildManager.calculateChildId(cfg.getId(), null), prefBuilder.getName());
//                    bld = cfg.getEditableBuilder();
//                    bld.setBuildPath(null);
//                }
//                bld.setManagedBuildOn(false);
//            } else {
//                System.out.println(UIMessages.getString("StdProjectTypeHandler.3")); //$NON-NLS-1$
//            }
//            cfg.setArtifactName(removeSpaces(project.getName()));
//            CConfigurationData data = cfg.getConfigurationData();
//            des.createConfiguration(ManagedBuildManager.CFG_DATA_PROVIDER_ID, data);
//        }
//        mngr.setProjectDescription(project, des);
//        
//        doPostProcess(project);
//    }
	
	
	
	private IProject createNewProject() {
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		String projectName = page.getProjectName();
        final IProject projectHandle =  workspace.getRoot().getProject(projectName);
        
        // get a project descriptor
        IPath defaultPath = Platform.getLocation();
        IPath newPath = page.getLocationPath();
        if (defaultPath.equals(newPath))
			newPath = null;
        
        // create project description. Note: we'add the nature after project creation,
        // so that builders get properly configured (Project.create() doesn't do it).
        final IProjectDescription description = workspace.newProjectDescription(projectName);
        description.setLocation(newPath);
		
        // define the operation to create a new project
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                createProject(description, projectHandle, monitor);
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
            	OmnetppMainPlugin.getDefault().logError(t);
                MessageDialog.openError(getShell(), "Creation problems", "Internal error: " + t.getMessage());
            }
            return null;
        }

        return projectHandle;
	}
	
    private void createProject(IProjectDescription description, IProject projectHandle, IProgressMonitor monitor) throws CoreException, OperationCanceledException {
        try {
            monitor.beginTask("", 2000);

            projectHandle.create(description, new SubProgressMonitor(monitor, 1000));
            if (monitor.isCanceled())
				throw new OperationCanceledException();

            projectHandle.open(IResource.BACKGROUND_REFRESH, new SubProgressMonitor(monitor, 1000));

            // add the project nature after now, after project creation, so that builders 
            // get properly configured (Project.create() doesn't do it).
            IProjectDescription description2 = projectHandle.getDescription();
            description2.setNatureIds(new String[] {IConstants.OMNETPP_NATURE_ID});
            projectHandle.setDescription(description2, monitor);
        } 
        finally {
            monitor.done();
        }
    }
}
