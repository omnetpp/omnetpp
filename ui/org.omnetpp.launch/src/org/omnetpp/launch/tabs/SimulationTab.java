package org.omnetpp.launch.tabs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
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
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;

import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPluging;

/**
 * A launch configuration tab that displays and edits omnetpp project
 */
public class SimulationTab extends AbstractLaunchConfigurationTab implements
            SelectionListener, ModifyListener {

	// Ini UI widgets
	protected Text fInifileText;
	protected Button fBrowseInifileButton;

	// Config class UI widgets
	protected Combo fConfigCombo;

    // Run UI widgets
    protected Text fRunText;

    // Environment UI widgets
    protected Button fDefaultEnvButton;
    protected Button fCmdEnvButton;
    protected Button fTkEnvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherText;
    private ILaunchConfiguration config;

    protected class ConfigEnumeratorCallback extends InifileParser.ParserAdapter {
        IFile currentFile;
        List<String> result;

        public ConfigEnumeratorCallback(IFile file, List<String> result) {
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
            result.add(sectionName);
        }
    }

    protected static final String EMPTY_STRING = ""; //$NON-NLS-1$

	public SimulationTab() {
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#createControl(org.eclipse.swt.widgets.Composite)
	 */
	public void createControl(Composite parent) {
		Composite comp = new Composite(parent, SWT.NONE);
		setControl(comp);

		GridLayout topLayout = new GridLayout();
		comp.setLayout(topLayout);

		createIniGroup(comp, 1);
        createConfigGroup(comp, 1);
        createUIGroup(comp, 1);
	}

	protected void createIniGroup(Composite parent, int colSpan) {
		Composite iniComp = SWTFactory.createComposite(parent, 3, colSpan, GridData.FILL_HORIZONTAL);

		SWTFactory.createLabel(iniComp, "Initialization file:", 1);

		fInifileText = SWTFactory.createSingleText(iniComp, 1);
		fInifileText.setToolTipText("The INI file defining parameters and configuration blocks (default: omnetpp.ini)");
		fInifileText.addModifyListener(this);
		fInifileText.addFocusListener(new FocusAdapter() {
            @Override
            public void focusLost(FocusEvent e) {
                updateConfigCombo();
            }
		});

		fBrowseInifileButton = createPushButton(iniComp, "Browse...", null);
		fBrowseInifileButton.addSelectionListener(new SelectionAdapter() {
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

        fDefaultEnvButton = createRadioButton(runComp, "Default");
        fDefaultEnvButton.setSelection(true);
        fDefaultEnvButton.addSelectionListener(this);

        fCmdEnvButton = createRadioButton(runComp, "Command line");
        fCmdEnvButton.addSelectionListener(this);

        fTkEnvButton = createRadioButton(runComp, "Tcl/TK");
        fTkEnvButton.addSelectionListener(this);

        fOtherEnvButton = createRadioButton(runComp, "Other:");
        fOtherEnvButton.addSelectionListener(this);

        fOtherText = SWTFactory.createText(runComp, SWT.SINGLE | SWT.BORDER, 1, GridData.FILL_HORIZONTAL);
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
		updateInifileFromConfig(config);
		IFile inifile = getInifile(config);
		if (inifile != null)
		    fConfigCombo.setItems(getConfigNames(inifile));
		updateConfigFromConfig(config);
	}

    protected void updateConfigCombo() {
        IFile inifile = getInifile(config);
        fConfigCombo.setItems(new String[] {});
        if (inifile != null)
            fConfigCombo.setItems(getConfigNames(inifile));
    }

	protected void updateInifileFromConfig(ILaunchConfiguration config) {
        String inifileName = EMPTY_STRING;
        try {
            inifileName = config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, EMPTY_STRING);
        } catch (CoreException ce) {
            LaunchPluging.logError(ce);
        }
        fInifileText.setText(inifileName);
	}

	protected void updateConfigFromConfig(ILaunchConfiguration config) {
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#performApply(org.eclipse.debug.core.ILaunchConfigurationWorkingCopy)
	 */
	public void performApply(ILaunchConfigurationWorkingCopy config) {
        config.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, fInifileText.getText());
	}

	protected void handleBrowseInifileButtonSelected() {
        ElementTreeSelectionDialog dialog = new ElementTreeSelectionDialog(getShell(), new WorkbenchLabelProvider(), new WorkbenchContentProvider() {
            // filter out the non INI files
            @Override
            public Object[] getChildren(Object element) {
                List<Object> filteredChildren = new ArrayList<Object>();
                for(Object child : super.getChildren(element)) {
                    if (!(child instanceof IFile) || "ini".equals(((IFile)child).getFileExtension()))
                            filteredChildren.add(child);
                }
                return filteredChildren.toArray();
            }
        });
        dialog.setTitle("Select an ini file");
        dialog.setMessage("Select an initialization file for the simulation");
        dialog.setInput(getProject(config));
        dialog.setComparator(new ResourceComparator(ResourceComparator.NAME));
        if (dialog.open() == IDialogConstants.OK_ID) {
            IResource resource = (IResource) dialog.getFirstResult();
            String arg = resource.getFullPath().toString();
//            String fileLoc = VariablesPlugin.getDefault().getStringVariableManager().generateVariableExpression("workspace_loc", arg); //$NON-NLS-1$
            // FIXME strip the leading project name make it project relative
            fInifileText.setText(arg);
        }
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#isValid(org.eclipse.debug.core.ILaunchConfiguration)
	 */
	@Override
    public boolean isValid(ILaunchConfiguration config) {

		setErrorMessage(null);
		setMessage(null);

		IProject project = null;
		try {
            project = ResourcesPlugin.getWorkspace().getRoot().getProject(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, EMPTY_STRING));
        } catch (CoreException e) {}
        if (project == null || !project.isOpen()) {
            setErrorMessage("Project must be opened");
            return false;
        }

        String name = fInifileText.getText().trim();
        if (EMPTY_STRING.equals(name))
            return true;

        IPath iniPath = new Path(name);
        if (!iniPath.isAbsolute()) {
            if (!project.getFile(name).exists()) {
                setErrorMessage("Initialization file does not exist");
                return false;
            }
            iniPath = project.getFile(name).getLocation();
        } else {
            if (!iniPath.toFile().exists()) {
                setErrorMessage("Initialization file does not exist");
                return false;
            }
        }

		return true;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#setDefaults(org.eclipse.debug.core.ILaunchConfigurationWorkingCopy)
	 */
	public void setDefaults(ILaunchConfigurationWorkingCopy config) {
	}

	@Override
	public Image getImage() {
	    return LaunchPluging.getImage("/icons/full/ctool16/omnetsim.gif");
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see org.eclipse.debug.ui.ILaunchConfigurationTab#getName()
	 */
	public String getName() {
		return "Simulation";
	}

    public void widgetDefaultSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
    }

    public void widgetSelected(SelectionEvent e) {
        updateLaunchConfigurationDialog();
    }

    public void modifyText(ModifyEvent e) {
        updateLaunchConfigurationDialog();
    }

    /**
     * @return The selected project if exist and open otherwise NULL
     */
    private IProject getProject(ILaunchConfiguration config) {
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
     * @return The selected ini file (omnetpp.ini) if the inifile editor is empty
     */
    private IFile getInifile(ILaunchConfiguration config) {
        // FIXME take it from the config instead of the control
        // FIXME lookup the inifile relative to the working directory not to the project
        String name =  fInifileText.getText().trim();
        IProject project = getProject(config);
        if (project == null)
            return null;
        // default ini file is omnetpp ini
        if (EMPTY_STRING.equals(name))
            name = "omnetpp.ini";

        IPath iniPath = new Path(name);
        if (!iniPath.isAbsolute()) {
            if (!project.getFile(name).exists())
                return null;
            return project.getFile(name);
        } else {
            if (!iniPath.toFile().exists())
                return null;
            IFile[] ifiles = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(iniPath);
            if (ifiles.length == 1)
                return ifiles[0];
            return null;
        }
    }

    private String [] getConfigNames(IFile inifile) {
        List<String> result = new ArrayList<String>();
        result.add("");
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
        return result.toArray(new String[] {});
    }

}
