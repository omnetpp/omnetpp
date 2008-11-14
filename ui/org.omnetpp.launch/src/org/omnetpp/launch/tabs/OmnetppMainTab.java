package org.omnetpp.launch.tabs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.internal.ui.SWTFactory;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ContainerSelectionDialog;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.ui.ToggleLink;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * A launch configuration tab that displays and edits omnetpp project
 *
 * @author rhornig
 */
public class OmnetppMainTab extends AbstractLaunchConfigurationTab 
implements ModifyListener {

    public final static String VAR_NED_PATH = "opp_ned_path";
    public final static String VAR_SHARED_LIBS = "opp_shared_libs";
    public final static String VAR_LD_LIBRARY_PATH_LOC = "opp_ld_library_path_loc";
    // UI widgets
    protected Button fProgOppRunButton;
    protected Button fProgOtherButton;
    protected Text fProgText;
    protected Button fShowDebugViewButton;

    // working dir
    private Button fWorkspaceButton;
    private Text workingDirText = null;

    // simulation parameters group
    protected String runTooltip;
    // configuration
    protected Text fInifileText;
    protected Combo fConfigCombo;
    protected Text fRunText;
    protected Text fNedPathText;
    protected Spinner fParallelismSpinner;
    protected Button fDefaultEnvButton;
    protected Button fCmdenvButton;
    protected Button fTkenvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherEnvText;
    protected Text fLibraryText;
    protected Text fAdditionalText;

    protected Button fEventLogDefaultButton;
    protected Button fEventLogYesButton;
    protected Button fEventLogNoButton;

    protected Button fDbgOnErrDefaultButton;
    protected Button fDbgOnErrYesButton;
    protected Button fDbgOnErrNoButton;
    
    private ILaunchConfiguration config;
    private boolean updateDialogStateInProgress = false;
    private boolean debugLaunchMode = false;
    private String infoText = null;
    private Button fBrowseForBinaryButton;

    private SelectionAdapter defaultSelectionAdapter = new SelectionAdapter() {
        @Override
        public void widgetSelected(SelectionEvent e) {
            updateDialogState();
        }
    };

    public OmnetppMainTab() {
        super();
    }

    public void createControl(Composite parent) {
        debugLaunchMode = ILaunchManager.DEBUG_MODE.equals(getLaunchConfigurationDialog().getMode());
        runTooltip = debugLaunchMode ? "The run number that should be executed (default: 0)"
                : "The run number(s) that should be executed (eg.: 0,2,7,9..11 or * for all runs) (default: 0)";
		final ScrolledComposite scollComp = new ScrolledComposite( parent, SWT.V_SCROLL | SWT.H_SCROLL );
		scollComp.setExpandHorizontal(true);
		scollComp.setExpandVertical(true);
		

		final Composite comp = SWTFactory.createComposite(scollComp, 1, 1, GridData.FILL_HORIZONTAL);
        createWorkingDirGroup(comp, 1);
        createSimulationGroup(comp, 1);
        createOptionsGroup(comp, 1);

        Composite advancedGroup = createAdvancedGroup(comp, 1);
        ToggleLink more = new ToggleLink(comp, SWT.NONE);
        more.setControls(new Control[] { advancedGroup });
        
        more.addSelectionListener(new SelectionAdapter() {
        	@Override
        	public void widgetSelected(SelectionEvent e) {
        		scollComp.setMinSize(comp.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        	}
        });

        scollComp.setMinSize(comp.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        scollComp.setContent(comp);
        setControl(scollComp);
    }

    public void initializeFrom(ILaunchConfiguration config) {
        this.config = config;
        try {
            // working directory init
            setWorkingDirectoryText(config.getAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, "").trim());
            String exeName = config.getAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, "");
           	fProgOtherButton.setSelection(StringUtils.isNotBlank(exeName));
            fProgOppRunButton.setSelection(StringUtils.isBlank(exeName));
			fProgText.setText(exeName);
            
            fInifileText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_INI_FILES,"" ).trim());
            setConfigName(config.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").trim());
            if (debugLaunchMode)
                fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, ""));
            else
                fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, ""));

            if (fParallelismSpinner != null)
                fParallelismSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1));

            // update UI radio buttons
            String uiArg = config.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "");
            fDefaultEnvButton.setSelection("".equals(uiArg));
            fCmdenvButton.setSelection("Cmdenv".equals(uiArg));
            fTkenvButton.setSelection("Tkenv".equals(uiArg));
            boolean isOther = !fDefaultEnvButton.getSelection() && !fCmdenvButton.getSelection() && !fTkenvButton.getSelection();
            fOtherEnvButton.setSelection(isOther);
            fOtherEnvText.setText(isOther ? uiArg.trim() : "");

            // update eventlog radio buttons  (anything that's not "false" will count as "true")
            String recordEventlogArg = config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "");
            fEventLogDefaultButton.setSelection(recordEventlogArg.equals(""));
            fEventLogNoButton.setSelection(recordEventlogArg.equals("false"));
            fEventLogYesButton.setSelection(!fEventLogDefaultButton.getSelection() && !fEventLogNoButton.getSelection());

            // update debug on error radio buttons  (anything that's not "false" will count as "true")
            String dbgOnErrArg = config.getAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "");
            fDbgOnErrDefaultButton.setSelection(dbgOnErrArg.equals(""));
            fDbgOnErrNoButton.setSelection(dbgOnErrArg.equals("false"));
            fDbgOnErrYesButton.setSelection(!fDbgOnErrDefaultButton.getSelection() && !fDbgOnErrNoButton.getSelection());

            fLibraryText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim());
            fNedPathText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim());
            fAdditionalText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "").trim());
            fShowDebugViewButton.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, false));

            // bring dialog to consistent state
            updateDialogState();

        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
    }

    public void performApply(ILaunchConfigurationWorkingCopy configuration) {
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, getWorkingDirectoryText());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, fProgText.getText());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, fInifileText.getText());        
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, getConfigName());        
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, fLibraryText.getText());        

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, fNedPathText.getText());        

        // if we are in debug mode, we should store the run parameter into the command line too
        String strippedRun = StringUtils.deleteWhitespace(fRunText.getText());
        if (debugLaunchMode) 
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, strippedRun);
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, strippedRun);

        if (fParallelismSpinner != null)
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, fParallelismSpinner.getSelection());

        if (fCmdenvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "Cmdenv");
        else if (fTkenvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "Tkenv");
        else if (fOtherEnvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, fOtherEnvText.getText());
        else configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "");

        if (fEventLogYesButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "true");        
        else if (fEventLogNoButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "false");
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "");

        if (fDbgOnErrYesButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "true");        
        else if (fDbgOnErrNoButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "false");
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "");

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, fAdditionalText.getText());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, fShowDebugViewButton.getSelection());        

        try {
        	Set<IResource> assocRes = new HashSet<IResource>();
        	if (configuration.getMappedResources() != null)
        		assocRes.addAll(Arrays.asList(configuration.getMappedResources()));
        	if (getIniFiles() != null)
        		assocRes.addAll(Arrays.asList(getIniFiles()));
			configuration.setMappedResources((IResource[]) assocRes.toArray(new IResource[assocRes.size()]));
		} catch (CoreException e) {
			LaunchPlugin.logError(e);
		}
        // clear the run info text, so next time it will be re-requested
        infoText = null;
    }

    public void setDefaults(ILaunchConfigurationWorkingCopy configuration) {
    	prepareLaunchConfig(configuration);
    }

    /**
     * Sets the required attributes to create a new simulation launch. Starting values are calculated
     * from the current resource selection.
     */
    public static void prepareLaunchConfig(ILaunchConfigurationWorkingCopy configuration) {
        // check the current selection and figure out the initial values if possible
        String defWorkDir = "";
        String defExe = "";
        String defIni = "omnetpp.ini";
        IResource selectedResource = DebugUITools.getSelectedResource();
        if (selectedResource != null) {
            if (selectedResource instanceof IFile) {
                IFile selFile = (IFile)selectedResource;
                defWorkDir = selFile.getParent().getFullPath().toString();
                // an ini file selected but (not omnetpp.ini) set the ini file name too
                if (StringUtils.equals("ini", selFile.getFileExtension()))
                    defIni = selFile.getName();
                // if executable set the project and program name
                if (OmnetppLaunchUtils.isExecutable(selFile)) 
                    defExe = selFile.getFullPath().toString();
            }
            // if we just selected a directory or project use it as working dir
            if (selectedResource instanceof IContainer) 
                defWorkDir = selectedResource.getFullPath().toString();
        }
        
        if (StringUtils.isEmpty(defExe))
        	defExe = getDefaultExeName(defWorkDir);

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, defWorkDir);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, defExe);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, defIni);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "${"+VAR_NED_PATH+":"+defWorkDir+"}");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "${"+VAR_SHARED_LIBS+":"+defWorkDir+"}");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "");

        // minimal CDT specific attributes required to start without errors
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_ID, "org.eclipse.cdt.debug.mi.core.CDebuggerNew");
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_STOP_AT_MAIN, true);
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_STOP_AT_MAIN_SYMBOL, "main");
    }
    
    protected static String getDefaultExeName(String defWorkDir) {
    	if (StringUtils.isEmpty(defWorkDir))
    		return "";

    	String progs = null;
        try {
            IStringVariableManager variableManager = VariablesPlugin.getDefault().getStringVariableManager();
            progs = variableManager.performStringSubstitution("${opp_simprogs:"+defWorkDir+"}", false);
            // when our CDT plugin is absent, these macros cannot be resolved; just use "" then 
            if (progs.contains("${"))
                progs = "";
        }
        catch (CoreException e) {
            Assert.isTrue(false);  // no exception will be thrown, due to 2nd arg of performStringSubstitution()
        }
        
        // return the first program from the list
        String [] splitProgs  = StringUtils.split(progs, ' ');
        if (splitProgs.length > 0) 
        	return splitProgs[0];
    	
		return "";
	}

	/**
     * Fills the config combo with the config section values from the inifiles
     */
    protected void updateDialogState() {
        if (updateDialogStateInProgress)
            return;
        updateDialogStateInProgress = true;

        // executable handling
        fProgText.setEnabled(fProgOtherButton.getSelection());
        fBrowseForBinaryButton.setEnabled(fProgOtherButton.getSelection());
        if (fProgOppRunButton.getSelection())
        	fProgText.setText("");
        
        // update the config combo
        IFile[] inifiles = getIniFiles();
        if (config == null || inifiles == null)
            fConfigCombo.setItems(new String[] {});
        else {
            String currentSelection = getConfigName();
            String newConfigNames[] = getConfigNames(inifiles);
            if (!ObjectUtils.equals(StringUtils.join(fConfigCombo.getItems()," - "),
                    StringUtils.join(newConfigNames," - "))) {
                fConfigCombo.setItems(newConfigNames);
                setConfigName(currentSelection);
            }
        }
        // update the UI (env) state
        if (!fOtherEnvButton.getSelection())
            fOtherEnvText.setText("");
        fOtherEnvText.setEnabled(fOtherEnvButton.getSelection());

        if (fParallelismSpinner != null) {
            fParallelismSpinner.setEnabled(fCmdenvButton.getSelection());
            if (!fCmdenvButton.getSelection())
                fParallelismSpinner.setSelection(1);
        }

        // update the state of apply and other system buttons
        updateLaunchConfigurationDialog();
        updateDialogStateInProgress = false;
    }

    protected String getWorkingDirectoryText() {
        return workingDirText.getText().trim();
    }

    protected void setWorkingDirectoryText(String dir) {
        if (dir != null) {
            workingDirText.setText(dir);
        }
    }

    /**
     * Expands and returns the working directory attribute of the given launch
     * configuration or NULL.
     */
    private IContainer getWorkingDirectory(){
        try {
            String expandedPath = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(getWorkingDirectoryText());
            if (expandedPath.length() > 0) {
                IPath newPath = new Path(expandedPath);
                if (newPath.segmentCount() == 0)
                	return null;
                IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(newPath);
                if (resource instanceof IContainer)
                	return (IContainer)resource;
                // not a container - we cannot convert
                return null;
            }
        } catch (CoreException e) {
            LaunchPlugin.logError("Error getting working directory from the Launch dialog", e);
        }
        return null;

    }

    /**
     * Returns the selected ini files or (omnetpp.ini) if the inifile text line
     * is empty. Returns null on error. Ignores names that start with a macro (${})
     */
    private IFile[] getIniFiles() {
        String names[] =  StringUtils.split(fInifileText.getText());
        return convertToFiles(names);
    }

    /**
     * convert to IFiles. Names are relative to the working dir.
     * names are workspace path relative. Skips any macros.
     */
    private IFile[] convertToFiles(String[] names) {
        List<IFile> result = new ArrayList<IFile>();
        IContainer workingDirectory = getWorkingDirectory();
        if (workingDirectory == null)
            return null;
        for (String name : names) {
            if (!name.startsWith("${")) {
                IPath namePath = new Path(name);
                if (!namePath.isAbsolute()) 
                    namePath = workingDirectory.getFullPath().append(namePath);

                result.add(ResourcesPlugin.getWorkspace().getRoot().getFile(namePath));
            }
        }
        return result.toArray(new IFile[result.size()]);
    }

    /**
     * Returns all the configuration names found in the supplied inifiles
     */
    private String[] getConfigNames(IFile[] inifiles) {
        Map<String,OmnetppLaunchUtils.ConfigEnumeratorCallback.Section> sections
        = new LinkedHashMap<String, OmnetppLaunchUtils.ConfigEnumeratorCallback.Section>();
        if (inifiles != null)
            for (IFile inifile : inifiles) {
                InifileParser iparser = new InifileParser();
                try {
                    iparser.parse(inifile, new OmnetppLaunchUtils.ConfigEnumeratorCallback(inifile, sections));
                } catch (ParseException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                } catch (CoreException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                } catch (IOException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                }
            }
        List<String> result = new ArrayList<String>();
        result.add("");
        for (OmnetppLaunchUtils.ConfigEnumeratorCallback.Section sec : sections.values())
            result.add(sec.toString());
        return result.toArray(new String[] {});
    }

    /**
     * Returns the currently selected config name (after stripping the comments an other stuff)
     */
    private String getConfigName() {
        return StringUtils.substringBefore(fConfigCombo.getText(), "--").trim();
    }

    /**
     * @param name The config name that should be selected from the drop down. If no match found
     * the first empty line will be selected
     */
    private void setConfigName(String name) {
        fConfigCombo.setText("");
        for (String line : fConfigCombo.getItems())
            if (line.startsWith(name)) {
                fConfigCombo.setText(line);
                return;
            }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.eclipse.debug.ui.ILaunchConfigurationTab#isValid(org.eclipse.debug.core.ILaunchConfiguration)
     */
    @Override
    public boolean isValid(ILaunchConfiguration configuration) {
        setErrorMessage(null);

        // working directory
        // if variables are present, we cannot resolve the directory
        String workingDirPath = getWorkingDirectoryText();
        if (workingDirPath.length() == 0) {
            setErrorMessage("Working directory must be set");
            return false;
        }

        IContainer workingDirectory = getWorkingDirectory();
        if (workingDirectory == null) {
            setErrorMessage("Not a directory");
            return false;
        }

        if (!ProjectUtils.hasOmnetppNature(workingDirectory.getProject())) {
            setErrorMessage("The selected project must be an OMNeT++ simulation");
            return false;
        }

        String name = fProgText.getText().trim();
        if (fProgOtherButton.getSelection() && StringUtils.isBlank(name)) {
                setErrorMessage("Simulation program must be set");
                return false;
        }
        // if there is name specified
        if (StringUtils.isNotEmpty(name)) {
            IPath exePath = new Path(name);
            // on windows add the exe extension if not present
            if (Platform.getOS().equals(Platform.OS_WIN32) && !"exe".equalsIgnoreCase(exePath.getFileExtension())) 
                exePath = exePath.addFileExtension("exe");
            IFile exefile = exePath.segmentCount() >1 ? ResourcesPlugin.getWorkspace().getRoot().getFile(exePath) : null;

            if (exefile == null || !exefile.isAccessible()) {
                setErrorMessage("Simulation program does not exist or not accessible in workspace");
                return false;
            }
        }

        // simulation block parameters
        IFile ifiles[] = getIniFiles();
        if (ifiles == null) {
            setErrorMessage("Ini file does not exist, or not accessible in workspace");
            return false;
        }
        if (ifiles.length == 0) {
            setErrorMessage("Ini file must be set");
            return false;
        }
        for (IFile ifile : ifiles) {
            if (!ifile.isAccessible()) {
                setErrorMessage("Ini file "+ifile.getName()+" does not exist, or not accessible in workspace");
                return false;
            }
        }

        boolean isMultipleRuns;
        try {
            isMultipleRuns = OmnetppLaunchUtils.containsMultipleRuns(StringUtils.deleteWhitespace(fRunText.getText()));
        } catch (CoreException e) {
            setErrorMessage(e.getMessage());
            return false;
        }

        if (fOtherEnvButton.getSelection() && StringUtils.isEmpty(fOtherEnvText.getText())) {
            setErrorMessage("Environment type must be specified");
            return false;
        }
        if (!fCmdenvButton.getSelection() && !fOtherEnvButton.getSelection() && isMultipleRuns) {
            setErrorMessage("Multiple runs are only supported for the Command line environment");
            return false;
        }

        return super.isValid(configuration);
    }

    @Override
    public Image getImage() {
        return LaunchPlugin.getImage("/icons/full/ctool16/omnetsim.gif");
    }

    public String getName() {
        return "Main";
    }


    // ********************************************************************
    // event handlers

    /**
     * Show a dialog that lets the user select a project. This in turn provides context for the main
     * type, allowing the user to key a main type name, or constraining the search for main types to
     * the specified project.
     */
    protected void handleBinaryBrowseButtonSelected() {
        ElementTreeSelectionDialog dialog
        = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                new OmnetppLaunchUtils.ExecutableWorkbenchContentProvider());
        dialog.setAllowMultiple(false);
        dialog.setTitle("Select Executable File");
        dialog.setMessage("Select the executable file that should be started.\n");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID && dialog.getFirstResult() instanceof IFile) {
            String exefile = ((IFile)dialog.getFirstResult()).getFullPath().toString();
            fProgText.setText(StringUtils.removeEnd(exefile, ".exe"));
        }
    }

    protected void handleBrowseLibrariesButtonSelected() {
        String extensionRegexp = ".*\\.";
        String extension = "";
        String prefix = "";
        if (Platform.getOS().equals(Platform.OS_WIN32 )) {
            extension = "dll";
        } 
        else if (Platform.getOS().equals(Platform.OS_MACOSX)) {
            extension = "dylib";
            prefix= "lib";
        }
        else {
            extension = "so";
            prefix = "lib";
        }
        extensionRegexp += extension;
        
        ElementTreeSelectionDialog dialog
        = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                new OmnetppLaunchUtils.FilteredWorkbenchContentProvider(extensionRegexp));
        dialog.setTitle("Select Shared Libraries");
        dialog.setMessage("Select the library file(s) you want to load at the beginning of the simulation.\n" +
        "Multiple files can be selected.");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            String libfiles = "";
            IContainer workingDirectory = getWorkingDirectory();
            for (Object resource : dialog.getResult()) {
                if (resource instanceof IFile && workingDirectory != null) {
                    libfiles += removePrefixSuffixFromName(
                    		      OmnetppLaunchUtils.makeRelativePathTo(((IFile)resource).getFullPath(), workingDirectory.getFullPath())
                    		      ,prefix, "."+extension)+" ";
                }
            }
            fLibraryText.setText(libfiles.trim());
        }
    }
    
    /**
     * Removes the given suffix and prefix from the NAME part of the path.
     */
    private String removePrefixSuffixFromName(IPath path, String namePrefix, String nameSuffix) {
    	String lastSeg = path.lastSegment();
    	lastSeg = StringUtils.removeStart(lastSeg, namePrefix);
    	lastSeg = StringUtils.removeEnd(lastSeg, nameSuffix);
    	return path.removeLastSegments(1).append(lastSeg).toString();
    }

    protected void handleBrowseInifileButtonSelected() {
        ElementTreeSelectionDialog dialog
        = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                new OmnetppLaunchUtils.FilteredWorkbenchContentProvider(".*\\.ini"));
        dialog.setTitle("Select Ini Files");
        dialog.setMessage("Select the initialization file(s) for the simulation.\n" +
        "Multiple files can be selected.");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            String inifiles = "";
            IContainer workingDirectory = getWorkingDirectory();
            for (Object resource : dialog.getResult()) {
                if (resource instanceof IFile && workingDirectory != null)
                    inifiles += OmnetppLaunchUtils.makeRelativePathTo(((IFile)resource).getFullPath(),
                            workingDirectory.getFullPath()).toString()+" ";
            }
            fInifileText.setText(inifiles.trim());
            updateDialogState();
        }
    }

    // working directory group handlers
    /**
     * Show a dialog that lets the user select a working directory from
     * the workspace
     */
    private void handleWorkingDirBrowseButtonSelected() {
        IContainer currentContainer= getWorkingDirectory();
        if (currentContainer == null)
            currentContainer = ResourcesPlugin.getWorkspace().getRoot();
        ContainerSelectionDialog dialog = new ContainerSelectionDialog(getShell(), currentContainer, false, "Select a workspace relative working directory:");
        dialog.showClosedProjects(false);
        dialog.open();
        Object[] results = dialog.getResult();
        if (results != null && results.length > 0 && results[0] instanceof IPath) {
            IPath path = (IPath)results[0];
            String containerName = path.toString();
            setWorkingDirectoryText(containerName);
        }
    }

    // ********************************************************************
    // event listeners

    public void modifyText(ModifyEvent e) {
        updateDialogState();
    }

    protected void updateMacros() {
        String workingDir = getWorkingDirectoryText();
        String libText = fLibraryText.getText();
        libText = libText.replaceAll("\\$\\{"+VAR_SHARED_LIBS+":.*?\\}", "\\${"+VAR_SHARED_LIBS+":"+workingDir+"}");
        fLibraryText.setText(libText);

        String nedText = fNedPathText.getText();
        nedText = nedText.replaceAll("\\$\\{"+VAR_NED_PATH+":.*?\\}", "\\${"+VAR_NED_PATH+":"+workingDir+"}");
        fNedPathText.setText(nedText);
    }
    
    // ********************************************************************
    // dialog UI control creation and layout
    public Group createWorkingDirGroup(Composite parent, int colSpan) {
        Group group = SWTFactory.createGroup(parent, "Working directory", 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout innerLd = (GridLayout)group.getLayout();
        innerLd.marginWidth = 0;

        setControl(group);
        workingDirText = SWTFactory.createSingleText(group, 1);
        workingDirText.addModifyListener(this);
        workingDirText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateMacros();
            }
        });
        fWorkspaceButton = createPushButton(group, "Browse...", null);
        fWorkspaceButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                handleWorkingDirBrowseButtonSelected();
            }
        });
        return group;
    }


    protected Composite createSimulationGroup(Composite parent, int colSpan) {
    	Composite composite = SWTFactory.createGroup(parent, "Simulation", 4, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)composite.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(composite, "Executable:",1);

        Composite innerComposite = SWTFactory.createComposite(composite, 3, 2, GridData.FILL_HORIZONTAL);
        GridLayout innerLd = (GridLayout)innerComposite.getLayout();
        innerLd.marginHeight = 0;
        innerLd.marginWidth = 0;
        
        fProgOppRunButton = createRadioButton(innerComposite, "opp_run");
        fProgOppRunButton.setSelection(true);
        fProgOtherButton = createRadioButton(innerComposite, "Other:");
        fProgText = SWTFactory.createSingleText(innerComposite, 1);
        fProgText.addModifyListener(this);

        fBrowseForBinaryButton = SWTFactory.createPushButton(composite, "Browse...", null);
        fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBinaryBrowseButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Ini file(s):", 1);

        fInifileText = SWTFactory.createSingleText(composite, 2);
        fInifileText.setToolTipText("The INI file(s) defining parameters and configuration blocks (relative to the working directory)");
        fInifileText.addModifyListener(this);

        Button browseInifileButton = SWTFactory.createPushButton(composite, "Browse...", null);
        browseInifileButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseInifileButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Config name:",1);

        fConfigCombo = SWTFactory.createCombo(composite, SWT.BORDER | SWT.READ_ONLY, 3, new String[] {});
        fConfigCombo.setToolTipText("The configuration from the INI file that should be executed");
        fConfigCombo.setVisibleItemCount(10);
        fConfigCombo.addModifyListener(this);

        SWTFactory.createLabel(composite, "Run number:",1);

        int runSpan = debugLaunchMode ? 3 : 1;
        fRunText = SWTFactory.createSingleText(composite, runSpan);
        fRunText.addModifyListener(this);
        HoverSupport hover = new HoverSupport();
        hover.adapt(fRunText, new IHoverTextProvider() {
            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outPreferredSize) {
                if (infoText == null)
                    infoText = OmnetppLaunchUtils.getSimulationRunInfo(config);
                outPreferredSize.preferredWidth = 350;
                return HoverSupport.addHTMLStyleSheet(runTooltip+"<pre>"+infoText+"</pre>");
            }
        });

        // parallel execution is not possible under CDT
        if (!debugLaunchMode) {
            SWTFactory.createLabel(composite, "Processes to run in parallel:", 1);
            fParallelismSpinner = new Spinner(composite, SWT.BORDER);
            fParallelismSpinner.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
            fParallelismSpinner.setMinimum(1);
            fParallelismSpinner.addModifyListener(this);
        }

        return composite;
    }

    protected Composite createOptionsGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Options", 2, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)composite.getLayout();
        ld.marginHeight = 1;

        createUIRadioButtons(composite, 2);
        createRecordEventlogRadioButtons(composite, 2);
        createDbgOnErrRadioButtons(composite, 2);
        return composite;
    }

    protected Composite createAdvancedGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Advanced", 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)composite.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(composite, "Dynamic libraries:", 1);

        fLibraryText = SWTFactory.createSingleText(composite, 1);
        fLibraryText.setToolTipText("DLLs or shared libraries to load (without extension, relative to the working directory. Use ${opp_shared_libs:/workingdir} for automatic setting.)");
        fLibraryText.addModifyListener(this);

        Button browseLibrariesButton = SWTFactory.createPushButton(composite, "Browse...", null);
        browseLibrariesButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseLibrariesButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "NED Source Path:", 1);
        fNedPathText = SWTFactory.createSingleText(composite, 2);
        fNedPathText.setToolTipText("Directories where NED files are read from (relative to the first selected INI file). " +
        "Use ${opp_ned_path:/workingdir} for automatic setting.");
        fNedPathText.addModifyListener(this);

        SWTFactory.createLabel(composite, "Additional arguments:", 1);
        fAdditionalText = SWTFactory.createSingleText(composite, 2);
        fAdditionalText.setToolTipText("Specify additional command line arguments");
        fAdditionalText.addModifyListener(this);

        fShowDebugViewButton = SWTFactory.createCheckButton(composite, "Show Debug View on Launch", null, false, 3);
        fShowDebugViewButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                updateLaunchConfigurationDialog();
            }
        });

        return composite;
    }

    protected void createUIRadioButtons(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "User interface:", 1);

        fDefaultEnvButton = createRadioButton(comp, "Default");
        fDefaultEnvButton.setSelection(true);
        fCmdenvButton = createRadioButton(comp, "Command line");
        fTkenvButton = createRadioButton(comp, "Tcl/Tk");
        fOtherEnvButton = createRadioButton(comp, "Other:");

        fOtherEnvText = SWTFactory.createSingleText(comp, 1);
        fOtherEnvText.setToolTipText("Specify the custom environment name");
        fOtherEnvText.addModifyListener(this);
    }

    protected void createRecordEventlogRadioButtons(Composite parent, int colSpan) {

        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "Record Eventlog:", 1);

        fEventLogDefaultButton = createRadioButton(comp, "Default");
        fEventLogDefaultButton.setSelection(true);
        fEventLogYesButton = createRadioButton(comp, "Yes");
        fEventLogNoButton = createRadioButton(comp, "No");
    }

    protected void createDbgOnErrRadioButtons(Composite parent, int colSpan) {

        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "Debug on error:", 1);

        fDbgOnErrDefaultButton = createRadioButton(comp, "Default");
        fDbgOnErrDefaultButton.setSelection(true);
        fDbgOnErrYesButton = createRadioButton(comp, "Yes");
        fDbgOnErrNoButton = createRadioButton(comp, "No");
    }
    
    
    protected Button createRadioButton(Composite comp, String label) {
        Button button = SWTFactory.createRadioButton(comp, label);
        button.setLayoutData(new GridData());
        button.addSelectionListener(defaultSelectionAdapter);
    	return button;
    }

}
