package org.omnetpp.launch;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;
import org.omnetpp.launch.tabs.OmnetppMainTab;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;


/**
 * Start or (create and start) a LaunchCofiguration associated with a selected NED file 
 * (NED file can be selected by clicking in the explorer or can be the active editor input)
 * 
 * Steps:
 *   1. If there's already a launch config that's associated with this NED file, 
 *      launch that (Pop up list selection dialog if there's more than one);
 *      else:
 *      
 *   2. Collect network(s) from the NED file. If there's none --> error dialog, and stop.
 *   
 *   3. If there's already an ini file that runs one of those networks, run it
 *      using the Inifile Shortcut. (If there's more than one, use list selection
 *      dialog).
 *      
 *   4. Otherwise, offer the user to create an inifile. (When there's more than one inifile,
 *      user is asked using a list selection dialog). Run the newly created inifile 
 *      using the Inifile Shortcut.  
 *
 * @author andras
 */
public class SimulationLaunchShortcut implements ILaunchShortcut {
    public static final String PREF_SKIP_LAUNCHCONFIGCREATED_MESSAGE = "org.omnetpp.launch.SkipLaunchConfigCreatedMsg";

    protected static class InifileConfig {
        InifileConfig(IFile f, String c, String n) {iniFile=f; config=c; network=n;}
        IFile iniFile;
        String config;
        String network; 
    }

    public void launch(ISelection selection, String mode) {
        if (selection instanceof IStructuredSelection) {
            Object obj =((IStructuredSelection)selection).getFirstElement();
            if (obj instanceof IResource) {
                IResource resource = (IResource)obj;
                doLaunch(resource, mode);
            }
        }
    }

    public void launch(IEditorPart editor, String mode) {
        if (editor != null && editor.getEditorInput() instanceof IFileEditorInput) {
            doLaunch(((IFileEditorInput)editor.getEditorInput()).getFile(), mode);
        }
    }

    protected void doLaunch(IResource resource, String mode) {
        try {
            // find launch config already associated with the file
            ILaunchConfiguration lc = findOrChooseLaunchConfigAssociatedWith(resource, mode);
            if (lc == null) {
                // choose executable to launch
                IFile exeFile = chooseExecutable(resource.getProject());
                if (exeFile == null)
                    return; //FIXME what if opp_run + dll-based?

                // determine ini file and config to use for launching
                IFile iniFile = null;
                String configName = null;
                if (resource instanceof IFile && "ini".equals(resource.getFileExtension())) {
                    iniFile = (IFile)resource;
                }
                else if (resource instanceof IFile && "ned".equals(resource.getFileExtension())) {
                    // find or create a matching ini file
                    IFile nedFile = (IFile)resource;
                    InifileConfig iniFileAndConfig = chooseOrCreateIniFileForNedFile(nedFile, mode);
                    if (iniFileAndConfig == null)
                        return; // user cancelled
                    iniFile = iniFileAndConfig.iniFile;
                    configName = iniFileAndConfig.config;
                } 
                else {
                    return; // resource not supported
                }

                // create launch config based on the above data
                lc = createLaunchConfig(exeFile, iniFile, configName, resource);

                // tell novice users what happened
                IPreferenceStore preferences = LaunchPlugin.getDefault().getPreferenceStore();
                String pref = preferences.getString(PREF_SKIP_LAUNCHCONFIGCREATED_MESSAGE);
                if (!MessageDialogWithToggle.ALWAYS.equals(pref)) {
                    MessageDialogWithToggle.openInformation(
                            Display.getCurrent().getActiveShell(), 
                            "Launch Configuration Created", 
                            "A launch configuration named '" + lc.getName() + "' has been created, and associated " +
                            "with resource '" + resource.getName() + "'. You can modify or delete this launch configuration " +
                            "in the Run|Run Configurations... and Run|Debug Configurations... dialogs.", 
                            "Do not show this message again", false, 
                            preferences, PREF_SKIP_LAUNCHCONFIGCREATED_MESSAGE);
                }
            }

            if (lc != null)
                lc.launch(mode, new NullProgressMonitor());
        } 
        catch (CoreException e) {
            LaunchPlugin.logError(e);
            ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", 
                    "Error launching simulation for '" + resource.getFullPath() +"'", e.getStatus());
        }
    }

    /**
     * When the user selected a NED file for launch, we have to find a matching 
     * ini file (or files), or offer creating one.
     */
    protected InifileConfig chooseOrCreateIniFileForNedFile(IFile nedFile, String mode) throws CoreException {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        if (!res.isNedFile(nedFile)) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: " + nedFile.getName() + " is not a NED file, or it is not in a NED source folder.");
            return null;
        }

        List<INEDTypeInfo> networks = getNetworksInNedFile(nedFile);
        if (networks.isEmpty()) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: " + nedFile.getName() + " contains no network.");
            return null;
        }

        // collect ini files that instantiate any of these networks
        List<InifileConfig> candidates = collectInifileConfigsForNetworks(nedFile, networks);

        if (candidates.isEmpty())
            return askAndCreateInifile(nedFile, networks);
        else if (candidates.size() == 1)
            return candidates.get(0);
        else
            return chooseInifileConfigFromDialog(candidates, nedFile.getParent());
    }

    /**
     * Collects the networks from the given NED file
     */
    protected List<INEDTypeInfo> getNetworksInNedFile(IFile nedFile) {
        List<INEDTypeInfo> result = new ArrayList<INEDTypeInfo>();
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        NedFileElementEx nedFileElement = res.getNedFileElement(nedFile);
        for (INEDElement child : nedFileElement)
            if (child instanceof IModuleTypeElement && ((IModuleTypeElement)child).isNetwork())
                result.add(((IModuleTypeElement)child).getNEDTypeInfo());
        return result;
    }

    /**
     * Finds and returns all ini files that run the given network.
     */
    protected List<InifileConfig> collectInifileConfigsForNetworks(IFile nedFile, List<INEDTypeInfo> networks) {
        for (INEDTypeInfo network : networks)
            Assert.isTrue(network.getNEDFile().equals(nedFile)); // must be in the specified file

        // first, collect all inifiles from *this* project
        // (note: an inifile that refers to this network cannot be in a referenced 
        // project, as the network's NED type is not visible there!)
        final List<IFile> iniFiles = new ArrayList<IFile>();
        try {
            nedFile.getProject().accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (resource instanceof IFile && "ini".equals(resource.getFileExtension()))
                        iniFiles.add((IFile)resource);
                    return true;
                }
            });
        } catch (CoreException e) {
            LaunchPlugin.logError(e);
        }

        // convert network names to String[], for faster lookup 
        final String packagePrefix = networks.get(0).getNamePrefix(); // all networks are in the same file
        final String[] networkNames = new String[networks.size()];
        for (int i=0; i<networks.size(); i++)
            networkNames[i] = networks.get(i).getName();

        // now, find those inifiles that refer to this network
        final List<InifileConfig> result = new ArrayList<InifileConfig>();
        for (final IFile iniFile : iniFiles) {
            // parse the inifile
            try {
                final boolean isSameDir = iniFile.getParent().equals(nedFile.getParent());
                InifileParser inifileParser = new InifileParser();
                inifileParser.parse(iniFile, new InifileParser.ParserAdapter() {
                    String currentSection = null;
                    public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
                        currentSection = sectionName;
                    }
                    public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {
                        if (key.equals("network")) {
                            if (isSameDir && ArrayUtils.contains(networkNames, value))
                                result.add(new InifileConfig(iniFile, currentSection, value));
                            else if (value.startsWith(packagePrefix) && ArrayUtils.contains(networkNames, StringUtils.removeStart(value, packagePrefix)))
                                result.add(new InifileConfig(iniFile, currentSection, value));
                            //FIXME sections extending this one should also be added!
                        }
                    }
                });
            }
            catch (ParseException e) { }
            catch (CoreException e) { }
            catch (IOException e) { }

        }
        return result;
    }

    /**
     * Ask the user if s/he wants to create an ini file for one of the networks 
     * in the given NED file, then create the inifile. Returns null if user cancelled.
     */
    protected InifileConfig askAndCreateInifile(IFile nedFile, List<INEDTypeInfo> networks) throws CoreException {
        String networkNames = "";
        for (INEDTypeInfo network : networks) {
            Assert.isTrue(network.getNEDFile().equals(nedFile)); // must be in the specified file
            networkNames += (networkNames.length()==0 ? "" : ", ") + network.getName();
        }

        String text = networks.size()==1 ? 
                "Network " + networkNames + " does not have an associated ini file. Do you want to create one now?" :
                    "None of the networks in " +nedFile.getName() + " (" + networkNames + ") have an associated ini file. Do you want to create one now?";

        if (!MessageDialog.openConfirm(Display.getCurrent().getActiveShell(), "Create Ini File", text)) 
            return null; // user cancelled

        // choose network to create ini file for
        INEDTypeInfo selectedNetwork = networks.get(0);
        if (networks.size() > 1) {
            // choose network from dialog
            ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
            dialog.setLabelProvider(new NedModelLabelProvider());
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

    /**
     * Generates unique name for a new inifile which will run the given network.
     */
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

    /** 
     * Creates the given inifile with a network= setting to run the given network.
     * The file must NOT yet exist.
     */
    protected InifileConfig createInifile(IFile iniFile, INEDTypeInfo network) throws CoreException {
        String content = 
            "[General]\n" +
            "network = " + network.getName() + "\n" +
            "**.apply-default = true\n";
        iniFile.create(new ByteArrayInputStream(content.getBytes()), false, new NullProgressMonitor());

        return new InifileConfig(iniFile, "General", network.getName());
    }

    /**
     * Offer the user a selection dialog to choose an inifile+configName from a list.
     * Inifile names that are in the given default folder will be shown without path (just the name).
     * Returns null if user cancelled. 
     */
    protected InifileConfig chooseInifileConfigFromDialog(List<InifileConfig> candidates, final IContainer defaultDir) {
        ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
        final WorkbenchLabelProvider workbenchLabelProvider = new WorkbenchLabelProvider(); // cannot subclass, due to final methods
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                InifileConfig cfg = (InifileConfig)element;
                String friendlyFileName = defaultDir.equals(cfg.iniFile.getParent()) ? cfg.iniFile.getName() : cfg.iniFile.getFullPath().toString();
                return friendlyFileName + " - " + cfg.config + " - network: " + cfg.network;
            }
            @Override
            public Image getImage(Object element) {
                InifileConfig cfg = (InifileConfig)element;
                return workbenchLabelProvider.getImage(cfg.iniFile);
            }
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Ini file and Config");
        dialog.setMessage("Select ini file and config:\n");
        dialog.setInput(candidates);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((InifileConfig)dialog.getResult()[0]);
        }
        return null;
    }

    /**
     * Creates and saves a launch configuration with the given attributes. 
     */
    protected ILaunchConfiguration createLaunchConfig(IFile exeFile, IFile iniFile, String configName, IResource resourceToAssociateWith) throws CoreException {
        ILaunchManager launchManager = DebugPlugin.getDefault().getLaunchManager();
        ILaunchConfigurationType launchType = launchManager.getLaunchConfigurationType(IOmnetppLaunchConstants.SIMULATION_LAUNCH_CONFIGURATION_TYPE);
        String name = launchManager.generateUniqueLaunchConfigurationNameFrom(iniFile.getProject().getName());
        ILaunchConfigurationWorkingCopy wc = launchType.newInstance(null, name);

        OmnetppMainTab.prepareLaunchConfig(wc);

        wc.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, exeFile.getFullPath().toString());
        wc.setAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, iniFile.getParent().getFullPath().toString());
        wc.setAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, iniFile.getName());
        if (configName != null)
            wc.setAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, configName);
        if (resourceToAssociateWith != null)
            wc.setMappedResources(new IResource[] {resourceToAssociateWith});

        return wc.doSave();
    }

    /**
     * List selection dialog to choose an executable from this project and all 
     * referenced projects. Returns null if user cancelled.
     */
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
        ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
        dialog.setLabelProvider(new WorkbenchLabelProvider() {
            @Override
            protected String decorateText(String input, Object element) {
                if (element instanceof IResource)
                    return input + " - " + ((IResource)element).getParent().getFullPath().toString();
                return input;
            }

        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Executable");
        dialog.setMessage("Select the executable file that should be started.\n");
        dialog.setInput(exeFiles);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((IFile)dialog.getResult()[0]);
        }
        return null;
    }

    /**
     * Returns the launch configuration associated with the resource (using 
     * ILaunchConfiguration.getMappedResources()); if there's more than one,
     * lets the user choose from a dialog. Returns null if there's no associated
     * launch config, or the user cancelled.
     */
    public ILaunchConfiguration findOrChooseLaunchConfigAssociatedWith(IResource resource, String mode) throws CoreException {
        ILaunchManager launchManager = DebugPlugin.getDefault().getLaunchManager();
        ILaunchConfigurationType launchType = launchManager.getLaunchConfigurationType(IOmnetppLaunchConstants.SIMULATION_LAUNCH_CONFIGURATION_TYPE);

        ILaunchConfiguration[] launchConfigs = launchManager.getLaunchConfigurations(launchType);
        List<ILaunchConfiguration> matchingConfigs = new ArrayList<ILaunchConfiguration>();
        for (ILaunchConfiguration config : launchConfigs) 
            if (ArrayUtils.contains(config.getMappedResources(), resource))
                matchingConfigs.add(config);

        if (matchingConfigs.size() == 0)
            return null;
        if (matchingConfigs.size() == 1)
            return matchingConfigs.get(0);

        ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                return ((ILaunchConfiguration)element).getName();
            }
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Choose Launch Configuration");
        dialog.setMessage("Select a launch configuration to start.");
        dialog.setInput(matchingConfigs);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((ILaunchConfiguration)dialog.getResult()[0]);
        }

        return null;
    }
}
