package org.omnetpp.launch;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
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
import org.omnetpp.common.util.FileUtils;
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
    public static final String PREF_DONTSHOW_LAUNCHCONFIGCREATED_MESSAGE = "org.omnetpp.launch.DoNotShowLaunchConfigCreatedMsg";

    // dummy IFile, used to represent "opp_run" as a return value  
    protected static final IFile IFILE_OPP_RUN = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path("-/-"));

    protected static class IniSection {
        IFile iniFile;
        String configName;
        String extendsName;
        String description;
        String network;
        public IniSection(IFile f, String c) {iniFile = f; configName = c;}
        public String toString() {return iniFile+"/"+configName;}
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
                // determine executable, ini file and ini config to use for launching
                String launchName = null;
                IFile exeFile = null;
                IFile iniFile = null;
                String configName = null;
    
                if (resource instanceof IContainer) {
                    IContainer folder = (IContainer)resource;
                    
                    // collect all ini files in this folder
                    List<IniSection> candidates = collectInifileConfigsForFolder(folder);
                    if (candidates.isEmpty()) {
                        MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: '" + folder.getFullPath().toString() + "' does not contain an ini file.");
                        return;
                    }

                    // choose executable to launch (note: we don't do this if there's no inifile!)
                    exeFile = chooseExecutable(resource.getProject());
                    if (exeFile == null)
                        return; // user cancelled
                    if (exeFile == IFILE_OPP_RUN)
                        exeFile = null;

                    // choose or create one
                    IniSection iniFileAndConfig;
                    if (candidates.size() == 1)
                        iniFileAndConfig = candidates.get(0);
                    else 
                        iniFileAndConfig = chooseInifileConfigFromDialog(candidates, folder);
    
                    if (iniFileAndConfig == null)
                        return; // user cancelled

                    iniFile = iniFileAndConfig.iniFile;
                    configName = iniFileAndConfig.configName;
                    launchName = folder.getName();
                }
                else if (resource instanceof IFile && "ini".equals(resource.getFileExtension())) {
                    // choose executable to launch
                    exeFile = chooseExecutable(resource.getProject());
                    if (exeFile == null)
                        return; // user cancelled
                    if (exeFile == IFILE_OPP_RUN)
                        exeFile = null;
    
                    // use selected ini file
                    iniFile = (IFile)resource;
                    launchName = resource.getParent().getName();
                }
                else if (resource instanceof IFile && "ned".equals(resource.getFileExtension())) {
                    // must be a valid NED file and must contain at least one network
                    IFile nedFile = (IFile)resource;
                    NEDResources res = NEDResourcesPlugin.getNEDResources();
                    if (!res.isNedFile(nedFile)) {
                        MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: '" + nedFile.getName() + "' is not a NED file, or it is not in a NED source folder.");
                        return;
                    }
                    List<INEDTypeInfo> networks = getNetworksInNedFile(nedFile);
                    if (networks.isEmpty()) {
                        MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: '" + nedFile.getName() + "' does not contain a network.");
                        return;
                    }
    
                    // choose executable to launch (note: we don't do this if NED file is not OK!)
                    exeFile = chooseExecutable(resource.getProject());
                    if (exeFile == null)
                        return; // user cancelled
                    if (exeFile == IFILE_OPP_RUN)
                        exeFile = null;
    
                    // choose network from NED file
                    INEDTypeInfo network = networks.size()==1 ? networks.get(0) : chooseNetwork(networks);
                    if (network == null)
                        return; // cancelled
                    
                    // collect ini files that instantiate this network
                    List<IniSection> candidates = collectInifileConfigsForNetwork(network);
    
                    // choose or create one
                    IniSection iniFileAndConfig;
                    if (candidates.size() == 1)
                        iniFileAndConfig = candidates.get(0);
                    else if (candidates.size() > 1)
                        iniFileAndConfig = chooseInifileConfigFromDialog(candidates, nedFile.getParent());
                    else /*if empty*/
                        iniFileAndConfig = askAndCreateInifile(network);
    
                    if (iniFileAndConfig == null)
                        return; // user cancelled
    
                    iniFile = iniFileAndConfig.iniFile;
                    configName = iniFileAndConfig.configName;
                    launchName = (configName!=null && !configName.equals(GENERAL)) ? configName : 
                        iniFileAndConfig.network!=null ? iniFileAndConfig.network : 
                            nedFile.getParent().getName();
                } 
                else {
                    return; // resource not supported
                }
    
                // create launch config based on the above data
                lc = createLaunchConfig(launchName, exeFile, iniFile, configName, resource);
    
                // tell novice users what happened
                IPreferenceStore preferences = LaunchPlugin.getDefault().getPreferenceStore();
                String pref = preferences.getString(PREF_DONTSHOW_LAUNCHCONFIGCREATED_MESSAGE);
                if (!MessageDialogWithToggle.ALWAYS.equals(pref)) {
                    int result = MessageDialogWithToggle.openOkCancelConfirm(
                            Display.getCurrent().getActiveShell(), 
                            "Launch Configuration Created", 
                            "A launch configuration named '" + lc.getName() + "' has been created, and associated " +
                            "with resource '" + resource.getName() + "'. You can modify or delete this launch configuration " +
                            "in the Run|Run Configurations... or Run|Debug Configurations... dialogs.", 
                            "Do not show this message again", false, 
                            preferences, PREF_DONTSHOW_LAUNCHCONFIGCREATED_MESSAGE).getReturnCode();
                    if (result != IDialogConstants.OK_ID) { // note: Esc returns -1, for which there is no ID constant
                        lc.delete();
                        return;
                    }
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
     * Returns the launch configuration associated with the resource (using 
     * ILaunchConfiguration.getMappedResources()); if there's more than one,
     * lets the user choose from a dialog. Returns null if there's no associated
     * launch config, or the user cancelled.
     */
    protected ILaunchConfiguration findOrChooseLaunchConfigAssociatedWith(IResource resource, String mode) throws CoreException {
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
    
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0)
            return ((ILaunchConfiguration)dialog.getResult()[0]);
        else 
            return null;
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

    protected INEDTypeInfo chooseNetwork(List<INEDTypeInfo> networks) {
        // choose network from dialog
        ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
        dialog.setLabelProvider(new NedModelLabelProvider());
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Network");
        dialog.setMessage("Select a network:");
        dialog.setInput(networks);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0)
            return ((INEDTypeInfo)dialog.getResult()[0]);
        return null; // user cancelled
    }
    
    protected List<IniSection> collectInifileConfigsForFolder(IContainer folder) {
        List<IFile> iniFiles = collectInifilesFrom(folder);
        List<IniSection> result = new ArrayList<IniSection>();
        for (IFile iniFile : iniFiles)
            result.add(new IniSection(iniFile, null));
        return result;
    }

    /**
     * Finds and returns all ini files that run the given network.
     */
    protected List<IniSection> collectInifileConfigsForNetwork(INEDTypeInfo network) {
        IFile nedFile = network.getNEDFile();

        // first, collect all inifiles from *this* project
        // (note: an inifile that refers to this network cannot be in a referenced 
        // project, as the network's NED type is not visible there!)
        final List<IFile> iniFiles = collectInifilesFrom(nedFile.getProject());

        // convert network names to String[], for faster lookup 
        final String networkName = network.getName();
        final String networkQName = network.getFullyQualifiedName();
        String interestingInifileRegex = "(?s).*\\b(include|" +networkName + ")\\b.*";

        // now, find those inifiles that refer to this network
        List<IniSection> result = new ArrayList<IniSection>();
        for (IFile iniFile : iniFiles) {
            try {
                String iniFileText = FileUtils.readTextFile(iniFile.getContents());
                if (iniFileText.matches(interestingInifileRegex)) {
                    // inifile looks interesting, so parse it
                    boolean isSameDir = iniFile.getParent().equals(nedFile.getParent());
                    Map<String,IniSection> sections = parseInifile(iniFileText, iniFile);

                    // first, collect sections where network matches
                    Set<IniSection> interestingSections = new HashSet<IniSection>(); 
                    for (IniSection section : sections.values()) {
                        if (section.network != null) {
                            if (isSameDir && networkName.equals(section.network))
                                interestingSections.add(section);
                            else if (networkQName.equals(section.network))
                                interestingSections.add(section);
                        }
                    }
                    
                    // transitive closure: add sections that extend the already 
                    // added ones, without overwriting the "network=" setting
                    boolean again = true;
                    while (again) {
                        again = false;
                        for (IniSection s : sections.values()) {
                            if (!interestingSections.contains(s) && s.network == null) {
                                IniSection baseSection = sections.get(StringUtils.defaultString(s.extendsName,GENERAL));
                                if (interestingSections.contains(baseSection)) {
                                    s.network = baseSection.network; // propagate up network name
                                    interestingSections.add(s);
                                    again = true;
                                }
                            }
                        }
                    }
                    result.addAll(interestingSections);
                }
            }
            catch (ParseException e) { }
            catch (CoreException e) { }
            catch (IOException e) { }

        }
        return result;
    }

    /**
     * Collects ALL inifiles under the given container.
     */
    protected List<IFile> collectInifilesFrom(IContainer folder) {
        final List<IFile> iniFiles = new ArrayList<IFile>();
        try {
            folder.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (resource instanceof IFile && "ini".equals(resource.getFileExtension()))
                        iniFiles.add((IFile)resource);
                    return true;
                }
            });
        } catch (CoreException e) {
            LaunchPlugin.logError(e);
        }
        return iniFiles;
    }

    /**
     * Extracts section names, and their extends=, network= and description= 
     * entries into a map. The "Config " prefix is stripped from section names.
     */
    //TODO should resolve includes, and process them too
    protected Map<String,IniSection> parseInifile(String inifileText, final IFile iniFile) throws ParseException, IOException {
        final Map<String, IniSection> sections = new HashMap<String, IniSection>();
        InifileParser inifileParser = new InifileParser();
        inifileParser.parse(inifileText, new InifileParser.ParserAdapter() {
            IniSection currentSection = null;
            public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
                currentSection = sections.get(sectionName);
                if (currentSection == null) {
                    currentSection = new IniSection(iniFile, StringUtils.removeStart(sectionName, CONFIG_));
                    sections.put(currentSection.configName, currentSection);
                }
            }
            public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {
                if (currentSection!=null) {
                    if (key.equals(CFGID_NETWORK.getKey()))
                        currentSection.network = value;
                    else if (key.equals(EXTENDS))
                        currentSection.extendsName = value;
                    else if (key.equals(CFGID_DESCRIPTION.getKey()))
                        currentSection.description = value;
                }                
            }
        });
        return sections;
    }

    /**
     * Asks the user if s/he wants to create an ini file for the given network, 
     * then creates the inifile. Returns null if user cancelled.
     */
    protected IniSection askAndCreateInifile(INEDTypeInfo network) throws CoreException {
        String text = "Network '" + network.getName() + "' does not have an associated ini file. Do you want to create one now?";
        if (!MessageDialog.openConfirm(Display.getCurrent().getActiveShell(), "Create Ini File", text)) 
            return null; // user cancelled

        // generate new ini file with a good name for the selected network
        IContainer container = network.getNEDFile().getParent();
        String inifileName = generateGoodInifileName(container, network);
        IFile iniFile = container.getFile(new Path(inifileName));
        return createInifile(iniFile, network);
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
    protected IniSection createInifile(IFile iniFile, INEDTypeInfo network) throws CoreException {
        String content = 
            "[General]\n" +
            "network = " + network.getName() + "\n" +
            "**.apply-default = true\n";
        iniFile.create(new ByteArrayInputStream(content.getBytes()), false, new NullProgressMonitor());

        IniSection section = new IniSection(iniFile, GENERAL);
        section.network = network.getName();
        return section;
    }

    /**
     * Offer the user a selection dialog to choose an inifile+configName from a list.
     * Inifile names that are in the given default folder will be shown without path (just the name).
     * Returns null if user cancelled. 
     */
    protected IniSection chooseInifileConfigFromDialog(List<IniSection> candidates, final IContainer defaultDir) {
        ListDialog dialog = new ListDialog(Display.getCurrent().getActiveShell());
        final WorkbenchLabelProvider workbenchLabelProvider = new WorkbenchLabelProvider(); // cannot subclass, due to final methods
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                IniSection cfg = (IniSection)element;
                String friendlyFileName = defaultDir.equals(cfg.iniFile.getParent()) ? cfg.iniFile.getName() : cfg.iniFile.getFullPath().toString();
                if (cfg.configName == null) {
                    return friendlyFileName;
                }
                else {
                    String sectionName = cfg.configName.equals(GENERAL) ? cfg.configName : CONFIG_ + cfg.configName;
                    return friendlyFileName + " - [" + sectionName + "]"+ (cfg.description==null ? "" : " - " + cfg.description);
                }
            }
            @Override
            public Image getImage(Object element) {
                IniSection cfg = (IniSection)element;
                return workbenchLabelProvider.getImage(cfg.iniFile);
            }
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Ini file and Config");
        dialog.setMessage("Select ini file and config:\n");
        dialog.setInput(candidates);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((IniSection)dialog.getResult()[0]);
        }
        return null;
    }

    /**
     * Creates and saves a launch configuration with the given attributes.
     * @param exeFile  null means "opp_run" 
     */
    protected ILaunchConfiguration createLaunchConfig(String suggestedName, IFile exeFile, IFile iniFile, String configName, IResource resourceToAssociateWith) throws CoreException {
        Assert.isTrue(iniFile != null);
        ILaunchManager launchManager = DebugPlugin.getDefault().getLaunchManager();
        ILaunchConfigurationType launchType = launchManager.getLaunchConfigurationType(IOmnetppLaunchConstants.SIMULATION_LAUNCH_CONFIGURATION_TYPE);
        if (suggestedName == null)
            suggestedName = resourceToAssociateWith.getParent().getName();
        String name = launchManager.generateUniqueLaunchConfigurationNameFrom(suggestedName);
        ILaunchConfigurationWorkingCopy wc = launchType.newInstance(null, name);

        OmnetppMainTab.prepareLaunchConfig(wc);

        wc.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, exeFile==null ? "" : exeFile.getFullPath().toString());
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
        // get executables and shared libs from the CDT settings
        // (we do it via macros, in order to avoid listing the CDT plugin as dependency in the manifest)
        String simProgs = null;
        String sharedLibs = null;
        try {
            IStringVariableManager variableManager = VariablesPlugin.getDefault().getStringVariableManager();
            simProgs = variableManager.performStringSubstitution("${opp_simprogs:"+project.getFullPath().toString()+"}", false);
            sharedLibs = variableManager.performStringSubstitution("${opp_shared_libs:"+project.getFullPath().toString()+"}", false);
            
            // when our CDT plugin is absent, these macros cannot be resolved; just use "" then 
            if (simProgs.contains("${"))
                simProgs = "";
            if (sharedLibs.contains("${"))
                sharedLibs = "";
        }
        catch (CoreException e) {
            Assert.isTrue(false);  // no exception will be thrown, due to 2nd arg of performStringSubstitution()
        }
        
        int numSimProgs = StringUtils.split(simProgs).length;
        int numSharedLibs = StringUtils.split(sharedLibs).length;
        if (numSimProgs == 1) {
            // convert to IFile and return it
            return pathToIFile(simProgs.trim());
        }
        else if (numSimProgs > 1) {
            // choose from them
            List<IFile> files = new ArrayList<IFile>();
            for (String path : simProgs.split(" "))
                files.add(pathToIFile(path));
            return chooseFromExeFiles(files);
        }
        else if (numSimProgs == 0 && numSharedLibs > 0) {
            // simulation is apparently shared lib based, return "opp_run"
            return IFILE_OPP_RUN;            
        }
        else {
            // numSimProgs == 0 && numSharedLibs == 0: nothing to run.
            // offer all executables found in the projects
            // FIXME we should actually offer shared libs as well, and add selected ones to the launch config!
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

            if (exeFiles.size() == 1)
                return exeFiles.get(0);
            else if (exeFiles.size() > 1)
                return chooseFromExeFiles(exeFiles);
            else {
                MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Cannot launch simulation: " +
                        "no executable found in project " + project.getName() + ". To specify a shared library to be " +
                        "run with opp_run, open the Run|Run Configurations... dialog."
                );
                return null;
            }
        }
    }

    protected IFile pathToIFile(String path) {
        return (IFile) ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
    }

    protected IFile chooseFromExeFiles(final List<IFile> exeFiles) {
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

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0)
            return ((IFile)dialog.getResult()[0]);
        else
            return null;
    }
}
