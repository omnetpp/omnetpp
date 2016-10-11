/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch.tabs;

import java.io.IOException;
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
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
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
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
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
public class OmnetppMainTab extends AbstractLaunchConfigurationTab {

    protected static final int MAX_TOOLTIP_CHARS = 50000;

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
    protected Text fImagePathText;
    protected Spinner fParallelismSpinner;
    protected Button fDefaultExternalEnvButton;
    protected Button fCmdenvButton;
    protected Button fTkenvButton;
    protected Button fQtenvButton;
    protected Button fOtherEnvButton;
    protected Text fOtherEnvText;
    protected Text fLibraryText;
    protected Text fAdditionalText;

    protected Button fEventLogDefaultButton;
    protected Button fEventLogYesButton;
    protected Button fEventLogNoButton;

    protected Button fDebugOnErrorDefaultButton;
    protected Button fDebugOnErrorYesButton;
    protected Button fDebugOnErrorNoButton;
    protected Button fDebugOnErrorAutoButton;

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

    private ModifyListener modifyListener = new ModifyListener() {
        @Override
        public void modifyText(ModifyEvent e) {
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
        final ScrolledComposite scolledComposite = new ScrolledComposite( parent, SWT.V_SCROLL | SWT.H_SCROLL );
        scolledComposite.setExpandHorizontal(true);
        scolledComposite.setExpandVertical(true);

        final Composite composite = SWTFactory.createComposite(scolledComposite, 1, 1, GridData.FILL_HORIZONTAL);
        createWorkingDirGroup(composite, 1);
        createSimulationGroup(composite, 1);
        createOptionsGroup(composite, 1);

        Composite advancedGroup = createAdvancedGroup(composite, 1);
        ToggleLink more = new ToggleLink(composite, SWT.NONE);
        more.setControls(new Control[] { advancedGroup });

        more.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                scolledComposite.setMinSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
            }
        });

        scolledComposite.setMinSize(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        scolledComposite.setContent(composite);
        setControl(scolledComposite);
    }

    protected Group createWorkingDirGroup(Composite parent, int colSpan) {
        Group group = SWTFactory.createGroup(parent, "Working directory", 3, colSpan, GridData.FILL_HORIZONTAL);
        GridLayout innerLd = (GridLayout)group.getLayout();
        innerLd.marginWidth = 0;

        setControl(group);
        workingDirText = SWTFactory.createSingleText(group, 1);
        workingDirText.addModifyListener(modifyListener);
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
        fProgText.addModifyListener(modifyListener);

        fBrowseForBinaryButton = SWTFactory.createPushButton(composite, "Browse...", null);
        fBrowseForBinaryButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBinaryBrowseButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Ini file(s):", 1);

        fInifileText = SWTFactory.createSingleText(composite, 2);
        fInifileText.setToolTipText("Ini file (or files), relative to the working directory");
        fInifileText.addModifyListener(modifyListener);

        Button browseInifileButton = SWTFactory.createPushButton(composite, "Browse...", null);
        browseInifileButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent evt) {
                handleBrowseInifileButtonSelected();
            }
        });

        SWTFactory.createLabel(composite, "Config name:",1);

        fConfigCombo = SWTFactory.createCombo(composite, SWT.BORDER | SWT.READ_ONLY, 3, new String[] {});
        fConfigCombo.setToolTipText("The configuration from the ini file");
        fConfigCombo.setVisibleItemCount(10);
        fConfigCombo.addModifyListener(modifyListener);

        SWTFactory.createLabel(composite, "Run number:",1);

        int runSpan = debugLaunchMode ? 3 : 1;
        fRunText = SWTFactory.createSingleText(composite, runSpan);
        fRunText.addModifyListener(modifyListener);
        HoverSupport hover = new HoverSupport();
        hover.adapt(fRunText, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                if (infoText == null)
                    infoText = truncateHoverText(OmnetppLaunchUtils.getSimulationRunInfo(config), MAX_TOOLTIP_CHARS);
                return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(runTooltip+"<pre>"+infoText+"</pre>"));
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

    protected String truncateHoverText(String text, int limit) {
        if (text.length() > limit) {
            int truncatePos = text.indexOf('\n', limit); // try to keep a whole last line
            if (truncatePos == -1 || truncatePos > 1.2*limit)
                truncatePos = limit;
            text = text.substring(0, truncatePos) + "\n...\n";
        }
        return text;
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
        fLibraryText.addModifyListener(modifyListener);

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
        fNedPathText.addModifyListener(modifyListener);

        SWTFactory.createLabel(composite, "Image Path:", 1);
        fImagePathText = SWTFactory.createSingleText(composite, 2);
        fImagePathText.setToolTipText("Directories where image files are read from (relative to the first selected ini file). " +
        "Use ${opp_image_path:/workingdir} for automatic setting.");
        fImagePathText.addModifyListener(modifyListener);

        SWTFactory.createLabel(composite, "Additional arguments:", 1);
        fAdditionalText = SWTFactory.createSingleText(composite, 2);
        fAdditionalText.setToolTipText("Specify additional command line arguments");
        fAdditionalText.addModifyListener(modifyListener);

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
        Composite comp = SWTFactory.createComposite(parent, 7, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "User interface:", 1);
        fDefaultExternalEnvButton = createRadioButton(comp, "Default");
        fCmdenvButton = createRadioButton(comp, "Cmdenv");
        fTkenvButton = createRadioButton(comp, "Tkenv");
        fQtenvButton = createRadioButton(comp, "Qtenv");
        fOtherEnvButton = createRadioButton(comp, "Other:");
        fOtherEnvText = SWTFactory.createSingleText(comp, 1);
        fOtherEnvText.addModifyListener(modifyListener);

        fDefaultExternalEnvButton.setToolTipText("Let the ini file setting or the default take effect");
        fCmdenvButton.setToolTipText("Launch the simulation with the -u Cmdenv option");
        fTkenvButton.setToolTipText("Launch the simulation with the -u Tkenv option");
        fQtenvButton.setToolTipText("Launch the simulation with the -u Qtenv option");
        fOtherEnvButton.setToolTipText("Launch the simulation with the -u <custom> option");
    }

    protected void createRecordEventlogRadioButtons(Composite parent, int colSpan) {
        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "Record eventlog:", 1);

        fEventLogDefaultButton = createRadioButton(comp, "Default");
        fEventLogYesButton = createRadioButton(comp, "Yes");
        fEventLogNoButton = createRadioButton(comp, "No");

        fEventLogDefaultButton.setToolTipText("Let the ini file setting take effect");
        fEventLogYesButton.setToolTipText("Override ini file setting");
        fEventLogNoButton.setToolTipText("Override ini file setting");
    }

    protected void createDbgOnErrRadioButtons(Composite parent, int colSpan) {

        Composite comp = SWTFactory.createComposite(parent, 6, colSpan, GridData.FILL_HORIZONTAL);
        ((GridLayout)comp.getLayout()).marginWidth = 0;
        ((GridLayout)comp.getLayout()).marginHeight = 0;

        SWTFactory.createLabel(comp, "Debug on errors:", 1);

        fDebugOnErrorDefaultButton = createRadioButton(comp, "Default");
        fDebugOnErrorYesButton = createRadioButton(comp, "Yes");
        fDebugOnErrorNoButton = createRadioButton(comp, "No");
        fDebugOnErrorAutoButton = createRadioButton(comp, "Auto");

        fDebugOnErrorDefaultButton.setToolTipText("Let the ini file setting take effect");
        fDebugOnErrorYesButton.setToolTipText("Override ini file setting");
        fDebugOnErrorNoButton.setToolTipText("Override ini file setting");
        fDebugOnErrorAutoButton.setToolTipText("Only for Debug launches");
    }

    protected Button createRadioButton(Composite comp, String label) {
        Button button = SWTFactory.createRadioButton(comp, label);
        button.setLayoutData(new GridData());
        button.addSelectionListener(defaultSelectionAdapter);
        return button;
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

            updateDialogStateInProgress = true;  // disable the change notification until we finish filling all controls and we are in consistent state
                                                 // disable only AFTER setting the INI file so the configuration selection combo is filled out automatically

            setConfigName(config.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").trim());

            if (debugLaunchMode)
                fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, ""));
            else
                fRunText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, ""));

            if (fParallelismSpinner != null)
                fParallelismSpinner.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1));

            // update UI radio buttons
            String uiArg = StringUtils.defaultIfEmpty(config.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "").trim(), IOmnetppLaunchConstants.UI_FALLBACKVALUE);
            fDefaultExternalEnvButton.setSelection(uiArg.equals(IOmnetppLaunchConstants.UI_DEFAULTEXTERNAL));
            fCmdenvButton.setSelection(uiArg.equals(IOmnetppLaunchConstants.UI_CMDENV));
            fTkenvButton.setSelection(uiArg.equals(IOmnetppLaunchConstants.UI_TKENV));
            fQtenvButton.setSelection(uiArg.equals(IOmnetppLaunchConstants.UI_QTENV));
            boolean isOther = !fDefaultExternalEnvButton.getSelection() && !fCmdenvButton.getSelection() && !fTkenvButton.getSelection() && !fQtenvButton.getSelection();
            fOtherEnvButton.setSelection(isOther);
            fOtherEnvText.setText(isOther ? uiArg.trim() : "");

            // update eventlog radio buttons  (anything that's not "false" will count as "true")
            String recordEventlogArg = config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "");
            fEventLogDefaultButton.setSelection(recordEventlogArg.equals(""));
            fEventLogNoButton.setSelection(recordEventlogArg.equals("false"));
            fEventLogYesButton.setSelection(recordEventlogArg.equals("true"));
            if (!fEventLogDefaultButton.getSelection() && !fEventLogNoButton.getSelection() && !fEventLogYesButton.getSelection())
                fEventLogDefaultButton.setSelection(true);

            // update debug on error radio buttons
            String dbgOnErrArg = config.getAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "auto");
            fDebugOnErrorDefaultButton.setSelection(dbgOnErrArg.equals(""));
            fDebugOnErrorNoButton.setSelection(dbgOnErrArg.equals("false"));
            fDebugOnErrorYesButton.setSelection(dbgOnErrArg.equals("true"));
            fDebugOnErrorAutoButton.setSelection(dbgOnErrArg.equals("auto"));
            if (!fDebugOnErrorDefaultButton.getSelection() && !fDebugOnErrorNoButton.getSelection() && !fDebugOnErrorYesButton.getSelection() && !fDebugOnErrorAutoButton.getSelection())
                fDebugOnErrorAutoButton.setSelection(true);

            fLibraryText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim());
            fNedPathText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim());
            fImagePathText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, "").trim());
            fAdditionalText.setText(config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "").trim());
            fShowDebugViewButton.setSelection(config.getAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, false));

            // bring dialog to consistent state
            updateDialogStateInProgress = false;
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
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, fImagePathText.getText());

        // if we are in debug mode, we should store the run parameter into the command line too
        String strippedRun = StringUtils.deleteWhitespace(fRunText.getText());
        if (debugLaunchMode)
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, strippedRun);
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, strippedRun);

        if (fDefaultExternalEnvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, IOmnetppLaunchConstants.UI_DEFAULTEXTERNAL);
        else if (fCmdenvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, IOmnetppLaunchConstants.UI_CMDENV);
        else if (fTkenvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, IOmnetppLaunchConstants.UI_TKENV);
        else if (fQtenvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, IOmnetppLaunchConstants.UI_QTENV);
        else if (fOtherEnvButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, fOtherEnvText.getText());
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "");

        // this MUST be filled only after the environment buttons are set otherwise
        // the UpdateDialogState method will set the value to 1 (the default if Tkenv is used)
        if (fParallelismSpinner != null)
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, fParallelismSpinner.getSelection());

        if (fEventLogYesButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "true");
        else if (fEventLogNoButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "false");
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "");

        if (fDebugOnErrorYesButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "true");
        else if (fDebugOnErrorNoButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "false");
        else if (fDebugOnErrorAutoButton.getSelection())
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "auto");
        else
            configuration.setAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "");

        configuration.setAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, fAdditionalText.getText());
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, fShowDebugViewButton.getSelection());

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
     * Returns all the configuration names found in the supplied inifiles
     */
    private String[] getConfigNames(IFile[] inifiles) {
        Map<String,OmnetppLaunchUtils.ConfigEnumeratorCallback.Section> sections = new LinkedHashMap<>();
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
            setErrorMessage("Multiple runs are only supported with Cmdenv, the command-line interface");
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
