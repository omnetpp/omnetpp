package org.omnetpp.launch;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;


/**
 * Start or (create and start) a LaunchCofiguration associated with a selected INI file 
 * (INI file can be selected by clicking in the explorer or can be the active editor input)  
 *
 * @author rhornig
 */
public class IniFileSimulationShortcut implements ILaunchShortcut {

	public void launch(ISelection selection, String mode) {
		if (selection instanceof IStructuredSelection ) {
			Object obj =((IStructuredSelection)selection).getFirstElement();
			if (obj instanceof IFile) {
				IFile file = (IFile)obj;
				if ("ini".equals(file.getFileExtension()))
					searchAndLaunch(file, mode);
			}
		}
	}

	public void launch(IEditorPart editor, String mode) {
		if (editor != null && editor.getEditorInput() instanceof IFileEditorInput) {
			searchAndLaunch(((IFileEditorInput)editor.getEditorInput()).getFile(), mode);
		}
	}
	
	public void searchAndLaunch(IFile iniFile, String mode) {
		try {
			ILaunchConfiguration lc = chooseLaunchConfig(iniFile, mode);
			if (lc == null)
				lc = createLaunchConfig(iniFile);
			
			if (lc != null)
				lc.launch(mode, new NullProgressMonitor());
			
		} catch (CoreException e) {
			LaunchPlugin.logError(e);
		}
	}

	protected ILaunchConfiguration createLaunchConfig(IFile iniFile)
			throws CoreException {
		String name = getLaunchManager().generateUniqueLaunchConfigurationNameFrom(iniFile.getProject().getName());
		ILaunchConfigurationWorkingCopy wc = getLaunchConfigurationType().newInstance(null, name);

		IFile exeFile = chooseExecutable(iniFile.getProject());
		if (exeFile == null) return null;
		
		wc.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, "${workspace_loc:"+iniFile.getParent().getFullPath().toString()+"}");
		wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, exeFile.getProject().getFullPath().toString());
		wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, exeFile.getProjectRelativePath().toString());
		wc.setAttribute(IOmnetppLaunchConstants.ATTR_SHOWDEBUGVIEW, false);
    	wc.setAttribute(IOmnetppLaunchConstants.ATTR_RUNNUMBER_FOR_DEBUG, "");
    	wc.setAttribute(IOmnetppLaunchConstants.ATTR_RUNNUMBER, "");
    	wc.setAttribute(IOmnetppLaunchConstants.ATTR_NUM_CONCURRENT_PROCESSES, 1);
    	wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "-n ${ned_path:/"+iniFile.getProject().getName()+"} "+iniFile.getName());
		wc.setMappedResources(new IResource[] {iniFile});
		
		return wc.doSave();
	}

	protected ILaunchConfiguration chooseLaunchConfig(IFile iniFile, String mode) throws CoreException {
		ILaunchConfiguration[] lConfigs = getLaunchManager().getLaunchConfigurations(getLaunchConfigurationType());
		List<ILaunchConfiguration> matchingConfigs = new ArrayList<ILaunchConfiguration>();
		for(ILaunchConfiguration config : lConfigs) 
			if(ArrayUtils.contains(config.getMappedResources(), iniFile))
				matchingConfigs.add(config);
		
        if (matchingConfigs.size() == 0)
        	return null;
        if (matchingConfigs.size() == 1)
        	return matchingConfigs.get(0);
        
        ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
        dialog.setLabelProvider(new LabelProvider() {
        	@Override
        	public String getText(Object element) {
        		return ((ILaunchConfiguration)element).getName();
        	}
        		
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select a configuration");
        dialog.setMessage("Select the launch configuration that should be started.\n");
        dialog.setInput(matchingConfigs);
        
		if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
			return ((ILaunchConfiguration)dialog.getResult()[0]);
		}
		
		return null;
	}

	protected IFile chooseExecutable(IProject project) {
		final List<IFile> exeFiles = new ArrayList<IFile>();
        IProject[] projects = ProjectUtils.getAllReferencedProjects(project);
        projects = (IProject[]) ArrayUtils.add(projects, project);
        
        for (IProject pr : projects) {
        	try {
				pr.accept(new IResourceVisitor() {
					public boolean visit(IResource resource) {
						if (OmnetppLaunchUtils.isExecutable(resource))
							exeFiles.add((IFile)resource);
						return true;
					}
				});
			} catch (CoreException e) {
				LaunchPlugin.logError(e);
			}
        	
        }
        
        if (exeFiles.size() == 0)
        	return null;
        if (exeFiles.size() == 1)
        	return exeFiles.get(0);

        // ask the user select an exe file
        ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
        dialog.setLabelProvider(new WorkbenchLabelProvider() {
        	@Override
        	protected String decorateText(String input, Object element) {
        		if (element instanceof IResource)
        			return input + " - " + ((IResource)element).getParent().getFullPath().toString();
        		return input;
        	}
        		
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Executable File");
        dialog.setMessage("Select the executable file that should be started.\n");
        dialog.setInput(exeFiles);
        
		if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
			return ((IFile)dialog.getResult()[0]);
		}
		
		return null;
	}
	
	protected ILaunchConfigurationType getLaunchConfigurationType() {
		return getLaunchManager().getLaunchConfigurationType(IOmnetppLaunchConstants.SIMULATION_LAUNCH_CONFIGURATION_TYPE);
	}

	protected ILaunchManager getLaunchManager() {
		return DebugPlugin.getDefault().getLaunchManager();
	}

}
