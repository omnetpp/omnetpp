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
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * A launch configuration tab that displays and edits omnetpp project
 */
public class SimulationTab extends OmnetppLaunchTab implements
            SelectionListener, ModifyListener {

	// UI widgets
	protected Text fInifileText;
	protected Combo fConfigCombo;
    protected Text fRunText;
    protected Button fDefaultEnvButton;
    protected Button fCmdEnvButton;
    protected Button fTkEnvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherEnvText;
    protected Text fLibraryText;
    protected Text fAdditionalText;


    /**
     * Reads the ini file and enumerates all config sections. resolves include directives recursively
     * @author rhornig
     */
    protected class ConfigEnumeratorCallback extends InifileParser.ParserAdapter {
        class Section {
            String name;
            boolean isScenario = false;
            String network;
            String extnds;
            String descr;
            @Override
            public String toString() {
                String additional = (StringUtils.isEmpty(descr)? "" : " "+descr)+
                                    (isScenario ? " (scenario)" : "")+
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
            } else {
                setErrorMessage("Unknown directive in inifile");
            }
        }

        @Override
        public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String comment) {
            if("extends".equals(key)){
                getSectionForName(currentSectionName).extnds = value;
            }
            if("description".equals(key)){
                getSectionForName(currentSectionName).descr = value;
            }
            if("network".equals(key)){
                getSectionForName(currentSectionName).network = value;
            }
        }

        @Override
        public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String comment) {
            currentSectionName = StringUtils.removeStart(sectionName,"Config ");
            currentSectionName = StringUtils.removeStart(currentSectionName,"Scenario ");
            getSectionForName(currentSectionName).isScenario = sectionName.startsWith("Scenario");
        }

        /**
         * @param name
         * @return The section with a given name (if not present in the map, adds it)
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
            for(Object child : super.getChildren(element)) {
                if (child instanceof IFile && ((IFile)child).getName().matches(regexp)
                                || getChildren(child).length > 0)
                        filteredChildren.add(child);
            }
            return filteredChildren.toArray();
        }
    };

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#createControl(org.eclipse.swt.widgets.Composite)
	 */
	public void createControl(Composite parent) {
		Composite comp = SWTFactory.createComposite(parent, 1, 1, GridData.FILL_HORIZONTAL);
		createLibraryGroup(comp, 1);
		createIniGroup(comp, 1);
        createConfigGroup(comp, 1);
        createUIGroup(comp, 1);
        createAdditionalGroup(comp, 1);
        setControl(comp);
	}

	protected void createIniGroup(Composite parent, int colSpan) {
		Composite comp = SWTFactory.createComposite(parent, 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

		SWTFactory.createLabel(comp, "Initialization file(s):", 1);

		fInifileText = SWTFactory.createSingleText(comp, 1);
		fInifileText.setToolTipText("The INI file(s) defining parameters and configuration blocks (default: omnetpp.ini)");
		fInifileText.addModifyListener(this);
		fInifileText.addFocusListener(new FocusAdapter() {
            @Override
            public void focusLost(FocusEvent e) {
                updateConfigCombo();
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
		Composite comp = SWTFactory.createGroup(parent, "Configuration", 2, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(comp, "Configuration name:",1);

        fConfigCombo = SWTFactory.createCombo(comp, SWT.BORDER | SWT.READ_ONLY, 1, new String[] {});
		fConfigCombo.setToolTipText("The configuration from the INI file that should be executed");
		fConfigCombo.setVisibleItemCount(10);
		fConfigCombo.addModifyListener(this);

		SWTFactory.createLabel(comp, "Run number:",1);

        fRunText = SWTFactory.createSingleText(comp, 1);
        fRunText.setToolTipText("The run number that should be executed");
        fRunText.addModifyListener(this);
	}

    protected void createUIGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(comp, "User interface:", 1);

        fDefaultEnvButton = SWTFactory.createRadioButton(comp, "Default");
        fDefaultEnvButton.setSelection(true);
        fDefaultEnvButton.addSelectionListener(this);

        fCmdEnvButton = SWTFactory.createRadioButton(comp, "Command line");
        fCmdEnvButton.addSelectionListener(this);

        fTkEnvButton = SWTFactory.createRadioButton(comp, "Tcl/Tk");
        fTkEnvButton.addSelectionListener(this);

        fOtherEnvButton = SWTFactory.createRadioButton(comp, "Other:");
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
        fLibraryText.setToolTipText("DLLs or shared libraries to load (without extension)");
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

    protected void createAdditionalGroup(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 2, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout ld = (GridLayout)comp.getLayout();
        ld.marginHeight = 1;

        SWTFactory.createLabel(comp, "Additional arguments:", 1);
        fAdditionalText = SWTFactory.createSingleText(comp, 1);
        fAdditionalText.setToolTipText("Specify additonal command line arguments");
        fAdditionalText.addModifyListener(this);
    }
	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#initializeFrom(org.eclipse.debug.core.ILaunchConfiguration)
	 */
    private enum ArgType {INI, CONFIG, RUN, UI, LIB, UNKNOWN};
	@Override
    public void initializeFrom(ILaunchConfiguration config) {
	    super.initializeFrom(config);
        try {
            ArgType nextType = ArgType.UNKNOWN;
            String args[] = StringUtils.split(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, EMPTY_STRING));
            String restArgs = "";        // the rest of the arguments we cannot recognize
            String iniArgs = "", libArgs = "", configArg="", runArg="", uiArg ="";
            for(int i=0; i<args.length; ++i) {
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
                }

                if("-f".equals(args[i]))
                    nextType = ArgType.INI;
                else if ("-c".equals(args[i]))
                    nextType = ArgType.CONFIG;
                else if ("-r".equals(args[i]))
                    nextType = ArgType.RUN;
                else if ("-u".equals(args[i]))
                    nextType = ArgType.UI;
                else if ("-l".equals(args[i]))
                    nextType = ArgType.LIB;
                else {
                    nextType = ArgType.UNKNOWN;
                    restArgs += args[i]+" ";
                }
            }

            // set the controls
            fInifileText.setText(iniArgs);
            setConfigName(configArg);
            fRunText.setText(runArg);
            fLibraryText.setText(libArgs);
            fAdditionalText.setText(restArgs);
            fOtherEnvText.setText("");
            if ("".equals(uiArg)) {
                fDefaultEnvButton.setSelection(true);
            } else {
                fDefaultEnvButton.setSelection(false);
                if ("cmdenv".equals(uiArg))
                    fCmdEnvButton.setSelection(true);
                else if ("tkenv".equals(uiArg))
                    fTkEnvButton.setSelection(true);
                else {
                    fOtherEnvButton.setSelection(true);
                    fOtherEnvText.setText(uiArg);
                }
            }
            fOtherEnvText.setEnabled(fOtherEnvButton.getSelection());

            updateConfigCombo();
        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
	}

    protected void updateConfigCombo() {
        IFile[] inifiles = getInifiles();
        if (inifiles == null)
            fConfigCombo.setItems(new String[] {});
        else {
            String currentSelection = getConfigName();
            String newConfigNames[] = getConfigNames(inifiles);
            if(!ObjectUtils.equals(StringUtils.join(fConfigCombo.getItems()," - "),
                                   StringUtils.join(newConfigNames," - "))) {
                fConfigCombo.setItems(newConfigNames);
                setConfigName(currentSelection);
            }
        }
    }

	public void performApply(ILaunchConfigurationWorkingCopy config) {
        String arg = "";
        if (!"".equals(fInifileText.getText()))
            arg += "-f "+ StringUtils.join(StringUtils.split(fInifileText.getText())," -f ")+" ";
        if (!"".equals(fLibraryText.getText()))
            arg += "-l "+ StringUtils.join(StringUtils.split(fLibraryText.getText())," -l ")+" ";
        if (!"".equals(getConfigName()))
            arg += "-c "+getConfigName()+" ";
        if (!"".equals(fRunText.getText()))
            arg += "-r "+fRunText.getText()+" ";
        if (fCmdEnvButton.getSelection())
            arg += "-u cmdenv ";
        if (fTkEnvButton.getSelection())
            arg += "-u tkenv ";
        if (fOtherEnvButton.getSelection())
            arg += "-u "+fOtherEnvText.getText()+" ";
        arg += fAdditionalText.getText();
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, arg);
	}

	protected void handleBrowseInifileButtonSelected() {
        ElementTreeSelectionDialog dialog
            = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                                                         new FilteredWorkbenchContentProvider(".*\\.ini"));
        dialog.setTitle("Select INI Files");
        dialog.setMessage("Select the initialization file(s) for the simulation.\n" +
        		          "Multiple files can be selected.");
        dialog.setInput(getProject());
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            String inifiles = "";
            for (Object resource : dialog.getResult()) {
                if (resource instanceof IFile)
                    inifiles += ((IFile)resource).getProjectRelativePath().toString()+" ";
            }
            fInifileText.setText(inifiles);
            updateConfigCombo();
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
	    dialog.setInput(getProject());
	    dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
	    if (dialog.open() == IDialogConstants.OK_ID) {
	        String libfiles = "";
	        for (Object resource : dialog.getResult()) {
	            if (resource instanceof IFile)
	                libfiles += ((IFile)resource).getProjectRelativePath().removeFileExtension().toString()+" ";
	        }
	        fLibraryText.setText(libfiles);
	    }
    }

	@Override
    public boolean isValid(ILaunchConfiguration config) {

		setErrorMessage(null);
		setMessage(null);

        if (getProject() == null) {
            setErrorMessage("Project must be opened");
            return false;
        }

        if (getInifiles() == null) {
            setErrorMessage("Initialization file does not exist");
            return false;
        }

		return true;
	}

	public void setDefaults(ILaunchConfigurationWorkingCopy config) {
	}

	@Override
	public Image getImage() {
	    return LaunchPlugin.getImage("/icons/full/ctool16/omnetsim.gif");
	}

	public String getName() {
		return "Simulation";
	}

    public void widgetDefaultSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
    }

    public void widgetSelected(SelectionEvent e) {
        if (!fOtherEnvButton.getSelection())
            fOtherEnvText.setText("");
        fOtherEnvText.setEnabled(fOtherEnvButton.getSelection());

        updateLaunchConfigurationDialog();
    }

    public void modifyText(ModifyEvent e) {
        updateLaunchConfigurationDialog();
        // enable/disable runs if config combo box has changed
        if (e.widget == fConfigCombo) {
            fRunText.setEnabled(isScenario());
            if(!isScenario())
                fRunText.setText("");
        }
    }

    /**
     * @return The selected project if exist and open otherwise NULL
     */
    protected IProject getProject() {
        IProject project = null;
        try {
            String projectName = getCurrentLaunchConfiguration().getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, EMPTY_STRING);
            if (StringUtils.isNotEmpty(projectName))
                project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
        } catch (Exception e) {}
        if (project == null || !project.isOpen())
            return null;
        return project;
    }

    /**
     * @param config
     * @return The selected ini files or (omnetpp.ini) if the inifile text line is empty
     * on error it returns NULL
     */
    private IFile[] getInifiles() {
        // FIXME lookup the inifile relative to the working directory not to the project
        List<IFile> result = new ArrayList<IFile>();
        String names[] =  StringUtils.split(fInifileText.getText().trim());
        IProject project = getProject();
        if (project == null)
            return null;
        // default ini file is omnetpp ini
        if (names.length == 0)
            names = new String[] {"omnetpp.ini"};

        for(String name : names) {
        IPath iniPath = new Path(name);
            if (!iniPath.isAbsolute()) {
                if (!project.getFile(name).exists())
                    return null;
                result.add(project.getFile(name));
            } else {
                if (!iniPath.toFile().exists())
                    return null;
                IFile[] ifiles = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(iniPath);
                if (ifiles.length == 1)
                    result.add(ifiles[0]);
                return null;
            }
        }
        return result.toArray(new IFile[result.size()]);
    }


    /**
     * @param inifiles
     * @return All the configuration names found in the supplied inifiles
     */
    private String [] getConfigNames(IFile[] inifiles) {
        Map<String,ConfigEnumeratorCallback.Section> sections = new LinkedHashMap<String, ConfigEnumeratorCallback.Section>();
        if (inifiles != null)
            for(IFile inifile : inifiles) {
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
        for(ConfigEnumeratorCallback.Section sec : sections.values())
            result.add(sec.toString());
        return result.toArray(new String[] {});
    }

    /**
     * @return Whether the currently selected line in the config combo is a scenario
     */
    private boolean isScenario() {
        return fConfigCombo.getText().contains("(scenario)");
    }

    /**
     * @return The currently selected config name (after stripping the comments an other stuff)
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
        for(String line : fConfigCombo.getItems())
            if (line.startsWith(name)) {
                fConfigCombo.setText(line);
                return;
            }
    }

    @Override
    public String getId() {
        return "org.omnetpp.launch.simulationTab";
    }
}
