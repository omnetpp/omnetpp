package org.omnetpp.launch.tabs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

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
import org.omnetpp.launch.LaunchPluging;

/**
 * A launch configuration tab that displays and edits omnetpp project
 */
public class SimulationTab extends AbstractLaunchConfigurationTab implements
            SelectionListener, ModifyListener {
    protected static final String EMPTY_STRING = ""; //$NON-NLS-1$

	// UI widgets
	protected Text fInifileText;
	protected Combo fConfigCombo;
    protected Text fRunText;
    protected Button fDefaultEnvButton;
    protected Button fCmdEnvButton;
    protected Button fTkEnvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherText;
    // config we are working on
    private ILaunchConfiguration config;

    /**
     * Reads the ini file and enumerates all config sections. resolves include directives recursively
     * @author rhornig
     */
    protected class ConfigEnumeratorCallback extends InifileParser.ParserAdapter {
        IFile currentFile;
        Collection<String> result;

        public ConfigEnumeratorCallback(IFile file, Collection<String> result) {
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
        public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String comment) {
            if ("General".equals(sectionName) || sectionName.startsWith("Config")) {
                result.add(StringUtils.removeStart(sectionName, "Config "));
            }
        }
    }

    /**
     * A workbench content provider that returns only files with a given extension
     * @author rhornig
     */
    protected class FilteredWorkbenchContentProvider extends WorkbenchContentProvider {
        private final String extension;

        public FilteredWorkbenchContentProvider(String extension) {
            super();
            this.extension = extension;
        }

        @Override
        public Object[] getChildren(Object element) {
            List<Object> filteredChildren = new ArrayList<Object>();
            for(Object child : super.getChildren(element)) {
                if (child instanceof IFile && extension.equals(((IFile)child).getFileExtension())
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
		createIniGroup(comp, 1);
        createConfigGroup(comp, 1);
        createUIGroup(comp, 1);
        setControl(comp);
	}

	protected void createIniGroup(Composite parent, int colSpan) {
		Composite iniComp = SWTFactory.createComposite(parent, 3, colSpan, GridData.FILL_HORIZONTAL);

		SWTFactory.createLabel(iniComp, "Initialization file(s):", 1);

		fInifileText = SWTFactory.createSingleText(iniComp, 1);
		fInifileText.setToolTipText("The INI file(s) defining parameters and configuration blocks (default: omnetpp.ini)");
		fInifileText.addModifyListener(this);
		fInifileText.addFocusListener(new FocusAdapter() {
            @Override
            public void focusLost(FocusEvent e) {
                updateConfigCombo();
            }
		});

		Button browseInifileButton = SWTFactory.createPushButton(iniComp, "Browse...", null);
		browseInifileButton.addSelectionListener(new SelectionAdapter() {
			@Override
            public void widgetSelected(SelectionEvent evt) {
				handleBrowseInifileButtonSelected();
				updateLaunchConfigurationDialog();
			}
		});
	}

    protected void createConfigGroup(Composite parent, int colSpan) {
		Composite confComp = SWTFactory.createGroup(parent, "Configuration", 2, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(confComp, "Configuration name:",1);

        fConfigCombo = SWTFactory.createCombo(confComp, SWT.BORDER | SWT.READ_ONLY, 1, new String[] {});
		fConfigCombo.setToolTipText("The configuration from the INI file that should be executed");
		fConfigCombo.setVisibleItemCount(10);
		fConfigCombo.addModifyListener(this);

		SWTFactory.createLabel(confComp, "Run number:",1);

        fRunText = SWTFactory.createSingleText(confComp, 1);
        fRunText.setToolTipText("The run number that should be executed");
        fRunText.addModifyListener(this);
	}

    protected void createUIGroup(Composite parent, int colSpan) {
        Composite runComp = SWTFactory.createComposite(parent, 5, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(runComp, "User interface:", 5);

        fDefaultEnvButton = SWTFactory.createRadioButton(runComp, "Default");
        fDefaultEnvButton.setSelection(true);
        fDefaultEnvButton.addSelectionListener(this);

        fCmdEnvButton = SWTFactory.createRadioButton(runComp, "Command line");
        fCmdEnvButton.addSelectionListener(this);

        fTkEnvButton = SWTFactory.createRadioButton(runComp, "Tcl/Tk");
        fTkEnvButton.addSelectionListener(this);

        fOtherEnvButton = SWTFactory.createRadioButton(runComp, "Other:");
        fOtherEnvButton.addSelectionListener(this);

        fOtherText = SWTFactory.createSingleText(runComp, 1);
        fOtherText.setToolTipText("Specify the custom environment name");
        fOtherText.addModifyListener(this);
    }

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#initializeFrom(org.eclipse.debug.core.ILaunchConfiguration)
	 */
	public void initializeFrom(ILaunchConfiguration config) {
	    this.config = config;
        try {
            String guardedArg = "-inifiles "+config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, EMPTY_STRING)+" -";
            fInifileText.setText(StringUtils.trimToEmpty(StringUtils.substringBetween(guardedArg, "-inifiles ", " -")));
            fConfigCombo.setText(StringUtils.trimToEmpty(StringUtils.substringBetween(guardedArg, "-c ", " -")));
            fRunText.setText(StringUtils.trimToEmpty(StringUtils.substringBetween(guardedArg, "-r ", " -")));
            String userEnv = StringUtils.trimToEmpty(StringUtils.substringBetween(guardedArg, "-u ", " -"));
            fOtherText.setText("");
            if ("".equals(userEnv)) {
                fDefaultEnvButton.setSelection(true);
            } else {
                fDefaultEnvButton.setSelection(false);
                if ("cmdenv".equals(userEnv))
                    fCmdEnvButton.setSelection(true);
                else if ("tkenv".equals(userEnv))
                    fTkEnvButton.setSelection(true);
                else {
                    fOtherEnvButton.setSelection(true);
                    fOtherText.setText(userEnv);
                }
            }
            fOtherText.setEnabled(fOtherEnvButton.getSelection());

            updateConfigCombo();
        } catch (CoreException ce) {
            LaunchPluging.logError(ce);
        }
	}

    protected void updateConfigCombo() {
        IFile[] inifiles = getInifiles();
        String currentSelection = fConfigCombo.getText();
        fConfigCombo.setItems(new String[] {});
        if (inifiles != null)
            fConfigCombo.setItems(getConfigNames(inifiles));
        fConfigCombo.setText(currentSelection);
    }

	public void performApply(ILaunchConfigurationWorkingCopy config) {
        String arg = fInifileText.getText();
        if (!"".equals(fConfigCombo.getText()))
            arg += " -c "+fConfigCombo.getText();
        if (!"".equals(fRunText.getText()))
            arg += " -r "+fRunText.getText();
        if (fCmdEnvButton.getSelection())
            arg += " -u cmdenv";
        if (fTkEnvButton.getSelection())
            arg += " -u tkenv";
        if (fOtherEnvButton.getSelection())
            arg += " -u "+fOtherText.getText();
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, arg);
	}

	protected void handleBrowseInifileButtonSelected() {
        ElementTreeSelectionDialog dialog
            = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(),
                                                         new FilteredWorkbenchContentProvider("ini"));
        dialog.setTitle("Select one or more ini file");
        dialog.setMessage("Select the initialization file(s) for the simulation");
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
	    return LaunchPluging.getImage("/icons/full/ctool16/omnetsim.gif");
	}

	public String getName() {
		return "Simulation";
	}

    public void widgetDefaultSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
    }

    public void widgetSelected(SelectionEvent e) {
        if (!fOtherEnvButton.getSelection())
            fOtherText.setText("");
        fOtherText.setEnabled(fOtherEnvButton.getSelection());

        updateLaunchConfigurationDialog();
    }

    public void modifyText(ModifyEvent e) {
        updateLaunchConfigurationDialog();
    }

    /**
     * @return The selected project if exist and open otherwise NULL
     */
    private IProject getProject() {
        IProject project = null;
        try {
            project = ResourcesPlugin.getWorkspace().getRoot().getProject(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, EMPTY_STRING));
        } catch (CoreException e) {}
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
        Set<String> result = new LinkedHashSet<String>();
        result.add("");
        if (inifiles != null)
            for(IFile inifile : inifiles) {
                InifileParser iparser = new InifileParser();
                try {
                    iparser.parse(inifile, new ConfigEnumeratorCallback(inifile, result));
                } catch (ParseException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                } catch (CoreException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                } catch (IOException e) {
                    setErrorMessage("Error reading inifile: "+e.getMessage());
                }
            }
        return result.toArray(new String[] {});
    }

}
