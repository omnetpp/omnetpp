package org.omnetpp.launch.tabs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.internal.ui.SWTFactory;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusAdapter;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
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
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

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
public class SimulationTab extends OmnetppLaunchTab  {
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

    /**
     * Reads the ini file and enumerates all config sections. resolves include directives recursively
     */
    protected class ConfigEnumeratorCallback extends InifileParser.ParserAdapter {
        class Section {
            String name;
            String network;
            String extnds;
            String descr;
            @Override
            public String toString() {
                String additional = (StringUtils.isEmpty(descr)? "" : " "+descr)+
                                    (StringUtils.isEmpty(extnds)? "" : " (extends: "+extnds+")")+
                                    (StringUtils.isEmpty(network)? "" : " (network: "+network+")");
                return name +(StringUtils.isEmpty(additional) ? "" : " --"+additional);
            }
        }
        IFile currentFile;
        String currentSectionName;
        Map<String, Section> result;

        public ConfigEnumeratorCallback(IFile file, Map<String, Section> result) {
            this.currentFile = file;
            this.result = result;
        }

        @Override
        public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String comment) {
            if (directive.equals("include")) {
                // recursively parse the included file
                try {
                    IFile file = currentFile.getParent().getFile(new Path(args));
                    new InifileParser().parse(file, new ConfigEnumeratorCallback(file, result));
                }
                catch (Exception e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                }
            }
            else {
                setErrorMessage("Unknown directive in inifile");
            }
        }

        @Override
        public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String comment) {
            if ("extends".equals(key)){
                getSectionForName(currentSectionName).extnds = value;
            }
            if ("description".equals(key)){
                getSectionForName(currentSectionName).descr = value;
            }
            if ("network".equals(key)){
                getSectionForName(currentSectionName).network = value;
            }
        }

        @Override
        public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String comment) {
            currentSectionName = StringUtils.removeStart(sectionName,"Config ");
        }

        /**
         * Returns the section with a given name (if not present in the map, adds it)
         */
        private Section getSectionForName(String name) {
            Section currSection = result.get(name);
            // if it still not in the
            if (currSection == null) {
                currSection = new Section();
                currSection.name = name;
                result.put(name, currSection);
            }
            return currSection;
        }
    }

    /**
     * A workbench content provider that returns only files with a given extension
     * @author rhornig
     */
    protected class FilteredWorkbenchContentProvider extends WorkbenchContentProvider {
        private final String regexp;

        /**
         * @param regexp The regular expression where matches should be displayed
         */
        public FilteredWorkbenchContentProvider(String regexp) {
            super();
            this.regexp = regexp;
        }

        @Override
        public Object[] getChildren(Object element) {
            List<Object> filteredChildren = new ArrayList<Object>();
            for (Object child : super.getChildren(element)) {
                if (child instanceof IFile && ((IFile)child).getName().matches(regexp)
                                || getChildren(child).length > 0)
                        filteredChildren.add(child);
            }
            return filteredChildren.toArray();
        }
    };

    public SimulationTab() {
        super();
    }

    /**
     * @param embeddingTab
     * @param cdtContributed Whether we are contributing to CDT (true) or using the stand-alone launcher (false)
     *        default is true (CDT contribution)
     */
    SimulationTab(OmnetppLaunchTab embeddingTab, boolean cdtContributed) {
        super(embeddingTab);
        this.cdtContributed = cdtContributed;
    }

    public void createControl(Composite parent) {
		Composite comp = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
		createLibraryGroup(comp, 1);
		createIniGroup(comp, 1);
        createConfigGroup(comp, 1);
        createUIGroup(comp, 1);
        createNedPathGroup(comp, 1);
        createAdditionalGroup(comp, 1);
        setControl(comp);
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
                    infoText = LaunchPlugin.getSimulationRunInfo(getCurrentLaunchConfiguration());
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
	@Override
    public void initializeFrom(ILaunchConfiguration config) {
	    super.initializeFrom(config);
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
                fParallelismSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.ATTR_PARALLELISM, 1));

        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
	}

    protected void updateNedPathText() {
        // skip updating if not yet initialized
        if (getCurrentLaunchConfiguration() == null)
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
        if (inifiles == null)
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

	public void performApply(ILaunchConfigurationWorkingCopy config) {
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
            config.setAttribute(IOmnetppLaunchConstants.ATTR_PARALLELISM, fParallelismSpinner.getSelection());

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
                                                         new FilteredWorkbenchContentProvider(".*\\.ini"));
        dialog.setTitle("Select INI Files");
        dialog.setMessage("Select the initialization file(s) for the simulation.\n" +
        		          "Multiple files can be selected.");
        dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            String inifiles = "";
            for (Object resource : dialog.getResult()) {
                if (resource instanceof IFile)
                        inifiles += makeRelativePathTo(((IFile)resource).getRawLocation(),
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
	            new FilteredWorkbenchContentProvider(extensionRegexp));
	    dialog.setTitle("Select Shared Libraries");
	    dialog.setMessage("Select the library file(s) you want to load at the beginning of the simulation.\n" +
	    		          "Multiple files can be selected.");
	    dialog.setInput(ResourcesPlugin.getWorkspace().getRoot());
	    dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
	    if (dialog.open() == IDialogConstants.OK_ID) {
	        String libfiles = "";
	        for (Object resource : dialog.getResult()) {
	            if (resource instanceof IFile)
                    libfiles += makeRelativePathTo(((IFile)resource).getRawLocation(),
                            getWorkingDirectoryPath()).toString()+" ";
	        }
	        fLibraryText.setText(libfiles.trim());
	    }
    }

	@Override
    public boolean isValid(ILaunchConfiguration config) {
	    setErrorMessage(null);
	    setMessage(null);

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

    public void widgetDefaultSelected(SelectionEvent e) {
        super.widgetDefaultSelected(e);
    }

    public void widgetSelected(SelectionEvent e) {
        updateUIGroup();
        super.widgetSelected(e);
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
     * Returns the selected library files. Returns null on on error
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
        String names[] =  StringUtils.split(fInifileText.getText().trim());
        // default ini file is omnetpp ini
        if (names.length == 0)
            names = new String[] {"omnetpp.ini"};

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
     * Returns all the configuration names found in the supplied inifiles
     */
    private String [] getConfigNames(IFile[] inifiles) {
        Map<String,ConfigEnumeratorCallback.Section> sections = new LinkedHashMap<String, ConfigEnumeratorCallback.Section>();
        if (inifiles != null)
            for (IFile inifile : inifiles) {
                InifileParser iparser = new InifileParser();
                try {
                    iparser.parse(inifile, new ConfigEnumeratorCallback(inifile, sections));
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
        for (ConfigEnumeratorCallback.Section sec : sections.values())
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

}
