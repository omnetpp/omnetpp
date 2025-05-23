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
import org.omnetpp.common.ui.SWTFactory;
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
    public static final String CAT_OUTPUTVECTORS = "Vector Results";
    public static final String CAT_OUTPUTSCALARS = "Scalar Results";
    public static final String CAT_STATISTICS = "Declared Statistics";
    public static final String CAT_DEBUGGING = "Debugging";
    public static final String CAT_FINGERPRINT = "Fingerprint";
    public static final String CAT_EVENTLOG = "Event Log";
    public static final String CAT_VISUAL = "Visual";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_QTENV = "Qtenv";
    public static final String CAT_EXTENSIONS = "Extensions";
    public static final String CAT_PARSIM = "Parallel Simulation";

    public static final Image ICON_WARNING = UIUtils.ICON_WARNING;

    public static String[] getCategoryNames() {
        return new String[] {
                CAT_GENERAL,
                CAT_SCENARIO,
                CAT_RANDOMNUMBERS,
                CAT_OUTPUTSCALARS,
                CAT_OUTPUTVECTORS,
                CAT_STATISTICS,
                CAT_VISUAL,
                CAT_DEBUGGING,
                CAT_FINGERPRINT,
                CAT_EVENTLOG,
                CAT_CMDENV,
                CAT_QTENV,
                CAT_EXTENSIONS,
                CAT_PARSIM,
                CAT_ADVANCED,
        };
    }

    public GenericConfigPage(Composite parent, String category, InifileEditor inifileEditor) {
        super(parent, inifileEditor);
        this.category = category;

        GridLayout gridLayout = new GridLayout(2, false);
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
        // Guideline: per-object options should come up as table, and NOT be collapsible?
        Group group = null;
        if (category.equals(CAT_GENERAL)) {
            group = createGroup(form, "Network");
            addTextFieldEditor(group, CFGID_NETWORK, "Network to simulate", c(null, "Name")); //FIXME use ComboBoxFieldEditor
            addSpacer(form);
            group = createGroup(form, "Stopping Condition");
            addTextFieldEditor(group, CFGID_SIM_TIME_LIMIT, "Simulation time limit", c(null, "Limit"));
            addTextFieldEditor(group, CFGID_CPU_TIME_LIMIT, "CPU time limit", c(null, "Limit"));
            addTextFieldEditor(group, CFGID_REAL_TIME_LIMIT, "Real time limit", c(null, "Limit"));
            addSpacer(form);
            group = createGroup(form, "Simulation Time Resolution");
            addComboboxFieldEditor(group, CFGID_SIMTIME_RESOLUTION, "Simulation time resolution", true);
            setCombo(CFGID_SIMTIME_RESOLUTION, SIMTIME_RESOLUTION_CHOICES);
            //TODO display extra info: "nanosecond resolution; range: +-100 days"
            addSpacer(form);
            group = createGroup(form, "Result Folder");
            addTextFieldEditor(group, CFGID_RESULT_DIR, "Result folder*", c(null, "Folder"));
            addNote(group, "* Determines the value of ${resultdir}, a macro that all default result file name definitions contain");
            addSpacer(group);
            addCheckboxFieldEditor(group, CFGID_RESULTDIR_SUBDIVISION, "Create sub-folders under the results folder");
            addCheckboxFieldEditor(group, CFGID_FNAME_APPEND_HOST, "Append host name to filenames");
        }
        else if (category.equals(CAT_ADVANCED)) {
            group = createGroup(form, "Output Vector Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_VECTOR_PRECISION, "Precision", c(null, "#Decimal Places"));
            addCheckboxFieldEditor(group, CFGID_OUTPUT_VECTOR_FILE_APPEND, "Append to existing vector file", c(null, "Append?"));
            addCheckboxFieldEditor(group, CFGID_VECTOR_RECORD_EMPTY, "Record empty vectors too", c("<modulepath>.<vectorname>", "Record if empty?"));
            addCheckboxFieldEditor(group, CFGID_VECTOR_RECORD_EVENTNUMBERS, "Record event numbers", c("<modulepath>.<vectorname>", "Record event numbers?"));
            addTextFieldEditor(group, CFGID_VECTOR_BUFFER, "Per-vector buffer size", c("<modulepath>.<vectorname>", "Buffer Size"));
            addTextFieldEditor(group, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "Total memory limit", c(null, "Memory Limit"));
            addCheckboxFieldEditor(group, CFGID_WITH_AKAROA, "Use vector with Akaroa*", c("<modulepath>.<parametername>", "Use Akaroa?"));
            addNote(group, "* Akaroa needs to be obtained and installed separately");
            addSpacer(form);
            group = createGroup(form, "Output Scalar Recording");
            addTextFieldEditor(group, CFGID_OUTPUT_SCALAR_PRECISION, "Precision", c(null, "#Decimal Places"));
            addCheckboxFieldEditor(group, CFGID_OUTPUT_SCALAR_FILE_APPEND, "Append to existing scalar file", c(null, "Append?"));
            addTextFieldEditor(group, CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ, "SQLite scalar file commit frequency");
            addComboboxFieldEditor(group, CFGID_OUTPUT_VECTOR_DB_INDEXING, "SQLite vector database indexing", true);
            addSpacer(form);
            group = createGroup(form, "Limits");
            addTextFieldEditor(group, CFGID_TOTAL_STACK, "Total activity stack");
            addTextFieldEditor(group, CFGID_MAX_MODULE_NESTING, "Allowed maximum module nesting", c(null, "#Levels"));
            addSpacer(form);
            group = createGroup(form, "Other");
            addTextFieldEditor(group, CFGID_RUNNUMBER_WIDTH, "Run number width", c(null, "Width"));
            addTextFieldEditor(group, CFGID_NED_PATH, "Additional NED path folders");
            addTextFieldEditor(group, CFGID_NED_PACKAGE_EXCLUSIONS, "Additional NED packages to exclude");
            addTextFieldEditor(group, CFGID_IMAGE_PATH, "Additional image path folders");
            addTextFieldEditor(group, CFGID_USER_INTERFACE, "User interface");
        }
        else if (category.equals(CAT_RANDOMNUMBERS)) {
            group = createGroup(form, "Random Number Generators");
            addTextFieldEditor(group, CFGID_NUM_RNGS, "Number of RNGs", c(null, "#RNGs"));
            addTextFieldEditor(group, CFGID_RNG_CLASS, "RNG class", c(null, "Class"));
            addSpacer(form);
            group = createGroup(form, "Automatic Seeds");
            addTextFieldEditor(group, CFGID_SEED_SET, "Seed set", c(null, "Index"));
            addNote(group, "Note: Further options also exist that allow specifying manual seeds, and mapping of the global RNGs to module-local RNGs.");
            //TODO the ones that contain "%":
            //group = createGroup(form, "Manual Seeds");
            //addTextTableFieldEditor(form, CFGID_RNG_n, "Module RNG mapping");
            //addTextTableFieldEditor(form, CFGID_SEED_n_LCG32, "Seed for LCG32 RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT, "Seed for Mersenne Twister RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT_Pn, "Per-partition Mersenne Twister seeds (for parsim)");
        }
        else if (category.equals(CAT_VISUAL)) {
            group = createGroup(form, "Visual");
            addTextTableFieldEditor(group, CFGID_DISPLAY_NAME, "Module display name", c("Module Path", "Display Name"));
            addTextTableFieldEditor(group, CFGID_DISPLAY_STRING, "Additional display string", c("Module Path", "Display String"));
        }
        else if (category.equals(CAT_SCENARIO)) {
            group = createGroup(form, "Run Labeling");
            addTextFieldEditor(group, CFGID_EXPERIMENT_LABEL, "Experiment label", c(null, "Label"));
            addTextFieldEditor(group, CFGID_MEASUREMENT_LABEL, "Measurement label", c(null, "Label"));
            addTextFieldEditor(group, CFGID_REPLICATION_LABEL, "Replication label", c(null, "Label"));
            addSpacer(form);
            group = createGroup(form, "Scenario Generation");
            addTextFieldEditor(group, CFGID_REPEAT, "Repeat count", c(null, "Count"));
            addTextFieldEditor(group, CFGID_CONSTRAINT, "Constraint expression", c(null, "Expression"));
            addTextFieldEditor(group, CFGID_ITERATION_NESTING_ORDER, "Loop nesting order", c(null, "Order"));
            addSpacer(form);
        }
        else if (category.equals(CAT_OUTPUTSCALARS)) {
            group = createGroup(form, "Output Scalar File");
            addCheckboxFieldEditor(group, CFGID_RECORD_SCALAR_RESULTS, "Enable writing an output scalar file");
            addTextFieldEditor(group, CFGID_OUTPUT_SCALAR_FILE, "Output scalar file*", c(null, "Filename"));
            addNote(group, "* An output scalar (.sca) file holds scalars, histograms, and statistical summaries recorded from simulations.");
            addNote(group, "The result directory can be configured on the " + CAT_GENERAL + " page.");
            addSpacer(group);
            addComboboxFieldEditor(group, CFGID_CONFIG_RECORDING, "Config options to record", false);
            setCombo(CFGID_CONFIG_RECORDING, CONFIG_RECORDING_CHOICES);
            addSpacer(form);
            group = createGroup(form, "Fine-grained Control");
            addCheckboxTableFieldEditor(group, CFGID_SCALAR_RECORDING, "Scalars to record (default: all)*", c("<modulepath>.<scalarname>", "Record?"));
            addNote(group, "* Applies to all item types: scalars, histograms, statistical summaries");
            addSpacer(group);
            addCheckboxFieldEditor(group, CFGID_BIN_RECORDING, "Record histogram bins**", c("<modulepath>.<scalarname>", "Record?"));
            addCheckboxFieldEditor(group, CFGID_PARAM_RECORDING, "Record parameters**", c("<modulepath>.<parametername>", "Record?"));
            addCheckboxFieldEditor(group, CFGID_PARAM_RECORD_AS_SCALAR, "Record parameters as scalars (obsolete)**", c("<modulepath>.<parametername>", "Record?"));
            addNote(group, "** Use the Expand icons on the right to specify the setting on a per-result basis");
            addSpacer(form);
            addLabel(form, "See the Advanced page for more settings.");
        }
        else if (category.equals(CAT_OUTPUTVECTORS)) {
            group = createGroup(form, "Output Vector File");
            addCheckboxFieldEditor(group, CFGID_RECORD_VECTOR_RESULTS, "Enable writing an output vector file");
            addTextFieldEditor(group, CFGID_OUTPUT_VECTOR_FILE, "Output vector file*", c(null, "Filename"));
            addNote(group, "* An output vector (.vec) file holds time series data (sequences of timestamped values) recorded from simulations");
            addNote(group, "The result directory can be configured on the " + CAT_GENERAL + " page.");
            addSpacer(form);
            group = createGroup(form, "Fine-grained Control");
            addTextFieldEditor(group, CFGID_VECTOR_RECORDING_INTERVALS, "Recording interval(s)*", c("<modulepath>.<vectorname>", "Intervals"));
            addNote(group, "* Use the Expand icons on the right to specify the setting on a per-vector basis");
            addSpacer(group);
            addCheckboxTableFieldEditor(group, CFGID_VECTOR_RECORDING, "Vectors to record (default: all)", c("<modulepath>.<vectorname>", "Record?"));
            addSpacer(form);
            addLabel(form, "See the Advanced page for more settings.");
        }
        else if (category.equals(CAT_STATISTICS)) {
            group = createGroup(form, "Overview");
            addLabel(group, 
                    "This page allows for configuring module (channel) statistics defined in NED files via @statistic properties. "
                    + "This functionality is a layer above output scalar and output vector recording: one @statistic may result "
                    + "in the recording of several scalar and vector result items. For example, a\n\n"
                    + "    @statistic[delay](...,record=mean,histogram,vector);\n\n"
                    + "property results in the recording of the 'delay:mean', 'delay:histogram' and 'delay:vector' results, "
                    + "which are each subject to the settings on the " + CAT_OUTPUTSCALARS + " and " + CAT_OUTPUTVECTORS + " pages. "
                    + "The 'mean', 'histogram', 'vector' words in the example are also known as \"recording modes\".");
            addSpacer(form);
            group = createGroup(form, "Fine-grained Control");
            addCheckboxTableFieldEditor(group, CFGID_STATISTIC_RECORDING, "Statistics to record*", c("<modulepath>.<statisticname>", "Record?"));
            addNote(group, "* Enables/disables the recording of all results from the matching @statistic properties as a whole");
            addSpacer(group);
            addTextTableFieldEditor(group, CFGID_RESULT_RECORDING_MODES, "Result recording modes", c("<modulepath>.<statisticname>", "Recording Modes"));
            addSpacer(form);
            group = createGroup(form, "Advanced");
            addTextFieldEditor(group, CFGID_WARMUP_PERIOD, "Warm-up period", c(null, "Value"));
        }
        else if (category.equals(CAT_DEBUGGING)) {
            group = createGroup(form, "General");
            addCheckboxFieldEditor(group, CFGID_DEBUG_ON_ERRORS, "Drop into the debugger on runtime errors");
            addSpacer(form);
            group = createGroup(form, "External Debugger");
            addCheckboxFieldEditor(group, CFGID_DEBUGGER_ATTACH_ON_STARTUP, "Attach debugger on startup");
            addCheckboxFieldEditor(group, CFGID_DEBUGGER_ATTACH_ON_ERROR, "Attach debugger on error");
            addTextFieldEditor(group, CFGID_DEBUGGER_ATTACH_COMMAND, "Debugger command");
            addTextFieldEditor(group, CFGID_DEBUGGER_ATTACH_WAIT_TIME, "Attach timeout");
            addNote(group, "Note: Using an external debugger requires extra configuration on some systems, including Ubuntu. Hint: \"ptrace_scope\".");
            addSpacer(form);
            group = createGroup(form, "Related Options");
            addCheckboxFieldEditor(group, CFGID_PRINT_UNDISPOSED, "Dump names of undisposed objects");
            addCheckboxFieldEditor(group, CFGID_ALLOW_OBJECT_STEALING_ON_DELETION, "Allow object stealing on component deletion");
            addCheckboxFieldEditor(group, CFGID_PARAMETER_MUTABILITY_CHECK, "Do not allow runtime changes made to parameters not marked as @mutable");
            addCheckboxFieldEditor(group, CFGID_CHECK_SIGNALS, "Check emitted signals against @signal declarations");
            addCheckboxFieldEditor(group, CFGID_DEBUG_STATISTICS_RECORDING, "Debug result recording");
            addCheckboxFieldEditor(group, CFGID_PRINT_INSTANTIATED_NED_TYPES, "Print list of instantiated NED types");
            addCheckboxFieldEditor(group, CFGID_PRINT_UNUSED_PARAMETERS, "Print names of unused parameters per NED type");
            addCheckboxFieldEditor(group, CFGID_WARNINGS, "Warnings"); //XXX
            addSpacer(form);
            group = createGroup(form, "Snapshot Files");
            addTextFieldEditor(group, CFGID_SNAPSHOT_FILE, "Snapshot file", c(null, "Filename"));
        }
        else if (category.equals(CAT_FINGERPRINT)) {
            group = createGroup(form, "Fingerprint");
            addTextFieldEditor(group, CFGID_FINGERPRINT, "Expected fingerprint(s)", c(null, "Fingerprint (Hex)"));
            addTextFieldEditor(group, CFGID_FINGERPRINT_INGREDIENTS, "Default ingredients");
            addCheckboxFieldEditor(group, CFGID_CMDENV_FAKE_GUI, "Enable FakeGUI mode*");
            addNote(group, "* In FakeGUI mode, Cmdenv calls refreshDisplay() periodically, allowing graphics to be included in the fingerprint");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_SEED, "Seed");
            addSpacer(form);
            group = createGroup(form, "Filter");
            addTextFieldEditor(group, CFGID_FINGERPRINT_EVENTS, "Events to include");
            addTextFieldEditor(group, CFGID_FINGERPRINT_MODULES, "Modules to include");
            addTextFieldEditor(group, CFGID_FINGERPRINT_RESULTS, "Results to include");
            addSpacer(form);
            group = createGroup(form, "Frequency of refreshDisplay() calls in FakeGUI mode");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_AFTER_EVENT_PROBABILITY, "After-event probability");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_BEFORE_EVENT_PROBABILITY, "Before-event probability");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_ON_HOLD_NUMSTEPS, "On-hold numsteps");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_ON_HOLD_PROBABILITY, "On-hold probability");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_ON_SIMTIME_NUMSTEPS, "On-simtime numsteps");
            addTextFieldEditor(group, CFGID_CMDENV_FAKE_GUI_ON_SIMTIME_PROBABILITY, "On-simtime probability");
        }
        else if (category.equals(CAT_EVENTLOG)) {
            group = createGroup(form, "Eventlog");
            addCheckboxFieldEditor(group, CFGID_RECORD_EVENTLOG, "Enable eventlog recording", c(null, "Enable?"));
            addTextFieldEditor(group, CFGID_EVENTLOG_FILE, "Eventlog file", c(null, "Filename"));
            addNote(group, "Note: Eventlog files (.elog) can be visualized in the Sequence Chart Tool.");
            addNote(group, "The result directory can be configured on the " + CAT_GENERAL + " page.");
            addSpacer(form);
            group = createGroup(form, "Content");
            addTextFieldEditor(group, CFGID_EVENTLOG_RECORDING_INTERVALS, "Recording intervals", c(null, "Intervals"));
            addTextFieldEditor(group, CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "Message details to record", c(null, "Pattern Expression"));
            addCheckboxTableFieldEditor(group, CFGID_MODULE_EVENTLOG_RECORDING, "Modules to record", c("Module Path", "Record?"));
            addTextFieldEditor(group, CFGID_EVENTLOG_OPTIONS, "Eventlog options", c(null, "Options"));
            addSpacer(form);
            group = createGroup(form, "Technical");
            addTextFieldEditor(group, CFGID_EVENTLOG_SNAPSHOT_FREQUENCY, "Eventlog snapshot frequency", c(null, "Value"));
            addTextFieldEditor(group, CFGID_EVENTLOG_INDEX_FREQUENCY, "Eventlog index frequency", c(null, "Value"));
            addTextFieldEditor(group, CFGID_EVENTLOG_MAX_SIZE, "Maximum eventlog size", c(null, "Size"));
            addTextFieldEditor(group, CFGID_EVENTLOG_MIN_TRUNCATED_SIZE, "Minimum truncated size", c(null, "Size"));
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
            addTextFieldEditor(group, CFGID_FINGERPRINTCALCULATOR_CLASS, "Fingerprint calculator class");
            addTextFieldEditor(group, CFGID_OUTPUTVECTORMANAGER_CLASS, "Output vector manager class");
            addTextFieldEditor(group, CFGID_OUTPUTSCALARMANAGER_CLASS, "Output scalar manager class");
            addTextFieldEditor(group, CFGID_EVENTLOGMANAGER_CLASS, "Eventlog manager class");
            addTextFieldEditor(group, CFGID_SNAPSHOTMANAGER_CLASS, "Snapshot manager class");
            addTextFieldEditor(group, CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS, "Config file reader class");
            addSpacer(form);
        }
        else if (category.equals(CAT_CMDENV)) {
            group = createGroup(form, "Default inifile configuration and run number");
            addTextFieldEditor(group, CFGID_CMDENV_CONFIG_NAME, "Config name");
            addTextFieldEditor(group, CFGID_CMDENV_RUNS_TO_EXECUTE, "Runs to execute");
            addNote(group, "Note: Command line options (-c, -r) override the above settings");
            addSpacer(form);
            addCheckboxFieldEditor(form, CFGID_CMDENV_EXPRESS_MODE, "Run in Express mode");
            addSpacer(form);
            group = createGroup(form, "Express Mode");
            addCheckboxFieldEditor(group, CFGID_CMDENV_PROGRESS_UPDATES, "Print progress updates");  //TODO refine docu!
            addCheckboxFieldEditor(group, CFGID_CMDENV_PERFORMANCE_DISPLAY, "Print performance data");
            addTextFieldEditor(group, CFGID_CMDENV_STATUS_FREQUENCY, "Status update frequency");
            addSpacer(form);
            group = createGroup(form, "Normal (Non-Express) Mode");
            addCheckboxFieldEditor(group, CFGID_CMDENV_EVENT_BANNERS, "Print event banners");
            addCheckboxFieldEditor(group, CFGID_CMDENV_EVENT_BANNER_DETAILS, "Detailed event banners");
            addComboboxFieldEditor(group, CFGID_CMDENV_LOG_LEVEL, "Log level (per module)", c("Component (component-path pattern)", "Log level"), false); //TODO 3-column editor
            setCombo(CFGID_CMDENV_LOG_LEVEL, LOGLEVEL_CHOICES);
            addTextFieldEditor(group, CFGID_CMDENV_LOG_PREFIX, "Log prefix");
            addSpacer(form);
            group = createGroup(form, "Other");
            addCheckboxFieldEditor(group, CFGID_CMDENV_STOP_BATCH_ON_ERROR, "Stop batch on first error");
            addCheckboxFieldEditor(group, CFGID_CMDENV_INTERACTIVE, "Allow interactivity");
            addCheckboxFieldEditor(group, CFGID_CMDENV_AUTOFLUSH, "Auto-flush output files");
            addCheckboxFieldEditor(group, CFGID_CMDENV_REDIRECT_OUTPUT, "Redirect stdout to per-run file");
            addTextFieldEditor(group, CFGID_CMDENV_OUTPUT_FILE, "Output file name");
            addTextFieldEditor(group, CFGID_CMDENV_EXTRA_STACK, "Extra coroutine stack");
            addSpacer(form);
            group = createGroup(form, "Logging");
            addCheckboxFieldEditor(group, CFGID_CMDENV_LOG_INITIALIZATION, "Log during network initialization");
            addCheckboxFieldEditor(group, CFGID_CMDENV_LOG_SIMULATION, "Log during simulation");
            addCheckboxFieldEditor(group, CFGID_CMDENV_LOG_FINALIZATION, "Log during network finalization");
            addCheckboxFieldEditor(group, CFGID_CMDENV_LOG_CLEANUP, "Log during network cleanup");
        }
        else if (category.equals(CAT_QTENV)) {
            group = createGroup(form, "On startup, set up the following simulation:");
            addTextFieldEditor(group, CFGID_QTENV_DEFAULT_CONFIG, "Config name");
            addTextFieldEditor(group, CFGID_QTENV_DEFAULT_RUN, "Run number");
            addSpacer(form);
            group = createGroup(form, "Other");
            addTextFieldEditor(group, CFGID_QTENV_EXTRA_STACK, "Extra coroutine stack");
            addTextFieldEditor(group, CFGID_QTENV_IDENTICON_SEED, "Identicon seed");
            addSpacer(form);
        }
        else if (category.equals(CAT_PARSIM)) {
            addCheckboxFieldEditor(form, CFGID_PARALLEL_SIMULATION, "Enable parallel simulation");
            group = createGroup(form, "Partitioning");
            addTextFieldEditor(group, CFGID_PARSIM_NUM_PARTITIONS, "Number of partitions");
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
    }

    protected void setCombo(ConfigOption option, String[] choices) {
        FieldEditor fieldEditor = getFieldEditorFor(option);
        if (fieldEditor != null)
            fieldEditor.setComboContents(Arrays.asList(choices));
    }

    protected Label addSpacer(Composite parent) {
        return SWTFactory.createLabel(parent, "", 2);
    }

    protected Label addNote(Composite parent, String text) {
        Label label = SWTFactory.createWrapLabel(parent, text, 2);
        label.setFont(SWTFactory.getSmallFont());
        return label;
    }

    protected Label addLabel(Composite parent, String text) {
        return SWTFactory.createWrapLabel(parent, text, 2);
    }

    protected Group createGroup(Composite parent, String groupLabel) {
        final Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));
        group.setText(groupLabel);

        GridLayout gridLayout = new GridLayout(2, false);
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
        Label l = addFieldEditorLabel(parent, e, label);
        FieldEditor editor = e.isGlobal() ?
                new TextFieldEditor(parent, e, getInifileDocument(), this, hints) :
                new ExpandableTextFieldEditor(parent, e, getInifileDocument(), this, "", hints);
        addFieldEditor(l, editor);
        return editor;
    }

    protected FieldEditor addComboboxFieldEditor(Composite parent, ConfigOption e, String label, boolean enableTypein) {
        return addComboboxFieldEditor(parent, e, label, null, enableTypein);
    }

    protected FieldEditor addComboboxFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints, boolean enableTypein) {
        Label l = addFieldEditorLabel(parent, e, label);
        FieldEditor editor = e.isGlobal() ?
                new ComboFieldEditor(parent, e, getInifileDocument(), this, hints, enableTypein) :
                new ExpandableComboFieldEditor(parent, e, getInifileDocument(), this, "", hints, enableTypein);
        addFieldEditor(l, editor);
        return editor;
    }

    protected FieldEditor addCheckboxFieldEditor(Composite parent, ConfigOption e, String label) {
        return addCheckboxFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addCheckboxFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        Label l = addFieldEditorLabel(parent, e, label);
        FieldEditor editor = e.isGlobal() ?
                new CheckboxFieldEditor(parent, e, getInifileDocument(), this, hints) :
                new ExpandableCheckboxFieldEditor(parent, e, getInifileDocument(), this, "", hints);
        addFieldEditor(l, editor);
        return editor;
    }

    protected FieldEditor addTextTableFieldEditor(Composite parent, ConfigOption e, String label) {
        return addTextTableFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addTextTableFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        Label l = addFieldEditorLabel(parent, e, label);
        FieldEditor editor = new TextTableFieldEditor(parent, e, getInifileDocument(), this, hints);
        addFieldEditor(l, editor);
        return editor;
    }

    protected FieldEditor addCheckboxTableFieldEditor(Composite parent, ConfigOption e, String label) {
        return addCheckboxTableFieldEditor(parent, e, label, null);
    }

    protected FieldEditor addCheckboxTableFieldEditor(Composite parent, ConfigOption e, String label, Map<String,Object> hints) {
        Label l = addFieldEditorLabel(parent, e, label);
        FieldEditor editor = new CheckboxTableFieldEditor(parent, e, getInifileDocument(), this, hints);
        addFieldEditor(l, editor);
        return editor;
    }

    protected Label addFieldEditorLabel(Composite parent, ConfigOption e, String text) {
        var label = SWTFactory.createLabel(parent, text, 1);
        var gd = (GridData)label.getLayoutData();
        gd.verticalIndent = 3;
        gd.verticalAlignment = SWT.BEGINNING;
        return label;
    }

    protected void addFieldEditor(Label label, FieldEditor editor) {
        fieldEditors.add(editor);
        editor.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        editor.addTooltipSupport(label);
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
        for (FieldEditor e : fieldEditors)
            e.reread();
    }

    @Override
    public void analysisCompleted() {
        for (FieldEditor e : fieldEditors)
            e.refreshDecorations();
    }

    @Override
    public List<ConfigOption> getSupportedKeys() {
        List<ConfigOption> result = new ArrayList<ConfigOption>();
        for (FieldEditor e : fieldEditors)
            result.add(e.getConfigKey());
        return result;
    }

}
