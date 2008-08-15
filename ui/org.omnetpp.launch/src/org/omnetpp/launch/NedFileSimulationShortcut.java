package org.omnetpp.launch;

import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;


/**
 * Start or (create and start) a LaunchCofiguration associated with a selected NED file 
 * (NED file can be selected by clicking in the explorer or can be the active editor input)  
 *
 * @author andras
 */
public class NedFileSimulationShortcut implements ILaunchShortcut {

    private static class InifileConfig {
        IFile iniFile;
        String config;
    }

    public void launch(ISelection selection, String mode) {
        if (selection instanceof IStructuredSelection ) {
            Object obj =((IStructuredSelection)selection).getFirstElement();
            if (obj instanceof IFile) {
                IFile file = (IFile)obj;
                if ("ned".equals(file.getFileExtension()))
                    searchAndLaunch(file, mode);
            }
        }
    }

    public void launch(IEditorPart editor, String mode) {
        if (editor != null && editor.getEditorInput() instanceof IFileEditorInput) {
            searchAndLaunch(((IFileEditorInput)editor.getEditorInput()).getFile(), mode);
        }
    }

    public void searchAndLaunch(IFile nedFile, String mode) {
        try {
            ILaunchConfiguration lc = OmnetppLaunchUtils.findOrChooseLaunchConfigAssociatedWith(nedFile, mode);
            if (lc == null) {
                // find or create a matching ini file
                InifileConfig cfg = chooseOrCreateInifileConfig(nedFile, mode);
                
                if (cfg != null) {
                    // launch the ini file (and selected config)
                    IFile exeFile = IniFileSimulationShortcut.chooseExecutable(cfg.iniFile.getProject());
                    if (exeFile != null) 
                        lc = IniFileSimulationShortcut.createLaunchConfig(exeFile, cfg.iniFile, cfg.config, nedFile);
                }
            }

            if (lc != null)
                lc.launch(mode, new NullProgressMonitor());
        } 
        catch (CoreException e) {
            LaunchPlugin.logError(e);
            ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", 
                    "Error launching network in NED file", e.getStatus());
        }
    }

    protected InifileConfig chooseOrCreateInifileConfig(IFile nedFile, String mode) throws CoreException {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        if (!res.isNedFile(nedFile))
            return null; // not a ned file; FIXME throw error???

        List<INEDTypeInfo> networks = getNetworksInNedFile(nedFile, res);
        if (networks.isEmpty())
            return null; // FIXME dialog: no network???

        List<InifileConfig> candidates = new ArrayList<InifileConfig>();
        for (INEDTypeInfo network : networks) 
            candidates.addAll(collectInifileConfigsForNetwork(network));

        if (candidates.isEmpty())
            return askAndCreateInifile(networks, nedFile);
        else if (candidates.size() == 1)
            return candidates.get(0);
        else
            return chooseInifileConfigFromDialog(candidates);
    }

    protected List<INEDTypeInfo> getNetworksInNedFile(IFile nedFile, NEDResources res) {
        List<INEDTypeInfo> result = new ArrayList<INEDTypeInfo>();
        NedFileElementEx nedFileElement = res.getNedFileElement(nedFile);
        for (INEDElement child : nedFileElement)
            if (child instanceof IModuleTypeElement && ((IModuleTypeElement)child).isNetwork())
                result.add(((IModuleTypeElement)child).getNEDTypeInfo());
        return result;
    }

    protected List<InifileConfig> collectInifileConfigsForNetwork(INEDTypeInfo network) {
        // TODO Auto-generated method stub
        return new ArrayList<InifileConfig>();
    }

    protected InifileConfig askAndCreateInifile(List<INEDTypeInfo> networks, IFile nedFile) throws CoreException {
        String networkNames = "";
        for (INEDTypeInfo network : networks)
            networkNames += (networkNames.length()==0 ? "" : ", ") + network.getName();

        String text = networks.size()==1 ? 
                "Network " + networkNames + " does not have an associated ini file. Do you want to create one now?" :
                    "None of the networks in " +nedFile.getName() + " (" + networkNames + ") have an associated ini file. Do you want to create one now?";

        if (!MessageDialog.openConfirm(null, "Create Ini File", text)) 
            return null; // user cancelled

        // choose network to create ini file for
        INEDTypeInfo selectedNetwork = networks.get(0);
        if (networks.size() > 1) {
            // choose network from dialog
            ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
            dialog.setLabelProvider(new LabelProvider() {
                @Override
                public String getText(Object element) {
                    INEDTypeInfo network = (INEDTypeInfo)element;
                    return network.getName();
                }
            });
            dialog.setContentProvider(new ArrayContentProvider());
            dialog.setTitle("Select Network");
            dialog.setMessage("Select a network from " + nedFile.getName().toString() + ":");
            dialog.setInput(networks);

            if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0)
                selectedNetwork = ((INEDTypeInfo)dialog.getResult()[0]);
            else 
                return null; // user cancelled
        }

        // generate new ini file with a good name for the selected network
        IContainer container = nedFile.getParent();
        String inifileName = generateGoodInifileName(container, selectedNetwork);
        IFile iniFile = container.getFile(new Path(inifileName));
        return createInifile(iniFile, selectedNetwork);
    }

    protected String generateGoodInifileName(IContainer container, INEDTypeInfo network) {
        if (!container.getFile(new Path("omnetpp.ini")).exists())
            return "omnetpp.ini";

        String nameBase = StringUtils.uncapitalize(network.getName());
        if (!container.getFile(new Path(nameBase+".ini")).exists())
            return nameBase+".ini";

        int suffix = 2;
        while (container.getFile(new Path(nameBase+suffix+".ini")).exists())
            suffix++;
        return nameBase+suffix+".ini"; 
    }

    protected InifileConfig createInifile(IFile iniFile, INEDTypeInfo network) throws CoreException {
        String content = 
            "[General]\n" +
            "network = " + network.getName() + "\n" +
            "**.apply-default = true\n";
        iniFile.create(new ByteArrayInputStream(content.getBytes()), false, new NullProgressMonitor());

        InifileConfig result = new InifileConfig();
        result.iniFile = iniFile;
        result.config = "General";
        return result;
    }

    protected InifileConfig chooseInifileConfigFromDialog(List<InifileConfig> candidates) {
        ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                InifileConfig cfg = (InifileConfig)element;
                return cfg.iniFile.getFullPath().toString() + " - " + cfg.config;
            }
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Ini file and Config");
        dialog.setMessage("Select ini file and configuration to start.\n");
        dialog.setInput(candidates);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((InifileConfig)dialog.getResult()[0]);
        }
        return null;
    }

    // FIXME there's a copy in IniFileSimulationShortcut -- factor out!
    protected ILaunchConfiguration createLaunchConfigForInifile(IFile iniFile, String config) throws CoreException {
        String name = getLaunchManager().generateUniqueLaunchConfigurationNameFrom(iniFile.getProject().getName());
        ILaunchConfigurationWorkingCopy wc = getLaunchConfigurationType().newInstance(null, name);

        IFile exeFile = chooseExecutable(iniFile.getProject());
        if (exeFile == null) return null;

        wc.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, "${workspace_loc:"+iniFile.getParent().getFullPath().toString()+"}");
        wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, exeFile.getProject().getFullPath().toString());
        wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, exeFile.getProjectRelativePath().toString());
        wc.setAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, false);
        wc.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, "");
        wc.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, "");
        wc.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        wc.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "-n ${ned_path:/"+iniFile.getProject().getName()+"} "+iniFile.getName());
        wc.setMappedResources(new IResource[] {iniFile});

        return wc.doSave();
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
