/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch.tabs;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchCommandConstants;
import org.eclipse.ui.dialogs.ContainerSelectionDialog;
import org.eclipse.ui.dialogs.ElementTreeSelectionDialog;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.eclipse.ui.views.navigator.ResourceComparator;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.ui.ToggleLink;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * A launch configuration tab that displays and edits OMNeT++ launch configuration
 *
 * @author rhornig, andras
 */
public class OmnetppMainTab extends AbstractLaunchConfigurationTab {

    protected static final int MAX_TOOLTIP_CHARS = 50000;

    // UI widgets
    protected Button fProgOppRunButton;
    protected Button fProgOtherButton;
    protected Text fProgText;

    // working dir
    private Button fWorkspaceButton;
    private Text workingDirText = null;

    // configuration
    protected Text fInifileText;
    protected Combo fConfigCombo;
    protected Text fRunText;
    protected Text fNedPathText;
    protected Text fImagePathText;
    protected Button fBatchingCheckbox;
    protected Spinner fParallelismSpinner;
    protected Spinner fBatchSizeSpinner;
    protected Combo fEnvirCombo;
    protected Text fSimTimeLimitText;
    protected Text fCpuTimeLimitText;
    protected Text fLibraryText;
    protected Text fAdditionalText;

    protected Button fDebugOnErrorCheckbox;

    protected Button fRedirectStdoutCheckbox;
    protected Button fRecordEventlogCheckbox;
    protected Button fRecordScalarsCheckbox;
    protected Button fRecordVectorsCheckbox;
    protected Button fCmdenvExpressModeCheckbox;
    protected Button fVerboseCheckbox;
    protected Button fStopBatchOnErrorCheckbox;

    // build before launch
    protected Combo fBuildBeforeLaunchCombo;
    protected Combo fSwitchBeforeBuildCombo;

    private ILaunchConfiguration config;
    private boolean updateDialogStateInProgress = false;
    private boolean isDebugLaunch = false;
    private String infoText = null;
    private Button fBrowseForBinaryButton;

    private static final String CMDENV = "Cmdenv", QTENV = "Qtenv", TKENV = "Tkenv";

    private SelectionAdapter defaultSelectionAdapter = new SelectionAdapter() {
        @Override
        public void widgetSelected(SelectionEvent e) {
            updateDialogState();
        }
    };

    private ModifyListener modifyListener = new ModifyListener() {
        @Override
        public void modifyText(ModifyEvent e) {
            infoText = null; // invalidate
            updateDialogState();
        }
    };

    public static class ConfigOptionsContentProposalProvider extends ContentProposalProvider {
        protected List<IContentProposal> proposals = null;

        public ConfigOptionsContentProposalProvider() {
            super(false, true);
        }

        @Override
        protected List<IContentProposal> getProposalCandidates(String prefix) {
            if (proposals == null) {
                proposals = new ArrayList<>();
                for (ConfigOption option : ConfigRegistry.getOptions())
                    proposals.add(new ContentProposal("--" + option.getName()+"=", "--" + option.getName()+"=", getDescription(option)));
                for (ConfigOption option : ConfigRegistry.getPerObjectOptions())
                    proposals.add(new ContentProposal("--" + option.getName()+"=", "--" + option.getName()+"= (*)", getDescription(option)));
                proposals.sort(null);
            }
            return proposals;
        }

        @Override
        protected String getCompletionPrefix(String text) {
            return text.replaceFirst("^.* ", "");  // keep only the last argument
        }

        protected static String getDescription(ConfigOption option) {
            String text = "--" + option.getName() + " = <" + option.getDataType().name().replaceFirst("CFG_", "") + ">";
            if (option.getDefaultValue() != null && !option.getDefaultValue().equals(""))
                text += ", default: " + option.getDefaultValue();
            if (option.getUnit() != null)
                text += ", unit: "+option.getUnit();
            text += "\n\n";
            text += option.getDescription();
            return text;
        }

    }

    public OmnetppMainTab() {
        super();
    }

    @Override
    public void createControl(Composite parent) {
        isDebugLaunch = getLaunchConfigurationDialog().getMode().equals(ILaunchManager.DEBUG_MODE);

        final ScrolledComposite scolledComposite = new ScrolledComposite( parent, SWT.V_SCROLL | SWT.H_SCROLL );
        scolledComposite.setExpandHorizontal(true);
        scolledComposite.setExpandVertical(true);

        final Composite composite = SWTFactory.createComposite(scolledComposite, 1, 1, GridData.FILL_HORIZONTAL);
        createSimulationGroup(composite, 1);
        createExecutionGroup(composite, 1);
        createOptionsGroup(composite, 1);

        Composite advancedGroup = createAdvancedGroup(composite, 1);
        Composite buildGroup = createBuildGroup(composite, 1);
        ToggleLink more = new ToggleLink(composite, SWT.NONE);
        more.setControls(new Control[] { advancedGroup, buildGroup });

        more.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                scolledComposite.setMinSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
            }
        });

        scolledComposite.setMinSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        scolledComposite.setContent(composite);
        setControl(scolledComposite);

        addModifyListeners(composite);
    }

    protected Composite createSimulationGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Simulation", 3, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(composite, "Executable:",1);
        Composite innerComposite = SWTFactory.createComposite(composite, 3, 1, GridData.FILL_HORIZONTAL);
        GridLayout innerLd = (GridLayout)innerComposite.getLayout();
        innerLd.marginHeight = 0;
        innerLd.marginWidth = 0;

        fProgOppRunButton = createRadioButton(innerComposite, "opp_run");
        fProgOppRunButton.setSelection(true);
        fProgOtherButton = createRadioButton(innerComposite, "Other:");
        fProgText = SWTFactory.createSingleText(innerComposite, 1);

        fBrowseForBinaryButton = SWTFactory.createPushButton(composite, "Browse...", null);
        fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBinaryBrowseButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Working dir:", 1);
        workingDirText = SWTFactory.createSingleText(composite, 1);
        workingDirText.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                updateMacros();
            }
        });
        fWorkspaceButton = createPushButton(composite, "Browse...", null);
        fWorkspaceButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                handleWorkingDirBrowseButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Ini file(s):", 1);
        fInifileText = SWTFactory.createSingleText(composite, 1);
        fInifileText.setToolTipText("Ini file (or files), relative to the working directory");

        Button browseInifileButton = SWTFactory.createPushButton(composite, "Browse...", null);
        browseInifileButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseInifileButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Config name:",1);
        fConfigCombo = SWTFactory.createCombo(composite, SWT.BORDER | SWT.READ_ONLY, 2, new String[] {});
        fConfigCombo.setToolTipText("Name of a configuration section in the ini file");
        fConfigCombo.setVisibleItemCount(10);

        SWTFactory.createLabel(composite, "Run(s):", 1);
        fRunText = SWTFactory.createText(composite, SWT.SINGLE | SWT.BORDER | SWT.SEARCH, 2);
        fRunText.setMessage("Enter filter expression; hover to get list of matching runs");

        String runTooltip = "Filter expression or list of run numbers (e.g. \"$numHosts>10\", \"1,5,8..13\")";

        HoverSupport hover = new HoverSupport();
        hover.adapt(fRunText, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                if (infoText == null)
                    infoText = truncateHoverText(OmnetppLaunchUtils.getSimulationRunInfo(config, fRunText.getText()), MAX_TOOLTIP_CHARS);
                return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(runTooltip + "<p/>" + infoText));
            }
        });

        return composite;
    }

    protected String truncateHoverText(String text, int limit) {
        if (text.length() > limit) {
            int truncatePos = text.indexOf('\n', limit); // try to keep a whole last line
            if (truncatePos == -1 || truncatePos > 1.2*limit)
                truncatePos = limit;
            text = text.substring(0, truncatePos) + "\n...\n";
        }
        return text;
    }

    protected void createExecutionGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Execution", 2, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(composite, "User interface:", 1);
        fEnvirCombo = SWTFactory.createCombo(composite, SWT.BORDER, 1, GridData.FILL_HORIZONTAL, new String[] {QTENV, TKENV, CMDENV});
        fEnvirCombo.setToolTipText("User interface for the simulation. Leave empty to use ini file setting or the default.");
        fEnvirCombo.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                if (fBatchingCheckbox != null) {
                    fBatchingCheckbox.setSelection(fEnvirCombo.getText().trim().equals(CMDENV));
                    updateDialogState();  // must be AFTER the previous line
                }
            }
        });

        if (!isDebugLaunch) { // launching multiple processes is not desirable under CDT
            fBatchingCheckbox = SWTFactory.createCheckButton(composite, "Allow multiple processes", null, false, 2);
            fBatchingCheckbox.setToolTipText("Execute simulation runs in batches, each batch in a separate process. Useful with Cmdenv.");

            Composite group = SWTFactory.setMargin(SWTFactory.createComposite(composite, 4, 2, GridData.FILL_HORIZONTAL), 0);

            SWTFactory.setIndent(SWTFactory.createLabel(group, "Number of CPUs to use:", 1), 20);
            fParallelismSpinner = new Spinner(group, SWT.BORDER);
            fParallelismSpinner.setToolTipText("Number of simulation processes that are able to run in parallel");
            fParallelismSpinner.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
            fParallelismSpinner.setMinimum(1);
            setSpinnerWidthHint(fParallelismSpinner);

            SWTFactory.setIndent(SWTFactory.createLabel(group, "Runs per process:", 1), 20);
            fBatchSizeSpinner = new Spinner(group, SWT.BORDER);
            fBatchSizeSpinner.setToolTipText("Number of simulation runs to be assigned to a single simulation process (Cmdenv instance)");
            fBatchSizeSpinner.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
            fBatchSizeSpinner.setMinimum(1);
            setSpinnerWidthHint(fBatchSizeSpinner);
        }

        if (isDebugLaunch) {
            fDebugOnErrorCheckbox = SWTFactory.createTristateCheckButton(composite, "Debug on errors", null, false, false, 2);
            fDebugOnErrorCheckbox.setToolTipText("Trigger debugger interrupt when simulation encounters runtime error. Overrides similar ini file setting.");
        }

        SWTFactory.createLabel(composite, "Simulation time limit:", 1);
        fSimTimeLimitText = SWTFactory.createSingleText(composite, 1);
        fSimTimeLimitText.setToolTipText("Simulation time to stop the simulation at. Overrides similar ini file setting.");

        SWTFactory.createLabel(composite, "CPU time limit:", 1);
        fCpuTimeLimitText = SWTFactory.createSingleText(composite, 1);
        fCpuTimeLimitText.setToolTipText("Maximum CPU time allowed for the simulation. Overrides similar ini file setting.");
    }

    protected static void setSpinnerWidthHint(Spinner spinner) {
        Point size = spinner.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        ((GridData)spinner.getLayoutData()).widthHint = size.x + 16; // TODO needed on some platforms
    }

    protected void createOptionsGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Output", 4, colSpan, GridData.FILL_HORIZONTAL);
        SWTFactory.setEqualColumnWidth(composite, false);

        fVerboseCheckbox = SWTFactory.createCheckButton(composite, "Verbose", null, false, 1);
        fStopBatchOnErrorCheckbox = SWTFactory.createTristateCheckButton(composite, "Stop batch on error", null, false, false, 1);
        fCmdenvExpressModeCheckbox = SWTFactory.createTristateCheckButton(composite, "Express mode", null, false, false, 1);
        fRedirectStdoutCheckbox = SWTFactory.createTristateCheckButton(composite, "Save stdout", null, false, false, 1);
        fRecordScalarsCheckbox = SWTFactory.createTristateCheckButton(composite, "Record scalar results", null, false, false, 1);
        fRecordVectorsCheckbox = SWTFactory.createTristateCheckButton(composite, "Record vector results", null, false, false, 1);
        fRecordEventlogCheckbox = SWTFactory.createTristateCheckButton(composite, "Record eventlog", null, false, false, 1);

        fVerboseCheckbox.setToolTipText("Verbose output");
        fStopBatchOnErrorCheckbox.setToolTipText("Skip further runs when a run stops with a simulation error. Overrides similar ini file setting");
        fCmdenvExpressModeCheckbox.setToolTipText("Run in Express mode (Cmdenv-only.) Overrides similar ini file setting");
        fRedirectStdoutCheckbox.setToolTipText("Save stdout to per-run files in the results folder (Cmdenv-only.) Overrides similar ini file setting");
        fRecordScalarsCheckbox.setToolTipText("Allow creating an output scalar file. Overrides similar ini file setting");
        fRecordVectorsCheckbox.setToolTipText("Allow creating an output vector file. Overrides similar ini file setting");
        fRecordEventlogCheckbox.setToolTipText("Record eventlog for Sequence Chart tool. Overrides similar ini file setting");
    }

    protected Composite createAdvancedGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Advanced", 3, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(composite, "Dynamic libraries:", 1);
        fLibraryText = SWTFactory.createSingleText(composite, 1);
        fLibraryText.setToolTipText("DLLs or shared libraries to load (without extension, relative to the working directory. Use ${opp_shared_libs:/workingdir} for automatic setting.)");

        Button browseLibrariesButton = SWTFactory.createPushButton(composite, "Browse...", null);
        browseLibrariesButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseLibrariesButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "NED Source Path:", 1);
        fNedPathText = SWTFactory.createSingleText(composite, 2);
        fNedPathText.setToolTipText("Directories where NED files are read from (relative to the first selected ini file). " +
        "Use ${opp_ned_path:/workingdir} for automatic setting.");

        SWTFactory.createLabel(composite, "Image Path:", 1);
        fImagePathText = SWTFactory.createSingleText(composite, 2);
        fImagePathText.setToolTipText("Directories where image files are read from (relative to the first selected ini file). Use ${opp_image_path:/workingdir} for automatic setting.");

        SWTFactory.createLabel(composite, "Additional arguments:", 1);
        fAdditionalText = SWTFactory.createSingleText(composite, 2);
        fAdditionalText.setToolTipText("Specify additional command line arguments");
        new ContentAssistCommandAdapter(fAdditionalText, new TextContentAdapter(),
                new ConfigOptionsContentProposalProvider(),
                IWorkbenchCommandConstants.EDIT_CONTENT_ASSIST, "-".toCharArray(), true);

        return composite;
    }

    protected Composite createBuildGroup(Composite parent, int colSpan) {
        Composite composite = SWTFactory.createGroup(parent, "Build before launch", 3, colSpan, GridData.FILL_HORIZONTAL);

        SWTFactory.createLabel(composite, "Projects to build:",1);
        fBuildBeforeLaunchCombo = SWTFactory.createCombo(composite, SWT.BORDER | SWT.READ_ONLY, 2, new String[] {"This project and all its dependencies", "This project only", "Do not build automatically before launch"});
        fBuildBeforeLaunchCombo.setToolTipText("Which projects to build automatically before launching");
        fBuildBeforeLaunchCombo.setVisibleItemCount(3);

        SWTFactory.createLabel(composite, "Configuration to build:",1);
        fSwitchBeforeBuildCombo = SWTFactory.createCombo(composite, SWT.BORDER | SWT.READ_ONLY, 2, new String[] {"Ask whether to switch the configuration", "Switch automatically to the proper configuration", "Always use the active configuration"});
        fSwitchBeforeBuildCombo.setToolTipText("Whether to switch the active configuration before building the project");
        fSwitchBeforeBuildCombo.setVisibleItemCount(3);

        return composite;
    }

    protected void addModifyListeners(Control control) {
        if (control instanceof Text)
            ((Text)control).addModifyListener(modifyListener);
        else if (control instanceof Button)
            ((Button)control).addSelectionListener(defaultSelectionAdapter);
        else if (control instanceof Combo) { // note: this is a Composite!
            ((Combo)control).addSelectionListener(defaultSelectionAdapter);
            ((Combo)control).addModifyListener(modifyListener);
        }
        else if (control instanceof Spinner) { // note: this is a Composite!
            ((Spinner)control).addSelectionListener(defaultSelectionAdapter);
            ((Spinner)control).addModifyListener(modifyListener);
        } else if (control instanceof Composite) {
            for (Control child : ((Composite)control).getChildren())
                addModifyListeners(child);
        }
    }

    @Override
    protected Button createRadioButton(Composite comp, String label) {
        Button button = SWTFactory.createRadioButton(comp, label);
        button.setLayoutData(new GridData());
        return button;
    }

    @Override
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

            updateDialogStateInProgress = true;  // disable the change notification until we finish filling all controls and we are in consistent state
                                                 // disable only AFTER setting the INI file so the configuration selection combo is filled out automatically

            setConfigName(config.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").trim());

            fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_RUNFILTER, ""));

            fEnvirCombo.setText(envirToComboString(config.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, QTENV)));

            if (fBatchingCheckbox != null) {
                fBatchingCheckbox.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_USE_BATCHING, false));
                fParallelismSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 2));
                fBatchSizeSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_BATCH_SIZE, 5));
            }

            if (fDebugOnErrorCheckbox != null)
                setTristateCheckbox(fDebugOnErrorCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_DEBUGMODE_DEBUG_ON_ERRORS, "true"));

            fSimTimeLimitText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_SIM_TIME_LIMIT, "").trim());
            fCpuTimeLimitText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_CPU_TIME_LIMIT, "").trim());

            setTristateCheckbox(fStopBatchOnErrorCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_STOP_BATCH_ON_ERROR, ""));
            setTristateCheckbox(fRedirectStdoutCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_CMDENV_REDIRECT_STDOUT, ""));
            setTristateCheckbox(fRecordEventlogCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, ""));
            setTristateCheckbox(fRecordScalarsCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_SCALARS, ""));
            setTristateCheckbox(fRecordVectorsCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_VECTORS, ""));
            setTristateCheckbox(fCmdenvExpressModeCheckbox, config.getAttribute(IOmnetppLaunchConstants.OPP_CMDENV_EXPRESS_MODE, ""));
            fVerboseCheckbox.setSelection(!config.getAttribute(IOmnetppLaunchConstants.OPP_SILENT, false));

            fLibraryText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim());
            fNedPathText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim());
            fImagePathText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, "").trim());
            fAdditionalText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "").trim());
            int buildBeforeValue = config.getAttribute(IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH, IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH_DEPENDENCIES);
            int switchBeforeBuildValue = config.getAttribute(IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD, IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_ASK);
            fBuildBeforeLaunchCombo.setText(fBuildBeforeLaunchCombo.getItem(buildBeforeValue));
            fSwitchBeforeBuildCombo.setText(fSwitchBeforeBuildCombo.getItem(switchBeforeBuildValue));

            // bring dialog to consistent state
            updateDialogStateInProgress = false;
            updateDialogState();

        } catch (CoreException ce) {
            LaunchPlugin.logError(ce);
        }
    }

    private static void setTristateCheckbox(Button c, String value) {
        if (value == null || value.isEmpty()) {
            c.setGrayed(true);
        }
        else {
            Assert.isTrue(value.equals("true") || value.equals("false"));
            c.setGrayed(false);
            c.setSelection(value.equals("true"));
        }
    }

    private static String getTristateCheckboxValue(Button c) {
        return c.getGrayed() ? "" : c.getSelection() ? "true" : "false";
    }

    @Override
    public void performApply(ILaunchConfigurationWorkingCopy configuration) {
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, getWorkingDirectoryText());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, fProgText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, fInifileText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, getConfigName());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_RUNFILTER, fRunText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, envirFromComboString(fEnvirCombo.getText().trim()));

        if (fBatchingCheckbox != null) {
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USE_BATCHING, fBatchingCheckbox.getSelection());
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, fParallelismSpinner.getSelection());
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_BATCH_SIZE, fBatchSizeSpinner.getSelection());
        }

        if (fDebugOnErrorCheckbox != null)
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUGMODE_DEBUG_ON_ERRORS, getTristateCheckboxValue(fDebugOnErrorCheckbox));

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SIM_TIME_LIMIT, fSimTimeLimitText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_CPU_TIME_LIMIT, fCpuTimeLimitText.getText().trim());

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_STOP_BATCH_ON_ERROR, getTristateCheckboxValue(fStopBatchOnErrorCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_CMDENV_REDIRECT_STDOUT, getTristateCheckboxValue(fRedirectStdoutCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, getTristateCheckboxValue(fRecordEventlogCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_SCALARS, getTristateCheckboxValue(fRecordScalarsCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_VECTORS, getTristateCheckboxValue(fRecordVectorsCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_CMDENV_EXPRESS_MODE, getTristateCheckboxValue(fCmdenvExpressModeCheckbox));
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SILENT, !fVerboseCheckbox.getSelection());

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, fLibraryText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, fNedPathText.getText().trim());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, fImagePathText.getText().trim());

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, fAdditionalText.getText().trim());

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH, fBuildBeforeLaunchCombo.getSelectionIndex());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD, fSwitchBeforeBuildCombo.getSelectionIndex());

        try {
            Set<IResource> assocRes = new HashSet<>();
            if (configuration.getMappedResources() != null)
                assocRes.addAll(Arrays.asList(configuration.getMappedResources()));
            if (getIniFiles() != null)
                assocRes.addAll(Arrays.asList(getIniFiles()));
            configuration.setMappedResources(assocRes.toArray(new IResource[assocRes.size()]));
        } catch (CoreException e) {
            LaunchPlugin.logError(e);
        }

        // clear the run info text, so next time it will be re-requested
        infoText = null;
    }

    @Override
    public void setDefaults(ILaunchConfigurationWorkingCopy configuration) {
        IResource selectedResource = DebugUITools.getSelectedResource();
        OmnetppLaunchUtils.setLaunchConfigDefaults(configuration, selectedResource);
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

        if (fBatchingCheckbox != null) {
            boolean useBatching = fBatchingCheckbox.getSelection();
            fParallelismSpinner.setEnabled(useBatching);
            fBatchSizeSpinner.setEnabled(useBatching);
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
    private IContainer getWorkingDirectory() {
        String expandedPath;
        try {
            expandedPath = StringUtils.substituteVariables(getWorkingDirectoryText());
        } catch (CoreException e) {
            return null;
        }
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
        List<IFile> result = new ArrayList<>();
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
     * Returns all the configuration names found in the supplied inifiles, together
     * with their descriptions (extends, network, description, etc.)
     */
    private String[] getConfigNames(IFile[] inifiles) {
        Map<String,OmnetppLaunchUtils.ConfigEnumeratorCallback.Section> sections = new LinkedHashMap<>();
        if (inifiles != null)
            for (IFile inifile : inifiles) {
                InifileParser iparser = new InifileParser();
                try {
                    iparser.parse(inifile, new OmnetppLaunchUtils.ConfigEnumeratorCallback(inifile, sections));
                } catch (CoreException e) {
                    setErrorMessage("Error reading inifile: " + e.getMessage());
                }
            }
        List<String> result = new ArrayList<>();
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

    private void setConfigName(String name) {
        fConfigCombo.setText("");
        for (String line : fConfigCombo.getItems()) {
            if (line.equals(name) || line.startsWith(name+" ")) {
                fConfigCombo.setText(line);
                return;
            }
        }
    }

    private static String envirToComboString(String value) {
        if (value == null || value.equals("") || value.equals(IOmnetppLaunchConstants.UI_DEFAULTEXTERNAL)) return "";
        return value;
    }

    private static String envirFromComboString(String value) {
        return value;
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
            setErrorMessage("Invalid working directory");
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

        String simTimeLimit = fSimTimeLimitText.getText();
        if (!StringUtils.isBlank(simTimeLimit)) {
            try {
                double d = UnitConversion.parseQuantity(simTimeLimit, "s");
                if (d < 0) {
                    setErrorMessage("Simulation time limit cannot be negative");
                    return false;
                }
            } catch (Exception e) {
                setErrorMessage("Wrong simulation time limit (missing measurement unit?)");
                return false;
            }
        }

        String cpuTimeLimit = fCpuTimeLimitText.getText();
        if (!StringUtils.isBlank(cpuTimeLimit)) {
            try {
                double d = UnitConversion.parseQuantity(cpuTimeLimit, "s");
                if (d < 0) {
                    setErrorMessage("CPU time limit cannot be negative");
                    return false;
                }
            } catch (Exception e) {
                setErrorMessage("Wrong CPU time limit (missing measurement unit?)");
                return false;
            }
        }

        return super.isValid(configuration);
    }

    @Override
    public Image getImage() {
        return LaunchPlugin.getImage("/icons/full/ctool16/omnetsim.gif");
    }

    @Override
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

    /**
     * Show a dialog that lets the user select a working directory from
     * the workspace
     */
    protected void handleWorkingDirBrowseButtonSelected() {
        IContainer currentContainer= getWorkingDirectory();
        if (currentContainer == null)
            currentContainer = ResourcesPlugin.getWorkspace().getRoot();
        ContainerSelectionDialog dialog = new ContainerSelectionDialog(getShell(), currentContainer, false, "Select a workspace-relative working directory:");
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

    protected void updateMacros() {
        String workingDir = getWorkingDirectoryText();
        try {
            workingDir = StringUtils.substituteVariables(workingDir);
        } catch (CoreException e) {
            // leave unresolved
        }
        workingDir = workingDir.replace("}", "?");  // close braces cause weird interference with the fields below (NED path, Shared libs)

        String libText = fLibraryText.getText();
        libText = libText.replaceAll("\\$\\{"+OmnetppLaunchUtils.VAR_SHARED_LIBS+":.*?\\}", Matcher.quoteReplacement("${"+OmnetppLaunchUtils.VAR_SHARED_LIBS+":"+workingDir+"}"));
        fLibraryText.setText(libText);

        String nedText = fNedPathText.getText();
        nedText = nedText.replaceAll("\\$\\{"+OmnetppLaunchUtils.VAR_NED_PATH+":.*?\\}", Matcher.quoteReplacement("${"+OmnetppLaunchUtils.VAR_NED_PATH+":"+workingDir+"}"));
        fNedPathText.setText(nedText);

        String imageText = fImagePathText.getText();
        imageText = imageText.replaceAll("\\$\\{"+OmnetppLaunchUtils.VAR_IMAGE_PATH+":.*?\\}", Matcher.quoteReplacement("${"+OmnetppLaunchUtils.VAR_IMAGE_PATH+":"+workingDir+"}"));
        fImagePathText.setText(imageText);
    }

}
