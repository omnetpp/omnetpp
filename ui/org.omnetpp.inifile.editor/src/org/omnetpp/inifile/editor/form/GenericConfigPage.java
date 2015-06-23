/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CHECK_SIGNALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_AUTOFLUSH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_CONFIG_NAME;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EVENT_BANNER_DETAILS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EV_OUTPUT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXPRESS_MODE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_EXTRA_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_INTERACTIVE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_LOG_FORMAT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_LOG_LEVEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_MESSAGE_TRACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_MODULE_MESSAGES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_OUTPUT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_PERFORMANCE_DISPLAY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_RUNS_TO_EXECUTE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CMDENV_STATUS_FREQUENCY;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONFIGURATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CONSTRAINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_CPU_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUGGER_ATTACH_COMMAND;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUGGER_ATTACH_ON_ERROR;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUGGER_ATTACH_ON_STARTUP;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUGGER_ATTACH_WAIT_TIME;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUG_ON_ERRORS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DEBUG_STATISTICS_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EVENTLOG_RECORDING_INTERVALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXPERIMENT_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FINGERPRINT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FNAME_APPEND_HOST;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_FUTUREEVENTSET_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_LOAD_LIBS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_LOG_LEVEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MAX_MODULE_NESTING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MEASUREMENT_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_MODULE_EVENTLOG_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NED_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NUM_RNGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUTSCALARMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUTVECTORMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_FILE_APPEND;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_SCALAR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTORS_MEMORY_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTOR_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_OUTPUT_VECTOR_PRECISION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARALLEL_SIMULATION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARAM_RECORD_AS_SCALAR;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_COMMUNICATIONS_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_DEBUG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARSIM_SYNCHRONIZATION_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PARTITION_ID;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_PRINT_UNDISPOSED;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REALTIMESCHEDULER_SCALING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RECORD_EVENTLOG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPEAT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_REPLICATION_LABEL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RESULT_DIR;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RESULT_RECORDING_MODES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RNG_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_RUNNUMBER_WIDTH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SCALAR_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SCHEDULER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SEED_SET;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIMTIME_SCALE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SIM_TIME_LIMIT;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOTMANAGER_CLASS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_SNAPSHOT_FILE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_CONFIG;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_DEFAULT_RUN;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_EXTRA_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_IMAGE_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TKENV_PLUGIN_PATH;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TOTAL_STACK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_MAX_BUFFERED_VALUES;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORDING_INTERVALS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_VECTOR_RECORD_EVENTNUMBERS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_WARMUP_PERIOD;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_WARNINGS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

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
    public static final String CAT_EVENTLOG = "Event Log";
    public static final String CAT_CMDENV = "Cmdenv";
    public static final String CAT_TKENV = "Tkenv";
    public static final String CAT_EXTENSIONS = "Extensions";
    public static final String CAT_PARSIM = "Parallel Simulation";

    public static final String[] SIMTIME_SCALE_CHOICES = new String[] {
        "0", "-3", "-6", "-9", "-12", "-15", "-18",  //TODO add (s),(ms),(us),(ns),(ps),(fs),(as) once combo supports it
    };

    public static final String[] LOGLEVEL_CHOICES = new String[] {
        "FATAL", "ERROR", "WARN", "INFO", "DETAILS", "DEBUG", "TRACE"
    };

    public static final Image ICON_WARNING = UIUtils.ICON_WARNING;

    public static String[] getCategoryNames() {
        return new String[] {
                CAT_GENERAL,
                CAT_SCENARIO,
                CAT_RANDOMNUMBERS,
                CAT_RESULTRECORDING,
                CAT_EVENTLOG,
                CAT_ADVANCED,
                CAT_CMDENV,
                CAT_TKENV,
                CAT_EXTENSIONS,
                CAT_PARSIM,
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
        if (category.equals(CAT_GENERAL)) {
            Group group1 = createGroup(form, "Network");
            addTextFieldEditor(group1, CFGID_NETWORK, "Network to simulate", c(null, "Name")); //FIXME use ComboBoxFieldEditor
            addSpacer(form);
            Group group2 = createGroup(form, "Stopping Condition");
            addTextFieldEditor(group2, CFGID_SIM_TIME_LIMIT, "Simulation time limit", c(null, "Simulation Time Limit"));
            addTextFieldEditor(group2, CFGID_CPU_TIME_LIMIT, "CPU time limit", c(null, "CPU Time Limit"));
            addSpacer(form);
            Group group3 = createGroup(form, "Other");
            addComboboxFieldEditor(group3, CFGID_SIMTIME_SCALE, "Simulation time precision");
            //TODO display extra info: "nanosecond resolution; range: +-100 days"
            addSpacer(form);
        }
        else if (category.equals(CAT_ADVANCED)) {
            Group group0 = createGroup(form, "Regression");
            addTextFieldEditor(group0, CFGID_FINGERPRINT, "Fingerprint to verify", c(null, "Fingerprint (Hex)"));
            addSpacer(form);
            Group group01 = createGroup(form, "Logging");
            addTextFieldEditor(group01, CFGID_LOG_LEVEL, "Per-module log level", c("Module", "Log level")); //TODO combo
            addSpacer(form);
            Group group2 = createGroup(form, "Debugging");
            addCheckboxFieldEditor(group2, CFGID_DEBUG_ON_ERRORS, "Debug on errors");
            addCheckboxFieldEditor(group2, CFGID_CHECK_SIGNALS, "Check emitted signals against @signal declarations");
            addCheckboxFieldEditor(group2, CFGID_PRINT_UNDISPOSED, "Dump names of undisposed objects");
            addCheckboxFieldEditor(group2, CFGID_WARNINGS, "Warnings"); //XXX
            addCheckboxFieldEditor(group2, CFGID_DEBUG_ON_ERRORS, "Debug on errors");
            addSpacer(form);
            Group group3 = createGroup(form, "External Debugger");
            addTextFieldEditor(group3, CFGID_DEBUGGER_ATTACH_COMMAND, "Debugger command");
            addCheckboxFieldEditor(group3, CFGID_DEBUGGER_ATTACH_ON_ERROR, "Attach debugger on errors");
            addCheckboxFieldEditor(group3, CFGID_DEBUGGER_ATTACH_ON_STARTUP, "Attach debugger on startup");
            addTextFieldEditor(group3, CFGID_DEBUGGER_ATTACH_WAIT_TIME, "Attach timeout");
            addLabel(group3, "Note: Using an external debugger requires extra configuration on some systems, including Ubuntu.");
            addSpacer(form);
            Group group4 = createGroup(form, "Output Vector Recording");
            addTextFieldEditor(group4, CFGID_OUTPUT_VECTOR_PRECISION, "Precision", c(null, "Precision"));
            addCheckboxFieldEditor(group4, CFGID_VECTOR_RECORD_EVENTNUMBERS, "Record event numbers", c("Vector (module-path.vectorname pattern)", null));
            addTextFieldEditor(group4, CFGID_VECTOR_MAX_BUFFERED_VALUES, "Buffered size for output vectors", c("Vector (module-path.vectorname pattern)", "Number of Values"));
            addTextFieldEditor(group4, CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "Total memory limit", c(null, "Memory Limit"));
            addSpacer(form);
            Group group5 = createGroup(form, "Output Scalar Recording");
            addTextFieldEditor(group5, CFGID_OUTPUT_SCALAR_PRECISION, "Precision", c(null, "Precision"));
            addCheckboxFieldEditor(group5, CFGID_OUTPUT_SCALAR_FILE_APPEND, "Append to existing file");
            addSpacer(form);
            Group group1 = createGroup(form, "Limits");
            addTextFieldEditor(group1, CFGID_TOTAL_STACK, "Total activity stack");
            addTextFieldEditor(group1, CFGID_MAX_MODULE_NESTING, "Allowed maximum module nesting", c(null, "Nesting Level"));
            addSpacer(form);
            Group group6 = createGroup(form, "Other");
            addTextFieldEditor(group6, CFGID_RUNNUMBER_WIDTH, "Run number width", c(null, "Width"));
            addTextFieldEditor(group6, CFGID_NED_PATH, "NED file path");
            addTextFieldEditor(group6, CFGID_USER_INTERFACE, "User interface");
        }
        else if (category.equals(CAT_RANDOMNUMBERS)) {
            Group group1 = createGroup(form, "Random Number Generators");
            addTextFieldEditor(group1, CFGID_NUM_RNGS, "Number of RNGs", c(null, "Number of RNGs"));
            addTextFieldEditor(group1, CFGID_RNG_CLASS, "RNG class", c(null, "RNG Class"));
            addSpacer(form);
            Group group2 = createGroup(form, "Automatic Seeds");
            addTextFieldEditor(group2, CFGID_SEED_SET, "Seed set", c(null, "Index of Seed Set"));
            //TODO the ones that contain "%":
            //Group group3 = createGroup(form, "Manual Seeds");
            //addTextTableFieldEditor(form, CFGID_RNG_n, "Module RNG mapping");
            //addTextTableFieldEditor(form, CFGID_SEED_n_LCG32, "Seed for LCG32 RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT, "Seed for Mersenne Twister RNG");
            //addTextTableFieldEditor(form, CFGID_SEED_n_MT_Pn, "Per-partition Mersenne Twister seeds (for parsim)");
        }
        else if (category.equals(CAT_SCENARIO)) {
            Group group1 = createGroup(form, "Run Labeling");
            addTextFieldEditor(group1, CFGID_EXPERIMENT_LABEL, "Experiment label", c(null, "Label"));
            addTextFieldEditor(group1, CFGID_MEASUREMENT_LABEL, "Measurement label", c(null, "Label"));
            addTextFieldEditor(group1, CFGID_REPLICATION_LABEL, "Replication label", c(null, "Label"));
            addSpacer(form);
            Group group2 = createGroup(form, "Scenario Generation");
            addTextFieldEditor(group2, CFGID_REPEAT, "Repeat count", c(null, "Repeat Count"));
            addTextFieldEditor(group2, CFGID_CONSTRAINT, "Constraint expression", c(null, "Expression"));
            addSpacer(form);
        }
        else if (category.equals(CAT_RESULTRECORDING)) {
            addTextFieldEditor(form, CFGID_RESULT_DIR, "Result folder", c(null, "Results Folder"));
            addCheckboxFieldEditor(form, CFGID_FNAME_APPEND_HOST, "Append host name to filenames");
            addSpacer(form);
            Group group0 = createGroup(form, "Statistic Recording");
            addTextFieldEditor(group0, CFGID_WARMUP_PERIOD, "Warm-up period", c(null, "Warm-up Period"));
            addTextFieldEditor(group0, CFGID_RESULT_RECORDING_MODES, "Result recording modes", c("Statistic (module-path.statisticname pattern)", "Recording Modes"));
            addCheckboxFieldEditor(group0, CFGID_DEBUG_STATISTICS_RECORDING, "Debug result recording");
            addSpacer(form);
            Group group1 = createGroup(form, "Output Vector Recording");
            addTextFieldEditor(group1, CFGID_OUTPUT_VECTOR_FILE, "Output vector file", c(null, "Filename"));
            addCheckboxFieldEditor(group1, CFGID_VECTOR_RECORDING, "Enable recording of vectors", c("Vector (module-path.vectorname pattern)", null));
            addTextFieldEditor(group1, CFGID_VECTOR_RECORDING_INTERVALS, "Recording intervals", c("Vector (module-path.vectorname pattern)", "Intervals"));
            addSpacer(form);
            Group group2 = createGroup(form, "Output Scalar Recording");
            addTextFieldEditor(group2, CFGID_OUTPUT_SCALAR_FILE, "Output scalar file", c(null, "Filename"));
            addCheckboxFieldEditor(group2, CFGID_SCALAR_RECORDING, "Enable recording of scalars", c("Scalar (module-path.scalarname pattern)", null));
            addCheckboxFieldEditor(group2, CFGID_PARAM_RECORD_AS_SCALAR, "Parameters to save as scalars", c("Parameter (module-path.paramname pattern)", null));
            addSpacer(form);
        }
        else if (category.equals(CAT_EVENTLOG)) {
            Group group0 = createGroup(form, "Eventlog");
            addCheckboxFieldEditor(group0, CFGID_RECORD_EVENTLOG, "Enable eventlog recording");
            addTextFieldEditor(group0, CFGID_EVENTLOG_FILE, "Eventlog file", c(null, "Filename"));
            addTextFieldEditor(group0, CFGID_EVENTLOG_RECORDING_INTERVALS, "Recording intervals", c(null, "Intervals"));
            addTextFieldEditor(group0, CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "Message details to record", c(null, "Pattern Expression"));
            addCheckboxFieldEditor(group0, CFGID_MODULE_EVENTLOG_RECORDING, "Modules to record", c("Module", null));
            addSpacer(form);
            Group group3 = createGroup(form, "Snapshots");
            addTextFieldEditor(group3, CFGID_SNAPSHOT_FILE, "Snapshot file", c(null, "Filename"));
            addSpacer(form);
        }
        else if (category.equals(CAT_EXTENSIONS)) {
            addTextFieldEditor(form, CFGID_LOAD_LIBS, "Shared libraries to load");
            addSpacer(form);
            Group group1 = createGroup(form, "Extension Classes");
            addTextFieldEditor(group1, CFGID_CONFIGURATION_CLASS, "Configuration class");
            addTextFieldEditor(group1, CFGID_SCHEDULER_CLASS, "Scheduler class");
            addTextFieldEditor(group1, CFGID_REALTIMESCHEDULER_SCALING, "Real-Time scheduler scaling");
            addTextFieldEditor(group1, CFGID_FUTUREEVENTSET_CLASS, "Future Event Set (FES) class");
            addTextFieldEditor(group1, CFGID_OUTPUTVECTORMANAGER_CLASS, "Output vector manager class");
            addTextFieldEditor(group1, CFGID_OUTPUTSCALARMANAGER_CLASS, "Output scalar manager class");
            addTextFieldEditor(group1, CFGID_SNAPSHOTMANAGER_CLASS, "Snapshot manager class");
            addTextFieldEditor(group1, CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS, "Config file reader class");
            addSpacer(form);
        }
        else if (category.equals(CAT_CMDENV)) {
            Group group0 = createGroup(form, "When no command-line options are present, run the following:");
            addTextFieldEditor(group0, CFGID_CMDENV_CONFIG_NAME, "Config name");
            addTextFieldEditor(group0, CFGID_CMDENV_RUNS_TO_EXECUTE, "Run numbers");
            addSpacer(form);
            addCheckboxFieldEditor(form, CFGID_CMDENV_EXPRESS_MODE, "Run in Express mode");
            addSpacer(form);
            Group group1 = createGroup(form, "Express Mode");
            addCheckboxFieldEditor(group1, CFGID_CMDENV_PERFORMANCE_DISPLAY, "Display performance data");
            addTextFieldEditor(group1, CFGID_CMDENV_STATUS_FREQUENCY, "Status frequency");
            addSpacer(form);
            Group group2 = createGroup(form, "Normal (Non-Express) Mode");
            addCheckboxFieldEditor(group2, CFGID_CMDENV_MODULE_MESSAGES, "Print module messages");
            addCheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNERS, "Print event banners");
            addCheckboxFieldEditor(group2, CFGID_CMDENV_EVENT_BANNER_DETAILS, "Detailed event banners");
            addCheckboxFieldEditor(group2, CFGID_CMDENV_MESSAGE_TRACE, "Message trace");
            addCheckboxFieldEditor(group2, CFGID_CMDENV_EV_OUTPUT, "Module log");
            addComboboxFieldEditor(group2, CFGID_CMDENV_LOG_LEVEL, "Log level");
            addTextFieldEditor(group2, CFGID_CMDENV_LOG_FORMAT, "Log prefix");
            addSpacer(form);
            Group group3 = createGroup(form, "Miscellaneus");
            addCheckboxFieldEditor(group3, CFGID_CMDENV_INTERACTIVE, "Interactive mode");
            addCheckboxFieldEditor(group3, CFGID_CMDENV_AUTOFLUSH, "Auto-flush output files");
            addTextFieldEditor(group3, CFGID_CMDENV_OUTPUT_FILE, "Redirect stdout to file");
            addTextFieldEditor(group3, CFGID_CMDENV_EXTRA_STACK, "Cmdenv extra stack");
        }
        else if (category.equals(CAT_TKENV)) {
            Group group0 = createGroup(form, "On startup, set up the following simulation:");
            addTextFieldEditor(group0, CFGID_TKENV_DEFAULT_CONFIG, "Config name");
            addTextFieldEditor(group0, CFGID_TKENV_DEFAULT_RUN, "Run number");
            addSpacer(form);
            Group group4 = createGroup(form, "Paths");
            addTextFieldEditor(group4, CFGID_TKENV_IMAGE_PATH, "Image path");
            addTextFieldEditor(group4, CFGID_TKENV_PLUGIN_PATH, "Plugin path");
            addSpacer(form);
            addTextFieldEditor(form, CFGID_TKENV_EXTRA_STACK, "Tkenv extra stack");
            addSpacer(form);
        }
        else if (category.equals(CAT_PARSIM)) {
            addCheckboxFieldEditor(form, CFGID_PARALLEL_SIMULATION, "Enable parallel simulation");
            Group group0 = createGroup(form, "Partitioning");
            addTextTableFieldEditor(group0, CFGID_PARTITION_ID, "Module partitioning", c("Module", "Partition ID(s)"));
            addSpacer(form);
            Group group1 = createGroup(form, "General");
            addTextFieldEditor(group1, CFGID_PARSIM_COMMUNICATIONS_CLASS, "Communications class");
            addTextFieldEditor(group1, CFGID_PARSIM_SYNCHRONIZATION_CLASS, "Synchronization class");
            addCheckboxFieldEditor(group1, CFGID_PARSIM_DEBUG, "Debug parallel simulation");
            addSpacer(form);
            Group group2 = createGroup(form, "Communications");
            addTextFieldEditor(group2, CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER, "MPI communications: MPI buffer");
            addTextFieldEditor(group2, CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX, "Named Pipe communications: prefix");
            addTextFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX, "File-based communications: prefix");
            addTextFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX, "File-based communications: read prefix");
            addCheckboxFieldEditor(group2, CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ, "File-based communications: preserve read files");
            addSpacer(form);
            Group group3 = createGroup(form, "Protocol-Specific Settings");
            addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, "Null Message Protocol: lookahead class");
            addTextFieldEditor(group3, CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, "Null Message Protocol: laziness");
            addTextFieldEditor(group3, CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "Ideal Simulation Protocol: table size");
            addSpacer(form);
        }
        else {
            throw new IllegalArgumentException("no such category: "+category);
        }

        // initialize combo boxes with static content
        FieldEditor simtimeScaleEditor = getFieldEditorFor(CFGID_SIMTIME_SCALE);
        if (simtimeScaleEditor != null)
            simtimeScaleEditor.setComboContents(Arrays.asList(SIMTIME_SCALE_CHOICES));
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
