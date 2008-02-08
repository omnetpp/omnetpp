package org.omnetpp.launch.tabs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.internal.ui.SWTFactory;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

// TODO check what user interface is present in the exe (-q userinterfaces) and disable which is not
/**
 * A launch configuration tab that displays and edits omnetpp project
 *
 * @author rhornig
 */
public class SimulationTab extends AbstractLaunchConfigurationTab 
    implements SelectionListener, ModifyListener {
    protected final String DEFAULT_RUNTOOLTIP= "The run number(s) that should be executed (eg.: 0,2,7,9..11 or * for ALL runs) (default: 0)";
	// UI widgets
	protected Text fInifileText;
	protected Combo fConfigCombo;
    protected Text fRunText;
    protected Text fNedPathText;
    protected Spinner fParallelismSpinner;
    protected Button fDefaultEnvButton;
    protected Button fCmdEnvButton;
    protected Button fTkEnvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherEnvText;
    protected Text fLibraryText;
    protected Text fAdditionalText;
    private boolean cdtContributed = true;
    private String infoText = null;
	private Composite notOmnetppProjectMessageComposite;
	private Composite simComposite;
    private ILaunchConfiguration config;
    private EmbeddableBlock changeListener;


    /**
     * called if we are contributing to the CDT launch type
     */
    public SimulationTab() {
        super();
    }

    /**
     * Called when 
     * @param embeddingTab
     * @param cdtContributed Whether we are contributing to CDT (true) or using the stand-alone launcher (false)
     *        default is true (CDT contribution)
     */
    SimulationTab(EmbeddableBlock changeListener, boolean cdtContributed) {
        this.changeListener = changeListener;
        this.cdtContributed = cdtContributed;
    }

    public void createControl(Composite parent) {
        Composite mainComposite = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
    	GridLayout mainGridLayout = new GridLayout();
    	mainGridLayout.marginHeight = mainGridLayout.marginWidth = 0;
		mainComposite.setLayout(mainGridLayout);

		notOmnetppProjectMessageComposite = SWTFactory.createComposite(mainComposite, 1, 1, GridData.FILL_HORIZONTAL);
		SWTFactory.createLabel(notOmnetppProjectMessageComposite, "This project is not an OMNEST/OMNeT++ project.", 1);
		simComposite = SWTFactory.createComposite(mainComposite, 1, 1, GridData.FILL_HORIZONTAL);
		createLibraryGroup(simComposite, 1);
		createIniGroup(simComposite, 1);
        createConfigGroup(simComposite, 1);
        createUIGroup(simComposite, 1);
        createNedPathGroup(simComposite, 1);
        createAdditionalGroup(simComposite, 1);
        setControl(mainComposite);

        updateMessageVisibility();
	}
    
    @Override
    public void activated(ILaunchConfigurationWorkingCopy workingCopy) {
    	// hide the tab content if not an omnetpp project
    	updateMessageVisibility();
    	super.activated(workingCopy);
    }

	protected void createIniGroup(Composite parent, int colSpan) {
		Composite comp = SWTFactory.createComposite(parent, 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

		SWTFactory.createLabel(comp, "Initialization file(s):", 1);

		fInifileText = SWTFactory.createSingleText(comp, 1);
		fInifileText.setToolTipText("The INI file(s) defining parameters and configuration blocks (default: omnetpp.ini, relative to the working directory)");
		fInifileText.addModifyListener(this);
		fInifileText.addFocusListener(new FocusAdapter() {
            @Override
            public void focusLost(FocusEvent e) {
                updateConfigCombo();
                updateNedPathText();
            }
		});

		Button browseInifileButton = SWTFactory.createPushButton(comp, "Browse...", null);
		browseInifileButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleBrowseInifileButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

    protected void createConfigGroup(Composite parent, int colSpan) {
		Composite comp = SWTFactory.createGroup(parent, "Configuration", 4, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(comp, "Configuration name:",1);

        fConfigCombo = SWTFactory.createCombo(comp, SWT.BORDER | SWT.READ_ONLY, 3, new String[] {});
		fConfigCombo.setToolTipText("The configuration from the INI file that should be executed");
		fConfigCombo.setVisibleItemCount(10);
		fConfigCombo.addModifyListener(this);

		SWTFactory.createLabel(comp, "Run number:",1);

		int runSPan = cdtContributed ? 3 : 1;
        fRunText = SWTFactory.createSingleText(comp, runSPan);
        fRunText.addModifyListener(this);
        HoverSupport hover = new HoverSupport();
        hover.adapt(fRunText, new IHoverTextProvider() {

            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outPreferredSize) {
                if (infoText == null)
                    infoText = LaunchPlugin.getSimulationRunInfo(config);
                outPreferredSize.preferredWidth = 350;
                return HoverSupport.addHTMLStyleSheet(DEFAULT_RUNTOOLTIP+"<pre>"+infoText+"</pre>");
            }
        });

        // parallel execution is not possible under CDT
        if (!cdtContributed) {
            SWTFactory.createLabel(comp, "Processes to run in parallel:", 1);
            fParallelismSpinner = new Spinner(comp, SWT.BORDER);
            fParallelismSpinner.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false));
            fParallelismSpinner.setMinimum(1);
            fParallelismSpinner.addModifyListener(this);
        }
    }

    protected void createUIGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(comp, "User interface:", 1);

        fDefaultEnvButton = SWTFactory.createRadioButton(comp, "Default");
        fDefaultEnvButton.setLayoutData(new GridData());
        fDefaultEnvButton.setSelection(true);
        fDefaultEnvButton.addSelectionListener(this);

        fCmdEnvButton = SWTFactory.createRadioButton(comp, "Command line");
        fCmdEnvButton.setLayoutData(new GridData());
        fCmdEnvButton.addSelectionListener(this);

        fTkEnvButton = SWTFactory.createRadioButton(comp, "Tcl/Tk");
        fTkEnvButton.setLayoutData(new GridData());
        fTkEnvButton.addSelectionListener(this);

        fOtherEnvButton = SWTFactory.createRadioButton(comp, "Other:");
        fOtherEnvButton.setLayoutData(new GridData());
        fOtherEnvButton.addSelectionListener(this);

        fOtherEnvText = SWTFactory.createSingleText(comp, 1);
        fOtherEnvText.setToolTipText("Specify the custom environment name");
        fOtherEnvText.addModifyListener(this);
    }

    protected void createLibraryGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(comp, "Dynamically loaded libraries:", 1);

        fLibraryText = SWTFactory.createSingleText(comp, 1);
        fLibraryText.setToolTipText("DLLs or shared libraries to load (without extension, relative to the working directory)");
        fLibraryText.addModifyListener(this);

        Button browseLibrariesButton = SWTFactory.createPushButton(comp, "Browse...", null);
        browseLibrariesButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseLibrariesButtonSelected();
                updateLaunchConfigurationDialog();
            }
        });
    }

    protected void createNedPathGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 2, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(comp, "NED Source Path:", 1);
        fNedPathText = SWTFactory.createSingleText(comp, 1);
        fNedPathText.setToolTipText("Specify the directories where NED files read from (relative to the first selected INI file)");
        fNedPathText.addModifyListener(this);
    }

    protected void createAdditionalGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 2, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(comp, "Additional arguments:", 1);
        fAdditionalText = SWTFactory.createSingleText(comp, 1);
        fAdditionalText.setToolTipText("Specify additional command line arguments");
        fAdditionalText.addModifyListener(this);
    }
	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#initializeFrom(org.eclipse.debug.core.ILaunchConfiguration)
	 */
    private enum ArgType {INI, CONFIG, RUN, UI, LIB, NEDPATH, UNKNOWN};

    public void initializeFrom(ILaunchConfiguration config) {
	    this.config = config;
        try {
            ArgType nextType = ArgType.UNKNOWN;
            String args[] = StringUtils.split(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, ""));
            String restArgs = "";        // the rest of the arguments we cannot recognize
            String iniArgs = "", libArgs = "", configArg="", runArg="", uiArg ="", nedPathArg ="";
            for (int i=0; i<args.length; ++i) {
                switch (nextType) {
                    case INI:
                        iniArgs += args[i]+" ";
                        nextType = ArgType.UNKNOWN;
                        continue;
                    case LIB:
                        libArgs += args[i]+" ";
                        nextType = ArgType.UNKNOWN;
                        continue;
                    case CONFIG:
                        configArg = args[i];
                        nextType = ArgType.UNKNOWN;
                        continue;
                    case RUN:
                        runArg = args[i];
                        nextType = ArgType.UNKNOWN;
                        continue;
                    case UI:
                        uiArg = args[i];
                        nextType = ArgType.UNKNOWN;
                        continue;
                    case NEDPATH:
                        nedPathArg = args[i];
                        nextType = ArgType.UNKNOWN;
                        continue;
                }

                if ("-f".equals(args[i]))
                    nextType = ArgType.INI;
                else if ("-c".equals(args[i]))
                    nextType = ArgType.CONFIG;
                else if ("-r".equals(args[i]))
                    nextType = ArgType.RUN;
                else if ("-u".equals(args[i]))
                    nextType = ArgType.UI;
                else if ("-l".equals(args[i]))
                    nextType = ArgType.LIB;
                else if ("-n".equals(args[i]))
                    nextType = ArgType.NEDPATH;
                else {
                    nextType = ArgType.UNKNOWN;
                    restArgs += args[i]+" ";
                }
            }

            // set the controls
            fInifileText.setText(iniArgs.trim());
            fLibraryText.setText(libArgs.trim());
            fAdditionalText.setText(restArgs.trim());
            fNedPathText.setText(nedPathArg.trim());
            fOtherEnvText.setText("");
            if ("".equals(uiArg)) {
                fDefaultEnvButton.setSelection(true);
            }
            else {
                fDefaultEnvButton.setSelection(false);
                if ("Cmdenv".equals(uiArg))
                    fCmdEnvButton.setSelection(true);
                else if ("Tkenv".equals(uiArg))
                    fTkEnvButton.setSelection(true);
                else {
                    fOtherEnvButton.setSelection(true);
                    fOtherEnvText.setText(uiArg.trim());
                }
            }
            fOtherEnvText.setEnabled(fOtherEnvButton.getSelection());
            updateUIGroup();

            updateConfigCombo();
            setConfigName(configArg.trim());

            if (cdtContributed)
                // if we are contributed to the CDT we get the value from the command line
                fRunText.setText(runArg.trim());
            else
                // otherwise we get it from a separate attribute
                fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.ATTR_RUN, ""));

            if (fParallelismSpinner != null)
                fParallelismSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.ATTR_NUM_CONCURRENT_PROCESSES, 1));

        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
	}

    protected void updateNedPathText() {
        // skip updating if not yet initialized
        if (config == null)
            return;
        IFile[] inifiles = getIniFiles();

        if (inifiles == null || inifiles.length == 0)
            return;

        String nedPathBase = inifiles[0].getProject().getFullPath().toPortableString();
        String nedPath = fNedPathText.getText();
        nedPath = nedPath.replaceFirst("\\$\\{ned_path:.*?\\}", "\\$\\{ned_path:"+nedPathBase+"\\}");
        fNedPathText.setText(nedPath);
    }

    protected void updateConfigCombo() {
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
    }

    protected void updateMessageVisibility() {
        boolean showNotOmnetppProjMessage = !isOmnetppProject() && cdtContributed;
        
        ((GridData)simComposite.getLayoutData()).exclude = showNotOmnetppProjMessage;
        ((GridData)notOmnetppProjectMessageComposite.getLayoutData()).exclude = !showNotOmnetppProjMessage;
        simComposite.setVisible(!showNotOmnetppProjMessage);
        notOmnetppProjectMessageComposite.setVisible(showNotOmnetppProjMessage);
        ((Composite)getControl()).layout();
    }

	public void performApply(ILaunchConfigurationWorkingCopy config) {
		if (!isOmnetppProject())
			return;
		
        String arg = "";
        if (StringUtils.isNotBlank(fInifileText.getText()))
            arg += "-f "+ StringUtils.join(StringUtils.split(fInifileText.getText())," -f ")+" ";
        if (StringUtils.isNotBlank(fLibraryText.getText()))
            arg += "-l "+ StringUtils.join(StringUtils.split(fLibraryText.getText())," -l ")+" ";
        if (StringUtils.isNotBlank(getConfigName()))
            arg += "-c "+getConfigName()+" ";
        if (StringUtils.isNotBlank(fNedPathText.getText()))
            arg += "-n "+StringUtils.trimToEmpty(fNedPathText.getText())+" ";
        // if we are contributed to the CDT launch dialog, we should store the run parameter into the command line
        String strippedRun = StringUtils.deleteWhitespace(fRunText.getText());
        if (cdtContributed) {
            if (StringUtils.isNotBlank(fRunText.getText()))
                arg += "-r "+strippedRun+" ";
        }
        else {
            // otherwise (stand-alone starter) we store it into a separate attribute
            config.setAttribute(IOmnetppLaunchConstants.ATTR_RUN, strippedRun);
        }

        if (fParallelismSpinner != null)
            config.setAttribute(IOmnetppLaunchConstants.ATTR_NUM_CONCURRENT_PROCESSES, fParallelismSpinner.getSelection());

        if (fCmdEnvButton.getSelection())
            arg += "-u Cmdenv ";
        if (fTkEnvButton.getSelection())
            arg += "-u Tkenv ";
        if (fOtherEnvButton.getSelection())
            arg += "-u "+fOtherEnvText.getText()+" ";
        arg += fAdditionalText.getText();
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, arg);
        // clear the run info text, so next time it will be re-requested
        infoText = null;
	}

	protected void handleBrowseInifileButtonSelected() {
        ElementTreeSelectionDialog dialog
            = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                                                         new OmnetppLaunchUtils.FilteredWorkbenchContentProvider(".*\\.ini"));
        dialog.setTitle("Select INI Files");
        dialog.setMessage("Select the initialization file(s) for the simulation.\n" +
        		          "Multiple files can be selected.");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            String inifiles = "";
            for (Object resource : dialog.getResult()) {
                if (resource instanceof IFile)
                        inifiles += OmnetppLaunchUtils.makeRelativePathTo(((IFile)resource).getRawLocation(),
                                                       getWorkingDirectoryPath()).toString()+" ";
            }
            fInifileText.setText(inifiles.trim());
            updateConfigCombo();
            updateNedPathText();
        }
	}

	protected void handleBrowseLibrariesButtonSelected() {
	    String extensionRegexp = ".*\\.";
	    if (SWT.getPlatform().equals("win32"))
	        extensionRegexp += "dll";
	    else if (SWT.getPlatform().equals("carbon"))
            extensionRegexp += "dylib";
	    else
            extensionRegexp += "so";
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
	        for (Object resource : dialog.getResult()) {
	            if (resource instanceof IFile)
                    libfiles += OmnetppLaunchUtils.makeRelativePathTo(((IFile)resource).getRawLocation(),
                            getWorkingDirectoryPath()).toString()+" ";
	        }
	        fLibraryText.setText(libfiles.trim());
	    }
    }

	@Override
    public boolean isValid(ILaunchConfiguration config) {
	    setErrorMessage(null);
	    setMessage(null);

	    if (!isOmnetppProject())
			return true;

		IFile ifiles[] = getIniFiles();
        if (ifiles == null) {
            setErrorMessage("Initialization file does not exist, or not accessible in workspace");
            return false;
        }
        for (IFile ifile : ifiles)
            if (!ifile.isAccessible()) {
                setErrorMessage("Initialization file "+ifile.getName()+" does not exist, or not accessible in workspace");
                return false;
            }

        IFile libfiles[] = getLibFiles();
        if ( libfiles == null) {
            setErrorMessage("Library file does not exist, or not accessible in workspace");
            return false;
        }
        for (IFile ifile : libfiles)
            if (!ifile.isAccessible()) {
                setErrorMessage("Library file "+ifile.getName()+" does not exist, or not accessible in workspace");
                return false;
            }

        if ("".equals(StringUtils.deleteWhitespace(fRunText.getText())) && isScenario() ) {
            setErrorMessage("Run number(s) must be specified if a scenario is selected");
            return false;
        }

        Integer runs[] = LaunchPlugin.parseRuns(StringUtils.deleteWhitespace(fRunText.getText()), 2);
        if (runs == null) {
            setErrorMessage("The run number(s) should be in a format like: 0,2,7,9..11 or use * for ALL runs");
            return false;
        }

        if (fOtherEnvButton.getSelection() && StringUtils.isEmpty(fOtherEnvText.getText())) {
            setErrorMessage("Environment type must be specified");
            return false;
        }
        if (!fCmdEnvButton.getSelection() && !fOtherEnvButton.getSelection()
                && runs!=null && runs.length>1) {
            setErrorMessage("Multiple runs are only supported for the Command line environment");
            return false;
        }
		return true;
	}

	public void setDefaults(ILaunchConfigurationWorkingCopy config) {
		// FIXME not added correctly if first we assign a non omnet project and we want to change
		// it only later
		if (isOmnetppProject())
			config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "-n ${ned_path:/}");
	}

	@Override
	public Image getImage() {
	    return LaunchPlugin.getImage("/icons/full/ctool16/omnetsim.gif");
	}

	public String getName() {
		return "Simulation";
	}

    @Override
    public String getId() {
        return IOmnetppLaunchConstants.OMNETPP_LAUNCH_ID+".simulationTab";
    }

    /**
     * updates the control states in the UI group
     */
    private void updateUIGroup() {
        if (!fOtherEnvButton.getSelection())
            fOtherEnvText.setText("");
        fOtherEnvText.setEnabled(fOtherEnvButton.getSelection());

        if (fParallelismSpinner != null) {
            fParallelismSpinner.setEnabled(fCmdEnvButton.getSelection());
            if (!fCmdEnvButton.getSelection())
                fParallelismSpinner.setSelection(1);
        }
    }

    /**
     * Expands and returns the working directory attribute of the given launch
     * configuration. Returns <code>null</code> if a working directory is not
     * specified. If specified, the working is verified to point to an existing
     * directory in the local file system.
     *
     * @return an absolute path to a directory in the local file system, or
     * <code>null</code> if unspecified
     * @throws CoreException if unable to retrieve the associated launch
     * configuration attribute, if unable to resolve any variables, or if the
     * resolved location does not point to an existing directory in the local
     * file system
     */
    private IPath getWorkingDirectoryPath(){
        if (config == null)
            return null;
        return LaunchPlugin.getWorkingDirectoryPath(config);
    }

    /**
     * Returns the project in which the currently selected EXE file is located
     */
    protected IProject getProject() {
        if (config == null)
            return null;
        
        try {
            // if we are contributing to cdt we can get the project attribute
            if (cdtContributed) {
                String projectName = config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
                return StringUtils.isNotBlank(projectName) ? ResourcesPlugin.getWorkspace().getRoot().getProject(projectName) : null;
            }
            else {
                String name = config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
                IFile exeFile = StringUtils.isNotBlank(name) ? ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(name)) : null;
                return exeFile != null ? exeFile.getProject() : null;
            }
        }
        catch (CoreException e) {
            // if the config attribute cannot be retrieved we dont know what project we are in
            return null;
        }
    }

    /**
     * true if project is specified (directly or indirectly in the exe file name) and it ha omnetpp nature 
     */
    protected boolean isOmnetppProject() {
        IProject project = getProject();
        return project != null && ProjectUtils.hasOmnetppNature(project);
    }

    /**
     * Returns the selected library files. Returns null on error
     */
    private IFile[] getLibFiles() {
        List<IFile> result = new ArrayList<IFile>();
        String names[] =  StringUtils.split(fLibraryText.getText().trim());

        for (String name : names) {
            IPath iniPath = getWorkingDirectoryPath().append(name).makeAbsolute();
            IFile[] ifiles = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(iniPath);
            if (ifiles.length == 1)
                result.add(ifiles[0]);
            else
                return null;
        }
        return result.toArray(new IFile[result.size()]);
    }

    /**
     * Returns the selected ini files or (omnetpp.ini) if the inifile text line
     * is empty. Returns null on error.
     */
    private IFile[] getIniFiles() {
        List<IFile> result = new ArrayList<IFile>();
        IPath workingDirectoryPath = getWorkingDirectoryPath();
        if (workingDirectoryPath == null)
            return null;
        String names[] =  StringUtils.split(fInifileText.getText().trim());
        // default ini file is omnetpp ini
        if (names.length == 0)
            names = new String[] {"omnetpp.ini"};

        for (String name : names) {
            IPath iniPath = workingDirectoryPath.append(name).makeAbsolute();
            IFile[] ifiles = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(iniPath);
            if (ifiles.length == 1)
                result.add(ifiles[0]);
            else
                return null;
        }
        return result.toArray(new IFile[result.size()]);
    }

    /**
     * Returns all the configuration names found in the supplied inifiles
     */
    private String [] getConfigNames(IFile[] inifiles) {
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
     * Returns whether the currently selected line in the config combo is a scenario
     */
    private boolean isScenario() {
    	//FIXME scenario sections are now also called [Config ...], so more sophisticated parsing is needed to figure out of which sections are in fact scenarios --Andras
        return fConfigCombo.getText().contains("(scenario)"); //FIXME by checking the label??? --Andras
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

    public void modifyText(ModifyEvent e) {
        if (changeListener != null)
            changeListener.widgetChanged();
        updateLaunchConfigurationDialog();
    }

    public void widgetDefaultSelected(SelectionEvent e) {
        if (changeListener != null)
            changeListener.widgetChanged();
        updateLaunchConfigurationDialog();
    }

    public void widgetSelected(SelectionEvent e) {
        updateUIGroup();
        if (changeListener != null)
            changeListener.widgetChanged();
        updateLaunchConfigurationDialog();
    }

}
