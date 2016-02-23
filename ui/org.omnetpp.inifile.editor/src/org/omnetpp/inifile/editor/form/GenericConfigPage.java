/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigOption;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;

/**
 * Form page for configuration entries.
 *
 * @author Andras
 */
public class GenericConfigPage extends ScrolledFormPage {
    private ArrayList<FieldEditor> fieldEditors = new ArrayList<FieldEditor>();
    private String category;

    public static final String CAT_GENERAL = "General";
    public static final String CAT_ADVANCED = "Advanced";
    public static final String CAT_SCENARIO = "Scenarios";
    public static final String CAT_RANDOMNUMBERS = "Random Numbers";
    public static final String CAT_RESULTRECORDING = "Result Recording";
    public static final String CAT_DEBUGGING = "Debugging";
    public static final String CAT_EVENTLOG = "Event Log";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_TKENV = "Tkenv";
    public static final String CAT_QTENV = "Qtenv";
    public static final String CAT_EXTENSIONS = "Extensions";
    public static final String CAT_PARSIM = "Parallel Simulation";

    public static final Image ICON_WARNING = UIUtils.ICON_WARNING;

    public static String[] getCategoryNames() {
        return new String[] {
                CAT_GENERAL,
                CAT_SCENARIO,
                CAT_RANDOMNUMBERS,
                CAT_RESULTRECORDING,
                CAT_DEBUGGING,
                CAT_EVENTLOG,
                CAT_CMDENV,
                CAT_TKENV,
                CAT_QTENV,
                CAT_EXTENSIONS,
                CAT_PARSIM,
                CAT_ADVANCED,
        };
    }

    public GenericConfigPage(Composite parent, String category, InifileEditor inifileEditor) {
        super(parent, inifileEditor);
        this.category = category;

        GridLayout gridLayout = new GridLayout(1,false);
        gridLayout.verticalSpacing = 0;
        form.setLayout(gridLayout);

        createTitleArea(form, category);
        addSpacer(form);

        createFieldEditors(form, category);
        layoutForm();

        // obtain focus if background is clicked
        form.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                setFocus();
            }
        });
    }

    protected void createFieldEditors(Composite form, String category) {
        //TODO revise: per-object options should come up as table, and NOT be collapsible?
        Group group = null;
        if (category.equals(CAT_GENERAL)) {
            group = createGroup(form, "Network");
            addTextFieldEditor(group, CFGID_NETWORK, "Network to simulate", c(null, "Name")); //FIXME use ComboBoxFieldEditor
            addSpacer(form);
            group = createGroup(form, "Stopping Condition");
            addTextFieldEditor(group, CFGID_SIM_TIME_LIMIT, "Simulation time limit", c(null, "Simulation Time Limit"));
            addTextFieldEditor(group, CFGID_CPU_TIME_LIMIT, "CPU time limit", c(null, "CPU Time Limit"));
            addSpacer(form);
            group = createGroup(form, "Other");
            addComboboxFieldEditor(group, CFGID_SIMTIME_PRECISION, "Simulation time precision");
            //TODO display extra info: "nanosecond resolution; range: +-100 days"
            addSpacer(form);
        }
        else if (category.equals(CAT_ADVANCED)) {
            group = createGroup(form, "Regression");
            addTextFieldEditor(group, CFGID_FINGERPRINT, "Fingerprint to verify", c(null, "Fingerprint (Hex)"));
            addLabel(group, "Further options allow controlling the ingredients of the fingerprint, filtering by modules/events, etc.");
            addSpacer(form);
            group = createGroup(form, "Output Vector Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_VECTOR_PRECISION, "Precision", c(null, "Precision"));
            addCheckboxFieldEditor(group, CFGID_VECTOR_RECORD_EVENTNUMBERS, "Record event numbers", c("Vector (module-path.vectorname pattern)", null));
            addTextFieldEditor(group, CFGID_VECTOR_MAX_BUFFERED_VALUES, "Buffered size for output vectors", c("Vector (module-path.vectorname pattern)", "Number of Values"));
            addTextFieldEditor(group, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "Total memory limit", c(null, "Memory Limit"));
            addSpacer(form);
            group = createGroup(form, "Output Scalar Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_SCALAR_PRECISION, "Precision", c(null, "Precision"));
            addCheckboxFieldEditor(group, CFGID_OUTPUT_SCALAR_FILE_APPEND, "Append to existing file");
            addSpacer(form);
            group = createGroup(form, "Limits");
            addTextFieldEditor(group, CFGID_TOTAL_STACK, "Total activity stack");
            addTextFieldEditor(group, CFGID_MAX_MODULE_NESTING, "Allowed maximum module nesting", c(null, "Nesting Level"));
            addSpacer(form);
            group = createGroup(form, "Other");
            addTextFieldEditor(group, CFGID_RUNNUMBER_WIDTH, "Run number width", c(null, "Width"));
            addTextFieldEditor(group, CFGID_NED_PATH, "Additional NED path folders");
            addTextFieldEditor(group, CFGID_IMAGE_PATH, "Additional image path folders");
            addTextFieldEditor(group, CFGID_USER_INTERFACE, "User interface");
        }
        else if (category.equals(CAT_RANDOMNUMBERS)) {
            group = createGroup(form, "Random Number Generators");
            addTextFieldEditor(group, CFGID_NUM_RNGS, "Number of RNGs", c(null, "Number of RNGs"));
            addTextFieldEditor(group, CFGID_RNG_CLASS, "RNG class", c(null, "RNG Class"));
            addLabel(group, "Further options allow mapping of the global RNGs to module-local RNGs.");
            addSpacer(form);
            group = createGroup(form, "Automatic Seeds");
            addTextFieldEditor(group, CFGID_SEED_SET, "Seed set", c(null, "Index of Seed Set"));
            //TODO the ones that contain "%":
            //group = createGroup(form, "Manual Seeds");
            //addTextTableFieldEditor(form, CFGID_RNG_n, "Module RNG mapping");
            //addTextTableFieldEditor(form, CFGID_SEED_n_LCG32, "Seed for LCG32 RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT, "Seed for Mersenne Twister RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT_Pn, "Per-partition Mersenne Twister seeds (for parsim)");
        }
        else if (category.equals(CAT_SCENARIO)) {
            group = createGroup(form, "Run Labeling");
            addTextFieldEditor(group, CFGID_EXPERIMENT_LABEL, "Experiment label", c(null, "Label"));
            addTextFieldEditor(group, CFGID_MEASUREMENT_LABEL, "Measurement label", c(null, "Label"));
            addTextFieldEditor(group, CFGID_REPLICATION_LABEL, "Replication label", c(null, "Label"));
            addSpacer(form);
            group = createGroup(form, "Scenario Generation");
            addTextFieldEditor(group, CFGID_REPEAT, "Repeat count", c(null, "Repeat Count"));
            addTextFieldEditor(group, CFGID_CONSTRAINT, "Constraint expression", c(null, "Expression"));
            addSpacer(form);
        }
        else if (category.equals(CAT_RESULTRECORDING)) {
            addTextFieldEditor(form, CFGID_RESULT_DIR, "Result folder", c(null, "Results Folder"));
            addCheckboxFieldEditor(form, CFGID_FNAME_APPEND_HOST, "Append host name to filenames");
            addSpacer(form);
            group = createGroup(form, "Statistic Recording");
            addCheckboxFieldEditor(group, CFGID_STATISTIC_RECORDING, "Enable recording of @statistics", c("Statistic (module-path.statisticname pattern)", null));
            addTextFieldEditor(group, CFGID_RESULT_RECORDING_MODES, "Result recording modes", c("Statistic (module-path.statisticname pattern)", "Recording Modes"));
            addTextFieldEditor(group, CFGID_WARMUP_PERIOD, "Warm-up period", c(null, "Warm-up Period"));
            addSpacer(form);
            group = createGroup(form, "Output Vector Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_VECTOR_FILE, "Output vector file", c(null, "Filename"));
            addCheckboxFieldEditor(group, CFGID_VECTOR_RECORDING, "Enable recording of vectors", c("Vector (module-path.vectorname pattern)", null));
            addTextFieldEditor(group, CFGID_VECTOR_RECORDING_INTERVALS, "Recording intervals", c("Vector (module-path.vectorname pattern)", "Intervals"));
            addSpacer(form);
            group = createGroup(form, "Output Scalar Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_SCALAR_FILE, "Output scalar file", c(null, "Filename"));
            addCheckboxFieldEditor(group, CFGID_SCALAR_RECORDING, "Enable recording of scalars", c("Scalar (module-path.scalarname pattern)", null));
            addCheckboxFieldEditor(group, CFGID_BIN_RECORDING, "Record histogram bins", c("Scalar (module-path.scalarname pattern)", null));
            addCheckboxFieldEditor(group, CFGID_PARAM_RECORD_AS_SCALAR, "Record parameters as scalars", c("Parameter (module-path.paramname pattern)", null));
            addSpacer(form);
        }
        else if (category.equals(CAT_DEBUGGING)) {
            group = createGroup(form, "General");
            addCheckboxFieldEditor(group, CFGID_DEBUG_ON_ERRORS, "Generate debug trap on errors");
            addSpacer(form);
            group = createGroup(form, "External Debugger");
            addCheckboxFieldEditor(group, CFGID_DEBUGGER_ATTACH_ON_STARTUP, "Attach debugger on startup");
            addCheckboxFieldEditor(group, CFGID_DEBUGGER_ATTACH_ON_ERROR, "Attach debugger on error");
            addTextFieldEditor(group, CFGID_DEBUGGER_ATTACH_COMMAND, "Debugger command");
            addTextFieldEditor(group, CFGID_DEBUGGER_ATTACH_WAIT_TIME, "Attach timeout");
            addLabel(group, "Note: Using an external debugger requires extra configuration on some systems, including Ubuntu (hint: \"ptrace_scope\").");
            addSpacer(form);
            group = createGroup(form, "Related Options");
            addCheckboxFieldEditor(group, CFGID_CHECK_SIGNALS, "Check emitted signals against @signal declarations");
            addCheckboxFieldEditor(group, CFGID_DEBUG_STATISTICS_RECORDING, "Debug result recording");
            addCheckboxFieldEditor(group, CFGID_PRINT_UNDISPOSED, "Dump names of undisposed objects");
            addCheckboxFieldEditor(group, CFGID_WARNINGS, "Warnings"); //XXX
            addSpacer(form);
        }
        else if (category.equals(CAT_EVENTLOG)) {
            group = createGroup(form, "Eventlog");
            addCheckboxFieldEditor(group, CFGID_RECORD_EVENTLOG, "Enable eventlog recording");
            addTextFieldEditor(group, CFGID_EVENTLOG_FILE, "Eventlog file", c(null, "Filename"));
            addTextFieldEditor(group, CFGID_EVENTLOG_RECORDING_INTERVALS, "Recording intervals", c(null, "Intervals"));
            addTextFieldEditor(group, CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "Message details to record", c(null, "Pattern Expression"));
            addCheckboxFieldEditor(group, CFGID_MODULE_EVENTLOG_RECORDING, "Modules to record", c("Module", null));
            addLabel(group, "Event log files (.elog) can be visualized in the Sequence Chart Tool.");
            addSpacer(form);
            group = createGroup(form, "Snapshots");
            addTextFieldEditor(group, CFGID_SNAPSHOT_FILE, "Snapshot file", c(null, "Filename"));
            addSpacer(form);
        }
        else if (category.equals(CAT_EXTENSIONS)) {
            addTextFieldEditor(form, CFGID_LOAD_LIBS, "Shared libraries to load");
            addSpacer(form);
            group = createGroup(form, "Extension Classes");
            addTextFieldEditor(group, CFGID_CONFIGURATION_CLASS, "Configuration class");
            addTextFieldEditor(group, CFGID_SCHEDULER_CLASS, "Scheduler class");
            addTextFieldEditor(group, CFGID_REALTIMESCHEDULER_SCALING, "Real-Time scheduler scaling");
            addTextFieldEditor(group, CFGID_FUTUREEVENTSET_CLASS, "Future Event Set (FES) class");
            addTextFieldEditor(group, CFGID_OUTPUTVECTORMANAGER_CLASS, "Output vector manager class");
            addTextFieldEditor(group, CFGID_OUTPUTSCALARMANAGER_CLASS, "Output scalar manager class");
            addTextFieldEditor(group, CFGID_EVENTLOGMANAGER_CLASS, "Eventlog manager class");
            addTextFieldEditor(group, CFGID_SNAPSHOTMANAGER_CLASS, "Snapshot manager class");
            addTextFieldEditor(group, CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS, "Config file reader class");
            addSpacer(form);
        }
        else if (category.equals(CAT_CMDENV)) {
            group = createGroup(form, "When no command-line options are present, run the following:");
            addTextFieldEditor(group, CFGID_CMDENV_CONFIG_NAME, "Config name");
            addTextFieldEditor(group, CFGID_CMDENV_RUNS_TO_EXECUTE, "Run numbers");
            addSpacer(form);
            addCheckboxFieldEditor(form, CFGID_CMDENV_EXPRESS_MODE, "Run in Express mode");
            addSpacer(form);
            group = createGroup(form, "Express Mode");
            addCheckboxFieldEditor(group, CFGID_CMDENV_PERFORMANCE_DISPLAY, "Display performance data");
            addTextFieldEditor(group, CFGID_CMDENV_STATUS_FREQUENCY, "Status frequency");
            addSpacer(form);
            group = createGroup(form, "Normal (Non-Express) Mode");
            addCheckboxFieldEditor(group, CFGID_CMDENV_EVENT_BANNERS, "Print event banners");
            addCheckboxFieldEditor(group, CFGID_CMDENV_EVENT_BANNER_DETAILS, "Detailed event banners");
            addComboboxFieldEditor(group, CFGID_CMDENV_LOG_LEVEL, "Log level (per module)", c("Component (component-path pattern)", "Log level")); //TODO 3-column editor
            addTextFieldEditor(group, CFGID_CMDENV_LOG_FORMAT, "Log prefix");
            addSpacer(form);
            group = createGroup(form, "Other");
            addCheckboxFieldEditor(group, CFGID_CMDENV_INTERACTIVE, "Allow interactivity");  //TODO when opened (table), it should contain a 3rd, combobox-based column (true/false) instead of a checkbox
            addCheckboxFieldEditor(group, CFGID_CMDENV_AUTOFLUSH, "Auto-flush output files");
            addTextFieldEditor(group, CFGID_CMDENV_OUTPUT_FILE, "Redirect stdout to file");
            addTextFieldEditor(group, CFGID_CMDENV_EXTRA_STACK, "Extra coroutine stack");
        }
        else if (category.equals(CAT_TKENV)) {
            group = createGroup(form, "On startup, set up the following simulation:");
            addTextFieldEditor(group, CFGID_TKENV_DEFAULT_CONFIG, "Config name");
            addTextFieldEditor(group, CFGID_TKENV_DEFAULT_RUN, "Run number");
            addSpacer(form);
            group = createGroup(form, "Other");
            addTextFieldEditor(group, CFGID_TKENV_PLUGIN_PATH, "Plugin path");
            addTextFieldEditor(group, CFGID_TKENV_EXTRA_STACK, "Extra coroutine stack");
            addSpacer(form);
        }
        else if (category.equals(CAT_QTENV)) {
            group = createGroup(form, "On startup, set up the following simulation:");
            addTextFieldEditor(group, CFGID_QTENV_DEFAULT_CONFIG, "Config name");
            addTextFieldEditor(group, CFGID_QTENV_DEFAULT_RUN, "Run number");
            addSpacer(form);
            group = createGroup(form, "Other");
            addTextFieldEditor(group, CFGID_QTENV_EXTRA_STACK, "Extra coroutine stack");
            addSpacer(form);
        }
        else if (category.equals(CAT_PARSIM)) {
            addCheckboxFieldEditor(form, CFGID_PARALLEL_SIMULATION, "Enable parallel simulation");
            group = createGroup(form, "Partitioning");
            addTextTableFieldEditor(group, CFGID_PARTITION_ID, "Module partitioning", c("Module", "Partition ID(s)"));
            addSpacer(form);
            group = createGroup(form, "General");
            addTextFieldEditor(group, CFGID_PARSIM_COMMUNICATIONS_CLASS, "Communications class");
            addTextFieldEditor(group, CFGID_PARSIM_SYNCHRONIZATION_CLASS, "Synchronization class");
            addCheckboxFieldEditor(group, CFGID_PARSIM_DEBUG, "Debug parallel simulation");
            addSpacer(form);
            group = createGroup(form, "Communications");
            addTextFieldEditor(group, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "MPI communications: MPI buffer");
            addTextFieldEditor(group, CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX, "Named Pipe communications: prefix");
            addTextFieldEditor(group, CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX, "File-based communications: prefix");
            addTextFieldEditor(group, CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX, "File-based communications: read prefix");
            addCheckboxFieldEditor(group, CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ, "File-based communications: preserve read files");
            addSpacer(form);
            group = createGroup(form, "Protocol-Specific Settings");
            addTextFieldEditor(group, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, "Null Message Protocol: lookahead class");
            addTextFieldEditor(group, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, "Null Message Protocol: laziness");
            addTextFieldEditor(group, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "Ideal Simulation Protocol: table size");
            addSpacer(form);
        }
        else {
            throw new IllegalArgumentException("no such category: "+category);
        }

        // initialize combo boxes with static content
        FieldEditor simtimePrecisionEditor = getFieldEditorFor(CFGID_SIMTIME_PRECISION);
        if (simtimePrecisionEditor != null)
            simtimePrecisionEditor.setComboContents(Arrays.asList(SIMTIME_PRECISION_CHOICES));
        FieldEditor cmdenvLoglevelEditor = getFieldEditorFor(CFGID_CMDENV_LOG_LEVEL);
        if (cmdenvLoglevelEditor != null)
            cmdenvLoglevelEditor.setComboContents(Arrays.asList(LOGLEVEL_CHOICES));
    }

    protected Label addSpacer(Composite parent) {
        return new Label(parent, SWT.NONE);
    }

    protected Label addLabel(Composite parent, String text) {
        Label label = new Label(parent, SWT.NONE);
        label.setText(text);
        return label;
    }

    protected Group createGroup(Composite parent, String groupLabel) {
        final Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        group.setText(groupLabel);

        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.verticalSpacing = 0;
        gridLayout.marginHeight = 5;
        group.setLayout(gridLayout);

        // when group background is clicked, transfer focus to first field editor in it
        group.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                for (Control c : group.getChildren())
                    if (c instanceof FieldEditor)
                        {c.setFocus(); return;}
            }
        });

        return group;
    }

    protected Map<String,Object> c(String objectColumnTitle, String valueColumnTitle) {
        Map<String,Object> result = new HashMap<String,Object>();
        result.put(TableFieldEditor.HINT_OBJECT_COL_TITLE, objectColumnTitle);
        result.put(TableFieldEditor.HINT_VALUE_COL_TITLE, valueColumnTitle);
        return result;
    }

    protected FieldEditor addTextFieldEditor(Composite parent, ConfigOption e, String label) {
        return addTextFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addTextFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        FieldEditor editor = e.isGlobal() ?
                new TextFieldEditor(parent, e, getInifileDocument(), this, label, hints) :
                new ExpandableTextFieldEditor(parent, e, getInifileDocument(), this, label, hints);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addComboboxFieldEditor(Composite parent, ConfigOption e, String label) {
        return addComboboxFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addComboboxFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        FieldEditor editor = e.isGlobal() ?
                new ComboFieldEditor(parent, e, getInifileDocument(), this, label, hints) :
                new ExpandableComboFieldEditor(parent, e, getInifileDocument(), this, label, hints);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addCheckboxFieldEditor(Composite parent, ConfigOption e, String label) {
        return addCheckboxFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addCheckboxFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        FieldEditor editor = e.isGlobal() ?
                new CheckboxFieldEditor(parent, e, getInifileDocument(), this, label, hints) :
                new ExpandableCheckboxFieldEditor(parent, e, getInifileDocument(), this, label, hints);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addTextTableFieldEditor(Composite parent, ConfigOption e, String label) {
        return addTextTableFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addTextTableFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        FieldEditor editor = new TextTableFieldEditor(parent, e, getInifileDocument(), this, label, hints);
        addFieldEditor(editor);
        return editor;
    }

    protected FieldEditor addCheckboxTableFieldEditor(Composite parent, ConfigOption e, String label) {
        return addCheckboxTableFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addCheckboxTableFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        FieldEditor editor = new CheckboxTableFieldEditor(parent, e, getInifileDocument(), this, label, hints);
        addFieldEditor(editor);
        return editor;
    }

    protected void addFieldEditor(FieldEditor editor) {
        fieldEditors.add(editor);
        editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
    }

    protected FieldEditor getFieldEditorFor(ConfigOption configOption) {
        for (FieldEditor e : fieldEditors)
            if (e.getConfigKey() == configOption)
                return e;
        return null;
    }

    protected boolean occursOutsideGeneral(String key) {
        IReadonlyInifileDocument doc = getInifileDocument();
        for (String section : doc.getSectionNames())
            if (doc.containsKey(section, key) && !section.equals(GENERAL))
                return true;
        return false;
    }

    @Override
    public String getPageCategory() {
        return category;
    }

    @Override
    public boolean setFocus() {
        return fieldEditors.isEmpty() ? false : fieldEditors.get(0).setFocus();
    }

    @Override
    public void reread() {
        super.reread();
        for (FieldEditor e : fieldEditors)
            e.reread();
    }

    @Override
    public List<ConfigOption> getSupportedKeys() {
        List<ConfigOption> result = new ArrayList<ConfigOption>();
        for (FieldEditor e : fieldEditors)
            result.add(e.getConfigKey());
        return result;
    }

}
