/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_BOOL;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_CUSTOM;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_DOUBLE;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_FILENAME;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_FILENAMES;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_INT;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_PATH;
import static org.omnetpp.inifile.editor.model.ConfigOption.DataType.CFG_STRING;
import static org.omnetpp.inifile.editor.model.ConfigOption.ObjectKind.*;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import org.omnetpp.inifile.editor.model.ConfigOption.DataType;
import org.omnetpp.inifile.editor.model.ConfigOption.ObjectKind;

/**
 * Contains the list of supported configuration options.
 *
 * This must be kept in sync with the C++ code. Procedure to do it:
 *   1. run any simulation executable with the "-h jconfig" option,
 *       and copy/paste the Java code it generates into this file
 *   2. run git diff to see the damage.
 *   3. check that the form editor covers all config options:
 *      open InifileFormEditor.java, and temporarily set
 *      DUMP_FORGOTTEN_CONFIG_KEYS = true. Then run the IDE and watch
 *      the output.
 *
 * @author Andras
 */
public class ConfigRegistry {
    /** Name of the [General] section */
    public static final String GENERAL = "General";

    /** Name prefix for the [Section &lt;name&gt;] sections; includes a trailing space. */
    public static final String CONFIG_ = "Config ";

    /** Name of the "extends=" config option */
    public static final String EXTENDS; // initialized at the bottom of this file

    /** Name of the "**.typename=" config option */
    public static final String TYPENAME; // initialized at the bottom of this file

    /** Inifile value keyword*/
    public static final String DEFAULT = "default";

    /** Inifile value keyword*/
    public static final String ASK = "ask";

    private static Map<String, ConfigOption> options = new HashMap<>();
    private static Map<String, ConfigOption> perObjectOptions = new HashMap<>();
    private static Map<String, String> configVars = new LinkedHashMap<>(); // preserve order

    private static ConfigOption addGlobalOption(String name, DataType type, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, true, type, null, defaultValue, description);
        options.put(name, e);
        return e;
    }

    private static ConfigOption addGlobalOptionU(String name, String unit, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, true, CFG_DOUBLE, unit, defaultValue, description);
        options.put(name, e);
        return e;
    }

    private static ConfigOption addPerRunOption(String name, DataType type, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, false, type, null, defaultValue, description);
        options.put(name, e);
        return e;
    }

    private static ConfigOption addPerRunOptionU(String name, String unit, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, false, CFG_DOUBLE, unit, defaultValue, description);
        options.put(name, e);
        return e;
    }

    private static ConfigOption addPerObjectOption(String name, ObjectKind objectKind, DataType type, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, objectKind, type, null, defaultValue, description);
        perObjectOptions.put(name, e);
        return e;
    }

    @SuppressWarnings("unused")
    private static ConfigOption addPerObjectOptionU(String name, ObjectKind objectKind, String unit, String defaultValue, String description) {
        ConfigOption e = new ConfigOption(name, objectKind, CFG_DOUBLE, unit, defaultValue, description);
        perObjectOptions.put(name, e);
        return e;
    }

    private static ConfigOption lookupWildcardConfigOption(Map<String, ConfigOption> map, String key) {
        // config keys may contain wildcards: '*' (any string) and '%' (numeric string)
        for (ConfigOption e : map.values()) {
            if (e.containsWildcard()) {
                String pattern = e.getName().replace("*", ".+").replace("%", "[0-9]+");
                if (key.matches(pattern))
                    return e;
            }
        }
        return null;
    }

    /**
     * Returns the ConfigOption with the given name, or null. It also finds and returns
     * wildcard options that match the given name, for example it will return "seed-%-mt"
     * for "seed-1-mt".
     */
    public static ConfigOption getOption(String name) {
        ConfigOption e = options.get(name);
        if (e == null)
            e = lookupWildcardConfigOption(options, name);
        return e;
    }

    /**
     * Returns the per-object ConfigOption with the given name, or null. It also finds
     * and returns wildcard options that match the given name, for example it will return
     * "rng-%" for "rng-1".
     */
    public static ConfigOption getPerObjectOption(String name) {
        ConfigOption e = perObjectOptions.get(name);
        if (e == null)
            e = lookupWildcardConfigOption(perObjectOptions, name);
        return e;
    }

    public static ConfigOption[] getOptions() {
        return options.values().toArray(new ConfigOption[options.size()]);
    }

    public static ConfigOption[] getPerObjectOptions() {
        return perObjectOptions.values().toArray(new ConfigOption[perObjectOptions.size()]);
    }

    private static String addConfigVariable(String name, String description) {
        configVars.put(name, description);
        return name;
    }

    public static String[] getConfigVariableNames() {
        return configVars.keySet().toArray(new String[]{});
    }

    public static String getConfigVariableDescription(String variable) {
        return configVars.get(variable);
    }

    /*=== The following code has been generated by "opp_run -h jconfig" ===*/
    public static final ConfigOption CFGID_BIN_RECORDING = addPerObjectOption(
        "bin-recording", KIND_SCALAR, CFG_BOOL, "true",
        "Whether the bins of the matching histogram object should be recorded, " +
        "provided that recording of the histogram object itself is enabled " +
        "(`**.<scalar-name>.scalar-recording=true`).\n" +
        "Usage: `<module-full-path>.<scalar-name>.bin-recording=true/false`. To " +
        "control histogram recording from a `@statistic`, use " +
        "`<statistic-name>:histogram` for `<scalar-name>`.\n" +
        "Example: `**.ping.roundTripTime:histogram.bin-recording=false`");
    public static final ConfigOption CFGID_CHECK_SIGNALS = addPerRunOption(
        "check-signals", CFG_BOOL, "true",
        "Controls whether the simulation kernel will validate signals emitted by " +
        "modules and channels against signal declarations (`@signal` properties) in " +
        "NED files. The default setting depends on the build type: `true` in DEBUG, " +
        "and `false` in RELEASE mode.");
    public static final ConfigOption CFGID_CMDENV_AUTOFLUSH = addPerRunOption(
        "cmdenv-autoflush", CFG_BOOL, "false",
        "Call `fflush(stdout)` after each event banner or status update; affects " +
        "both express and normal mode. Turning on autoflush may have a performance " +
        "penalty, but it can be useful with printf-style debugging for tracking down " +
        "program crashes.");
    public static final ConfigOption CFGID_CMDENV_CONFIG_NAME = addGlobalOption(
        "cmdenv-config-name", CFG_STRING, null,
        "Specifies the name of the configuration to be run (for a value `Foo`, " +
        "section `[Config Foo]` will be used from the ini file). See also " +
        "`cmdenv-runs-to-execute`. The `-c` command line option overrides this " +
        "setting.");
    public static final ConfigOption CFGID_CMDENV_EVENT_BANNER_DETAILS = addPerRunOption(
        "cmdenv-event-banner-details", CFG_BOOL, "false",
        "When `cmdenv-express-mode=false`: print extra information after event " +
        "banners.");
    public static final ConfigOption CFGID_CMDENV_EVENT_BANNERS = addPerRunOption(
        "cmdenv-event-banners", CFG_BOOL, "true",
        "When `cmdenv-express-mode=false`: turns printing event banners on/off.");
    public static final ConfigOption CFGID_CMDENV_EXPRESS_MODE = addPerRunOption(
        "cmdenv-express-mode", CFG_BOOL, "true",
        "Selects normal (debug/trace) or express mode.");
    public static final ConfigOption CFGID_CMDENV_EXTRA_STACK = addGlobalOptionU(
        "cmdenv-extra-stack", "B", "8KiB",
        "Specifies the extra amount of stack that is reserved for each `activity()` " +
        "simple module when the simulation is run under Cmdenv.");
    public static final ConfigOption CFGID_CMDENV_INTERACTIVE = addPerRunOption(
        "cmdenv-interactive", CFG_BOOL, "false",
        "Defines what Cmdenv should do when the model contains unassigned " +
        "parameters. In interactive mode, it asks the user. In non-interactive mode " +
        "(which is more suitable for batch execution), Cmdenv stops with an error.");
    public static final ConfigOption CFGID_CMDENV_LOG_LEVEL = addPerObjectOption(
        "cmdenv-log-level", KIND_MODULE, CFG_STRING, "TRACE",
        "Specifies the per-component level of detail recorded by log statements, " +
        "output below the specified level is omitted. Available values are (case " +
        "insensitive): `off`, `fatal`, `error`, `warn`, `info`, `detail`, `debug` or " +
        "`trace`. Note that the level of detail is also controlled by the globally " +
        "specified runtime log level and the `COMPILETIME_LOGLEVEL` macro that is " +
        "used to completely remove log statements from the executable.");
    public static final ConfigOption CFGID_CMDENV_LOG_PREFIX = addPerRunOption(
        "cmdenv-log-prefix", CFG_STRING, "[%l]  ",
        "Specifies the format string that determines the prefix of each log line. " +
        "The format string may contain format directives in the syntax `%x` (a `%` " +
        "followed by a single format character).  For example `%l` stands for log " +
        "level, and `%J` for source component. See the manual for the list of " +
        "available format characters.");
    public static final ConfigOption CFGID_CMDENV_OUTPUT_FILE = addPerRunOption(
        "cmdenv-output-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.out",
        "When `cmdenv-record-output=true`: file name to redirect standard output to. " +
        "See also `fname-append-host`.");
    public static final ConfigOption CFGID_CMDENV_PERFORMANCE_DISPLAY = addPerRunOption(
        "cmdenv-performance-display", CFG_BOOL, "true",
        "When `cmdenv-express-mode=true`: print detailed performance information. " +
        "Turning it on results in a 3-line entry printed on each update, containing " +
        "ev/sec, simsec/sec, ev/simsec, number of messages created/still " +
        "present/currently scheduled in FES.");
    public static final ConfigOption CFGID_CMDENV_REDIRECT_OUTPUT = addPerRunOption(
        "cmdenv-redirect-output", CFG_BOOL, "false",
        "Causes Cmdenv to redirect standard output of simulation runs to a file or " +
        "separate files per run. This option can be useful with running simulation " +
        "campaigns (e.g. using opp_runall), and also with parallel simulation. See " +
        "also: `cmdenv-output-file`, `fname-append-host`.");
    public static final ConfigOption CFGID_CMDENV_RUNS_TO_EXECUTE = addGlobalOption(
        "cmdenv-runs-to-execute", CFG_STRING, null,
        "Specifies which runs to execute from the selected configuration (see " +
        "`cmdenv-config-name` option). It accepts a filter expression of iteration " +
        "variables such as `$numHosts>10 && $iatime==1s`, or a comma-separated list " +
        "of run numbers or run number ranges, e.g. `1,3..4,7..9`. If the value is " +
        "missing, Cmdenv executes all runs in the selected configuration. The `-r` " +
        "command line option overrides this setting.");
    public static final ConfigOption CFGID_CMDENV_STATUS_FREQUENCY = addPerRunOptionU(
        "cmdenv-status-frequency", "s", "2s",
        "When `cmdenv-express-mode=true`: print status update every n seconds.");
    public static final ConfigOption CFGID_CMDENV_STOP_BATCH_ON_ERROR = addPerRunOption(
        "cmdenv-stop-batch-on-error", CFG_BOOL, "true",
        "Decides whether Cmdenv should skip the rest of the runs when an error " +
        "occurs during the execution of one run.");
    public static final ConfigOption CFGID_CONFIGURATION_CLASS = addGlobalOption(
        "configuration-class", CFG_STRING, null,
        "Part of the Envir plugin mechanism: selects the class from which all " +
        "configuration information will be obtained. This option lets you replace " +
        "omnetpp.ini with some other implementation, e.g. database input. The " +
        "simulation program still has to bootstrap from an omnetpp.ini (which " +
        "contains the configuration-class setting). The class should implement the " +
        "`cConfigurationEx` interface.");
    public static final ConfigOption CFGID_CONSTRAINT = addPerRunOption(
        "constraint", CFG_STRING, null,
        "For scenarios. Contains an expression that iteration variables (`${}` " +
        "syntax) must satisfy for that simulation to run. Example: `$i < $j+1`.");
    public static final ConfigOption CFGID_CPU_TIME_LIMIT = addPerRunOptionU(
        "cpu-time-limit", "s", null,
        "Stops the simulation when CPU usage has reached the given limit. The " +
        "default is no limit. Note: To reduce per-event overhead, this time limit is " +
        "only checked every N events (by default, N=1024).");
    public static final ConfigOption CFGID_DEBUG_ON_ERRORS = addGlobalOption(
        "debug-on-errors", CFG_BOOL, "false",
        "When set to true, runtime errors will cause the simulation program to break " +
        "into the C++ debugger (if the simulation is running under one, or " +
        "just-in-time debugging is activated). Once in the debugger, you can view " +
        "the stack trace or examine variables.");
    public static final ConfigOption CFGID_DEBUG_STATISTICS_RECORDING = addPerRunOption(
        "debug-statistics-recording", CFG_BOOL, "false",
        "Turns on the printing of debugging information related to statistics " +
        "recording (`@statistic` properties)");
    public static final ConfigOption CFGID_DEBUGGER_ATTACH_COMMAND = addGlobalOption(
        "debugger-attach-command", CFG_STRING, "nemiver --attach=%u &",
        "Command line to launch the debugger. It must contain exactly one percent " +
        "sign, as `%u`, which will be replaced by the PID of this process. The " +
        "command must not block (i.e. it should end in `&` on Unix-like systems).");
    public static final ConfigOption CFGID_DEBUGGER_ATTACH_ON_ERROR = addGlobalOption(
        "debugger-attach-on-error", CFG_BOOL, "false",
        "When set to true, runtime errors and crashes will trigger an external " +
        "debugger to be launched, allowing you to perform just-in-time debugging on " +
        "the simulation process. The debugger command is configurable. Note that " +
        "debugging (i.e. attaching to) a non-child process needs to be explicitly " +
        "enabled on some systems, e.g. Ubuntu.");
    public static final ConfigOption CFGID_DEBUGGER_ATTACH_ON_STARTUP = addGlobalOption(
        "debugger-attach-on-startup", CFG_BOOL, "false",
        "When set to true, the simulation program will launch an external debugger " +
        "attached to it, allowing you to set breakpoints before proceeding. The " +
        "debugger command is configurable.  Note that debugging (i.e. attaching to) " +
        "a non-child process needs to be explicitly enabled on some systems, e.g. " +
        "Ubuntu.");
    public static final ConfigOption CFGID_DEBUGGER_ATTACH_WAIT_TIME = addGlobalOptionU(
        "debugger-attach-wait-time", "s", "20s",
        "An interval to wait after launching the external debugger, to give the " +
        "debugger time to start up and attach to the simulation process.");
    public static final ConfigOption CFGID_DESCRIPTION = addPerRunOption(
        "description", CFG_STRING, null,
        "Descriptive name for the given simulation configuration. Descriptions get " +
        "displayed in the run selection dialog.");
    public static final ConfigOption CFGID_EVENTLOG_FILE = addPerRunOption(
        "eventlog-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.elog",
        "Name of the eventlog file to generate.");
    public static final ConfigOption CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN = addPerRunOption(
        "eventlog-message-detail-pattern", CFG_CUSTOM, null,
        "A list of patterns separated by '|' character which will be used to write " +
        "message detail information into the eventlog for each message sent during " +
        "the simulation. The message detail will be presented in the sequence chart " +
        "tool. Each pattern starts with an object pattern optionally followed by ':' " +
        "character and a comma separated list of field patterns. In both patterns " +
        "and/or/not/* and various field match expressions can be used. The object " +
        "pattern matches to class name, the field pattern matches to field name by " +
        "default.\n" +
        "  `EVENTLOG-MESSAGE-DETAIL-PATTERN := ( DETAIL-PATTERN '|' )* " +
        "DETAIL_PATTERN`\n" +
        "  `DETAIL-PATTERN := OBJECT-PATTERN [ ':' FIELD-PATTERNS ]`\n" +
        "  `OBJECT-PATTERN := MATCH-EXPRESSION`\n" +
        "  `FIELD-PATTERNS := ( FIELD-PATTERN ',' )* FIELD_PATTERN`\n" +
        "  `FIELD-PATTERN := MATCH-EXPRESSION`\n" +
        "Examples:\n" +
        "  `*`: captures all fields of all messages\n" +
        "  `*Frame:*Address,*Id`: captures all fields named somethingAddress and " +
        "somethingId from messages of any class named somethingFrame\n" +
        "  `MyMessage:declaredOn(MyMessage)`: captures instances of MyMessage " +
        "recording the fields declared on the MyMessage class\n" +
        "  `*:(not declaredOn(cMessage) and not declaredOn(cNamedObject) and not " +
        "declaredOn(cObject))`: records user-defined fields from all messages");
    public static final ConfigOption CFGID_EVENTLOG_RECORDING_INTERVALS = addPerRunOption(
        "eventlog-recording-intervals", CFG_CUSTOM, null,
        "Simulation time interval(s) when events should be recorded. Syntax: " +
        "`[<from>]..[<to>],...` That is, both start and end of an interval are " +
        "optional, and intervals are separated by comma. Example: `..10.2, " +
        "22.2..100, 233.3..`");
    public static final ConfigOption CFGID_EVENTLOGMANAGER_CLASS = addGlobalOption(
        "eventlogmanager-class", CFG_STRING, "omnetpp::envir::EventlogFileManager",
        "Part of the Envir plugin mechanism: selects the eventlog manager class to " +
        "be used to record data. The class has to implement the `cIEventlogManager` " +
        "interface.");
    public static final ConfigOption CFGID_EXPERIMENT_LABEL = addPerRunOption(
        "experiment-label", CFG_STRING, "${configname}",
        "Identifies the simulation experiment (which consists of several, " +
        "potentially repeated measurements). This string gets recorded into result " +
        "files, and may be referred to during result analysis.");
    public static final ConfigOption CFGID_EXTENDS = addPerRunOption(
        "extends", CFG_STRING, null,
        "Name of the configuration this section is based on. Entries from that " +
        "section will be inherited and can be overridden. In other words, " +
        "configuration lookups will fall back to the base section.");
    public static final ConfigOption CFGID_FINGERPRINT = addPerRunOption(
        "fingerprint", CFG_STRING, null,
        "The expected fingerprints of the simulation. If you need multiple " +
        "fingerprints, separate them with commas. When provided, the fingerprints " +
        "will be calculated from the specified properties of simulation events, " +
        "messages, and statistics during execution, and checked against the provided " +
        "values. Fingerprints are suitable for crude regression tests. As " +
        "fingerprints occasionally differ across platforms, more than one value can " +
        "be specified for a single fingerprint, separated by spaces, and a match " +
        "with any of them will be accepted. To obtain a fingerprint, enter a dummy " +
        "value (such as `0000`), and run the simulation.");
    public static final ConfigOption CFGID_FINGERPRINT_EVENTS = addPerRunOption(
        "fingerprint-events", CFG_STRING, "*",
        "Configures the fingerprint calculator to consider only certain events. The " +
        "value is a pattern that will be matched against the event name by default. " +
        "It may also be an expression containing pattern matching characters, field " +
        "access, and logical operators. The default setting is '*' which includes " +
        "all events in the calculated fingerprint. If you configured multiple " +
        "fingerprints, separate filters with commas.");
    public static final ConfigOption CFGID_FINGERPRINT_INGREDIENTS = addPerRunOption(
        "fingerprint-ingredients", CFG_STRING, "tplx",
        "Specifies the list of ingredients to be taken into account for fingerprint " +
        "computation. Each character corresponds to one ingredient: 'e' event " +
        "number, 't' simulation time, 'n' message (event) full name, 'c' message " +
        "(event) class name, 'k' message kind, 'l' message bit length, 'o' message " +
        "control info class name, 'd' message data, 'i' module id, 'm' module full " +
        "name, 'p' module full path, 'a' module class name, 'r' random numbers " +
        "drawn, 's' scalar results, 'z' statistic results, 'v' vector results, 'x' " +
        "extra data provided by modules. Note: ingredients specified in an expected " +
        "fingerprint (characters after the '/' in the fingerprint value) take " +
        "precedence over this setting. If you configured multiple fingerprints, " +
        "separate ingredients with commas.");
    public static final ConfigOption CFGID_FINGERPRINT_MODULES = addPerRunOption(
        "fingerprint-modules", CFG_STRING, "*",
        "Configures the fingerprint calculator to consider only certain modules. The " +
        "value is a pattern that will be matched against the module full path by " +
        "default. It may also be an expression containing pattern matching " +
        "characters, field access, and logical operators. The default setting is '*' " +
        "which includes all events in all modules in the calculated fingerprint. If " +
        "you configured multiple fingerprints, separate filters with commas.");
    public static final ConfigOption CFGID_FINGERPRINT_RESULTS = addPerRunOption(
        "fingerprint-results", CFG_STRING, "*",
        "Configures the fingerprint calculator to consider only certain results. The " +
        "value is a pattern that will be matched against the result full path by " +
        "default. It may also be an expression containing pattern matching " +
        "characters, field access, and logical operators. The default setting is '*' " +
        "which includes all results in all modules in the calculated fingerprint. If " +
        "you configured multiple fingerprints, separate filters with commas.");
    public static final ConfigOption CFGID_FINGERPRINTCALCULATOR_CLASS = addGlobalOption(
        "fingerprintcalculator-class", CFG_STRING, "omnetpp::cSingleFingerprintCalculator",
        "Part of the Envir plugin mechanism: selects the fingerprint calculator " +
        "class to be used to calculate the simulation fingerprint. The class has to " +
        "implement the `cFingerprintCalculator` interface.");
    public static final ConfigOption CFGID_FNAME_APPEND_HOST = addGlobalOption(
        "fname-append-host", CFG_BOOL, null,
        "Turning it on will cause the host name and process Id to be appended to the " +
        "names of output files (e.g. omnetpp.vec, omnetpp.sca). This is especially " +
        "useful with distributed simulation. The default value is true if parallel " +
        "simulation is enabled, false otherwise.");
    public static final ConfigOption CFGID_FUTUREEVENTSET_CLASS = addGlobalOption(
        "futureeventset-class", CFG_STRING, "omnetpp::cEventHeap",
        "Part of the Envir plugin mechanism: selects the class for storing the " +
        "future events in the simulation. The class has to implement the " +
        "`cFutureEventSet` interface.");
    public static final ConfigOption CFGID_IMAGE_PATH = addGlobalOption(
        "image-path", CFG_PATH, null,
        "A semicolon-separated list of directories that contain module icons and " +
        "other resources. This list with be concatenated with `OMNETPP_IMAGE_PATH`.");
    public static final ConfigOption CFGID_ITERATION_NESTING_ORDER = addPerRunOption(
        "iteration-nesting-order", CFG_STRING, null,
        "Specifies the loop nesting order for iteration variables (`${}` syntax). " +
        "The value is a comma-separated list of iteration variables; the list may " +
        "also contain at most one asterisk. Variables that are not explicitly listed " +
        "will be inserted at the position of the asterisk, or appended to the list " +
        "if there is no asterisk. The first variable will become the outermost loop, " +
        "and the last one the innermost loop. Example: " +
        "`repetition,numHosts,*,iaTime`.");
    public static final ConfigOption CFGID_LOAD_LIBS = addGlobalOption(
        "load-libs", CFG_FILENAMES, null,
        "A space-separated list of dynamic libraries to be loaded on startup. The " +
        "libraries should be given without the `.dll` or `.so` suffix -- that will " +
        "be automatically appended.");
    public static final ConfigOption CFGID_MAX_MODULE_NESTING = addPerRunOption(
        "max-module-nesting", CFG_INT, "50",
        "The maximum allowed depth of submodule nesting. This is used to catch " +
        "accidental infinite recursions in NED.");
    public static final ConfigOption CFGID_MEASUREMENT_LABEL = addPerRunOption(
        "measurement-label", CFG_STRING, "${iterationvars}",
        "Identifies the measurement within the experiment. This string gets recorded " +
        "into result files, and may be referred to during result analysis.");
    public static final ConfigOption CFGID_MODULE_EVENTLOG_RECORDING = addPerObjectOption(
        "module-eventlog-recording", KIND_SIMPLE_MODULE, CFG_BOOL, "true",
        "Enables recording events on a per module basis. This is meaningful for " +
        "simple modules only. Usage: " +
        "`<module-full-path>.module-eventlog-recording=true/false`. Examples: " +
        "`**.router[10..20].**.module-eventlog-recording = true`; " +
        "`**.module-eventlog-recording = false`");
    public static final ConfigOption CFGID_NED_PATH = addGlobalOption(
        "ned-path", CFG_PATH, null,
        "A semicolon-separated list of directories. The directories will be regarded " +
        "as roots of the NED package hierarchy, and all NED files will be loaded " +
        "from their subdirectory trees. This option is normally left empty, as the " +
        "OMNeT++ IDE sets the NED path automatically, and for simulations started " +
        "outside the IDE it is more convenient to specify it via a command-line " +
        "option or the NEDPATH environment variable.");
    public static final ConfigOption CFGID_NETWORK = addPerRunOption(
        "network", CFG_STRING, null,
        "The name of the network to be simulated.  The package name can be omitted " +
        "if the ini file is in the same directory as the NED file that contains the " +
        "network.");
    public static final ConfigOption CFGID_NUM_RNGS = addPerRunOption(
        "num-rngs", CFG_INT, "1",
        "The number of random number generators.");
    public static final ConfigOption CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ = addGlobalOption(
        "output-scalar-db-commit-freq", CFG_INT, "100000",
        "Used with SqliteOutputScalarManager: COMMIT every n INSERTs.");
    public static final ConfigOption CFGID_OUTPUT_SCALAR_FILE = addPerRunOption(
        "output-scalar-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sca",
        "Name for the output scalar file.");
    public static final ConfigOption CFGID_OUTPUT_SCALAR_FILE_APPEND = addPerRunOption(
        "output-scalar-file-append", CFG_BOOL, "false",
        "What to do when the output scalar file already exists: append to it " +
        "(OMNeT++ 3.x behavior), or delete it and begin a new file (default).");
    public static final ConfigOption CFGID_OUTPUT_SCALAR_PRECISION = addPerRunOption(
        "output-scalar-precision", CFG_INT, "14",
        "The number of significant digits for recording data into the output scalar " +
        "file. The maximum value is ~15 (IEEE double precision). This has no effect " +
        "on SQLite recording, as it stores values as 8-byte IEEE floating point " +
        "numbers.");
    public static final ConfigOption CFGID_OUTPUT_VECTOR_DB_INDEXING = addGlobalOption(
        "output-vector-db-indexing", CFG_CUSTOM, "skip",
        "Whether and when to add an index to the 'vectordata' table in SQLite output " +
        "vector files. Possible values: skip, ahead, after");
    public static final ConfigOption CFGID_OUTPUT_VECTOR_FILE = addPerRunOption(
        "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.vec",
        "Name for the output vector file.");
    public static final ConfigOption CFGID_OUTPUT_VECTOR_FILE_APPEND = addPerRunOption(
        "output-vector-file-append", CFG_BOOL, "false",
        "What to do when the output vector db already exists: append to it, or " +
        "delete it and begin a new file (default). Note: `cIndexedFileOutputVectorManager` " + 
        "currently does not support appending.");
    public static final ConfigOption CFGID_OUTPUT_VECTOR_PRECISION = addPerRunOption(
        "output-vector-precision", CFG_INT, "14",
        "The number of significant digits for recording data into the output vector " +
        "file. The maximum value is ~15 (IEEE double precision). This setting has no " +
        "effect on SQLite recording (it stores values as 8-byte IEEE floating point " +
        "numbers), and for the \"time\" column which is represented as fixed-point " +
        "numbers and always get recorded precisely.");
    public static final ConfigOption CFGID_OUTPUT_VECTORS_MEMORY_LIMIT = addPerRunOptionU(
        "output-vectors-memory-limit", "B", "16MiB",
        "Total memory that can be used for buffering output vectors. Larger values " +
        "produce less fragmented vector files (i.e. cause vector data to be grouped " +
        "into larger chunks), and therefore allow more efficient processing later. " +
        "There is also a per-vector limit, see `**.vector-buffer`.");
    public static final ConfigOption CFGID_OUTPUTSCALARMANAGER_CLASS = addGlobalOption(
        "outputscalarmanager-class", CFG_STRING, "omnetpp::envir::cFileOutputScalarManager",
        "Part of the Envir plugin mechanism: selects the output scalar manager class " +
        "to be used to record data passed to recordScalar(). The class has to " +
        "implement the `cIOutputScalarManager` interface.");
    public static final ConfigOption CFGID_OUTPUTVECTORMANAGER_CLASS = addGlobalOption(
        "outputvectormanager-class", CFG_STRING, "omnetpp::envir::cIndexedFileOutputVectorManager",
        "Part of the Envir plugin mechanism: selects the output vector manager class " +
        "to be used to record data from output vectors. The class has to implement " +
        "the `cIOutputVectorManager` interface.");
    public static final ConfigOption CFGID_PARALLEL_SIMULATION = addGlobalOption(
        "parallel-simulation", CFG_BOOL, "false",
        "Enables parallel distributed simulation.");
    public static final ConfigOption CFGID_PARAM_RECORD_AS_SCALAR = addPerObjectOption(
        "param-record-as-scalar", KIND_PARAMETER, CFG_BOOL, "false",
        "Applicable to module parameters: specifies whether the module parameter " +
        "should be recorded into the output scalar file. Set it for parameters whose " +
        "value you will need for result analysis.");
    public static final ConfigOption CFGID_PARSIM_COMMUNICATIONS_CLASS = addGlobalOption(
        "parsim-communications-class", CFG_STRING, "omnetpp::cFileCommunications",
        "If `parallel-simulation=true`, it selects the class that implements " +
        "communication between partitions. The class must implement the " +
        "`cParsimCommunications` interface.");
    public static final ConfigOption CFGID_PARSIM_DEBUG = addGlobalOption(
        "parsim-debug", CFG_BOOL, "true",
        "With `parallel-simulation=true`: turns on printing of log messages from the " +
        "parallel simulation code.");
    public static final ConfigOption CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX = addGlobalOption(
        "parsim-filecommunications-prefix", CFG_STRING, "comm/",
        "When `cFileCommunications` is selected as parsim communications class: " +
        "specifies the prefix (directory+potential filename prefix) for creating the " +
        "files for cross-partition messages.");
    public static final ConfigOption CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ = addGlobalOption(
        "parsim-filecommunications-preserve-read", CFG_BOOL, "false",
        "When `cFileCommunications` is selected as parsim communications class: " +
        "specifies that consumed files should be moved into another directory " +
        "instead of being deleted.");
    public static final ConfigOption CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX = addGlobalOption(
        "parsim-filecommunications-read-prefix", CFG_STRING, "comm/read/",
        "When `cFileCommunications` is selected as parsim communications class: " +
        "specifies the prefix (directory) where files will be moved after having " +
        "been consumed.");
    public static final ConfigOption CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE = addGlobalOption(
        "parsim-idealsimulationprotocol-tablesize", CFG_INT, "100000",
        "When `cIdealSimulationProtocol` is selected as parsim synchronization " +
        "class: specifies the memory buffer size for reading the ISP event trace " +
        "file.");
    public static final ConfigOption CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER = addGlobalOption(
        "parsim-mpicommunications-mpibuffer", CFG_INT, null,
        "When `cMPICommunications` is selected as parsim communications class: " +
        "specifies the size of the MPI communications buffer. The default is to " +
        "calculate a buffer size based on the number of partitions.");
    public static final ConfigOption CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX = addGlobalOption(
        "parsim-namedpipecommunications-prefix", CFG_STRING, "comm/",
        "When `cNamedPipeCommunications` is selected as parsim communications class: " +
        "selects the prefix (directory+potential filename prefix) where name pipes " +
        "are created in the file system.");
    public static final ConfigOption CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS = addGlobalOption(
        "parsim-nullmessageprotocol-laziness", CFG_DOUBLE, "0.5",
        "When `cNullMessageProtocol` is selected as parsim synchronization class: " +
        "specifies the laziness of sending null messages. Values in the range " +
        "`[0,1)` are accepted. Laziness=0 causes null messages to be sent out " +
        "immediately as a new EOT is learned, which may result in excessive null " +
        "message traffic.");
    public static final ConfigOption CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS = addGlobalOption(
        "parsim-nullmessageprotocol-lookahead-class", CFG_STRING, "cLinkDelayLookahead",
        "When `cNullMessageProtocol` is selected as parsim synchronization class: " +
        "specifies the C++ class that calculates lookahead. The class should " +
        "subclass from `cNMPLookahead`.");
    public static final ConfigOption CFGID_PARSIM_SYNCHRONIZATION_CLASS = addGlobalOption(
        "parsim-synchronization-class", CFG_STRING, "omnetpp::cNullMessageProtocol",
        "If `parallel-simulation=true`, it selects the parallel simulation " +
        "algorithm. The class must implement the `cParsimSynchronizer` interface.");
    public static final ConfigOption CFGID_PARTITION_ID = addPerObjectOption(
        "partition-id", KIND_MODULE, CFG_STRING, null,
        "With parallel simulation: in which partition the module should be " +
        "instantiated. Specify numeric partition ID, or a comma-separated list of " +
        "partition IDs for compound modules that span across multiple partitions. " +
        "Ranges (`5..9`) and `*` (=all) are accepted too.");
    public static final ConfigOption CFGID_PRINT_UNDISPOSED = addGlobalOption(
        "print-undisposed", CFG_BOOL, "true",
        "Whether to report objects left (that is, not deallocated by simple module " +
        "destructors) after network cleanup.");
    public static final ConfigOption CFGID_QTENV_DEFAULT_CONFIG = addGlobalOption(
        "qtenv-default-config", CFG_STRING, null,
        "Specifies which config Qtenv should set up automatically on startup. The " +
        "default is to ask the user.");
    public static final ConfigOption CFGID_QTENV_DEFAULT_RUN = addGlobalOption(
        "qtenv-default-run", CFG_STRING, null,
        "Specifies which run (of the default config, see `qtenv-default-config`) " +
        "Qtenv should set up automatically on startup. A run filter is also " +
        "accepted. The default is to ask the user.");
    public static final ConfigOption CFGID_QTENV_EXTRA_STACK = addGlobalOptionU(
        "qtenv-extra-stack", "B", "80KiB",
        "Specifies the extra amount of stack that is reserved for each `activity()` " +
        "simple module when the simulation is run under Qtenv.");
    public static final ConfigOption CFGID_REAL_TIME_LIMIT = addPerRunOptionU(
        "real-time-limit", "s", null,
        "Stops the simulation after the specified amount of time has elapsed. The " +
        "default is no limit. Note: To reduce per-event overhead, this time limit is " +
        "only checked every N events (by default, N=1024).");
    public static final ConfigOption CFGID_REALTIMESCHEDULER_SCALING = addGlobalOption(
        "realtimescheduler-scaling", CFG_DOUBLE, null,
        "When cRealTimeScheduler is selected as scheduler class: ratio of simulation " +
        "time to real time. For example, `realtimescheduler-scaling=2` will cause " +
        "simulation time to progress twice as fast as runtime.");
    public static final ConfigOption CFGID_RECORD_EVENTLOG = addPerRunOption(
        "record-eventlog", CFG_BOOL, "false",
        "Enables recording an eventlog file, which can be later visualized on a " +
        "sequence chart. See `eventlog-file` option too.");
    public static final ConfigOption CFGID_REPEAT = addPerRunOption(
        "repeat", CFG_INT, "1",
        "For scenarios. Specifies how many replications should be done with the same " +
        "parameters (iteration variables). This is typically used to perform " +
        "multiple runs with different random number seeds. The loop variable is " +
        "available as `${repetition}`. See also: `seed-set` key.");
    public static final ConfigOption CFGID_REPLICATION_LABEL = addPerRunOption(
        "replication-label", CFG_STRING, "#${repetition}",
        "Identifies one replication of a measurement (see `repeat` and " +
        "`measurement-label` options as well). This string gets recorded into result " +
        "files, and may be referred to during result analysis.");
    public static final ConfigOption CFGID_RESULT_DIR = addPerRunOption(
        "result-dir", CFG_STRING, "results",
        "Value for the `${resultdir}` variable, which is used as the default " +
        "directory for result files (output vector file, output scalar file, " +
        "eventlog file, etc.)");
    public static final ConfigOption CFGID_RESULT_RECORDING_MODES = addPerObjectOption(
        "result-recording-modes", KIND_STATISTIC, CFG_STRING, "default",
        "Defines how to calculate results from the matching `@statistic`.\n" +
        "Usage: " +
        "`<module-full-path>.<statistic-name>.result-recording-modes=<modes>`. " +
        "Special values: `default`, `all`: they select the modes listed in the " +
        "`record` key of `@statistic`; all selects all of them, default selects the " +
        "non-optional ones (i.e. excludes the ones that end in a question mark). " +
        "Example values: `vector`, `count`, `last`, `sum`, `mean`, `min`, `max`, " +
        "`timeavg`, `stats`, `histogram`. More than one values are accepted, " +
        "separated by commas. Expressions are allowed. Items prefixed with `-` get " +
        "removed from the list. Example: " +
        "`**.queueLength.result-recording-modes=default,-vector,+timeavg`");
    public static final ConfigOption CFGID_RNG_n = addPerObjectOption(
        "rng-%", KIND_COMPONENT, CFG_INT, null,
        "Maps a module-local RNG to one of the global RNGs. Example: " +
        "`**.gen.rng-1=3` maps the local RNG 1 of modules matching `**.gen` to the " +
        "global RNG 3. The value may be an expression, with the `index` and " +
        "`ancestorIndex()` operators being potentially very useful. The default is " +
        "one-to-one mapping, i.e. RNG k of all modules refer to the global RNG k " +
        "(`for k=0..num-rngs-1`).\n" +
        "Usage: `<module-full-path>.rng-<local-index>=<global-index>`. Examples: " +
        "`**.mac.rng-0=1; **.source[*].rng-0=index`");
    public static final ConfigOption CFGID_RNG_CLASS = addPerRunOption(
        "rng-class", CFG_STRING, "omnetpp::cMersenneTwister",
        "The random number generator class to be used. It can be `cMersenneTwister`, " +
        "`cLCG32`, `cAkaroaRNG`, or you can use your own RNG class (it must be " +
        "subclassed from `cRNG`).");
    public static final ConfigOption CFGID_RUNNUMBER_WIDTH = addPerRunOption(
        "runnumber-width", CFG_INT, "0",
        "Setting a nonzero value will cause the `$runnumber` variable to get padded " +
        "with leading zeroes to the given length.");
    public static final ConfigOption CFGID_SCALAR_RECORDING = addPerObjectOption(
        "scalar-recording", KIND_SCALAR, CFG_BOOL, "true",
        "Whether the matching output scalars and statistic objects should be " +
        "recorded.\n" +
        "Usage: `<module-full-path>.<scalar-name>.scalar-recording=true/false`. To " +
        "enable/disable individual recording modes for a @statistic (those added via " +
        "the `record=...` key of `@statistic` or the `**.result-recording-modes=...` " +
        "config option), use `<statistic-name>:<mode>` for `<scalar-name>`, and make " +
        "sure the `@statistic` as a whole is not disabled with " +
        "`**.<statistic-name>.statistic-recording=false`.\n" +
        "Example: `**.ping.roundTripTime:stddev.scalar-recording=false`");
    public static final ConfigOption CFGID_SCHEDULER_CLASS = addGlobalOption(
        "scheduler-class", CFG_STRING, "omnetpp::cSequentialScheduler",
        "Part of the Envir plugin mechanism: selects the scheduler class. This " +
        "plugin interface allows for implementing real-time, hardware-in-the-loop, " +
        "distributed and distributed parallel simulation. The class has to implement " +
        "the `cScheduler` interface.");
    public static final ConfigOption CFGID_SECTIONBASEDCONFIG_CONFIGREADER_CLASS = addGlobalOption(
        "sectionbasedconfig-configreader-class", CFG_STRING, null,
        "When `configuration-class=SectionBasedConfiguration`: selects the " +
        "configuration reader C++ class, which must subclass from " +
        "`cConfigurationReader`.");
    public static final ConfigOption CFGID_SEED_n_LCG32 = addPerRunOption(
        "seed-%-lcg32", CFG_INT, null,
        "When cLCG32 is selected as random number generator: seed for the kth RNG. " +
        "(Substitute k for '%' in the key.)");
    public static final ConfigOption CFGID_SEED_n_MT = addPerRunOption(
        "seed-%-mt", CFG_INT, null,
        "When Mersenne Twister is selected as random number generator (default): " +
        "seed for RNG number k. (Substitute k for '%' in the key.)");
    public static final ConfigOption CFGID_SEED_n_MT_Pn = addPerRunOption(
        "seed-%-mt-p%", CFG_INT, null,
        "With parallel simulation: When Mersenne Twister is selected as random " +
        "number generator (default): seed for RNG number k in partition number p. " +
        "(Substitute k for the first '%' in the key, and p for the second.)");
    public static final ConfigOption CFGID_SEED_SET = addPerRunOption(
        "seed-set", CFG_INT, "${runnumber}",
        "Selects the kth set of automatic random number seeds for the simulation. " +
        "Meaningful values include `${repetition}` which is the repeat loop counter " +
        "(see `repeat` option), and `${runnumber}`.");
    public static final ConfigOption CFGID_SIM_TIME_LIMIT = addPerRunOptionU(
        "sim-time-limit", "s", null,
        "Stops the simulation when simulation time reaches the given limit. The " +
        "default is no limit.");
    public static final ConfigOption CFGID_SIMTIME_RESOLUTION = addGlobalOption(
        "simtime-resolution", CFG_CUSTOM, "ps",
        "Sets the resolution for the 64-bit fixed-point simulation time " +
        "representation. Accepted values are: second-or-smaller time units (`s`, " +
        "`ms`, `us`, `ns`, `ps`, `fs` or as), power-of-ten multiples of such units " +
        "(e.g. 100ms), and base-10 scale exponents in the -18..0 range. The maximum " +
        "representable simulation time depends on the resolution. The default is " +
        "picosecond resolution, which offers a range of ~110 days.");
    public static final ConfigOption CFGID_SIMTIME_SCALE = addGlobalOption(
        "simtime-scale", CFG_INT, "-12",
        "DEPRECATED in favor of simtime-resolution. Sets the scale exponent, and " +
        "thus the resolution of time for the 64-bit fixed-point simulation time " +
        "representation. Accepted values are -18..0; for example, -6 selects " +
        "microsecond resolution. -12 means picosecond resolution, with a maximum " +
        "simtime of ~110 days.");
    public static final ConfigOption CFGID_SNAPSHOT_FILE = addPerRunOption(
        "snapshot-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sna",
        "Name of the snapshot file.");
    public static final ConfigOption CFGID_SNAPSHOTMANAGER_CLASS = addGlobalOption(
        "snapshotmanager-class", CFG_STRING, "omnetpp::envir::cFileSnapshotManager",
        "Part of the Envir plugin mechanism: selects the class to handle streams to " +
        "which snapshot() writes its output. The class has to implement the " +
        "`cISnapshotManager` interface.");
    public static final ConfigOption CFGID_STATISTIC_RECORDING = addPerObjectOption(
        "statistic-recording", KIND_STATISTIC, CFG_BOOL, "true",
        "Whether the matching `@statistic` should be recorded. This option lets one " +
        "completely disable all recording from a @statistic. Disabling a " +
        "`@statistic` this way is more efficient than specifying " +
        "`**.scalar-recording=false` and `**.vector-recording=false` together.\n" +
        "Usage: " +
        "`<module-full-path>.<statistic-name>.statistic-recording=true/false`.\n" +
        "Example: `**.ping.roundTripTime.statistic-recording=false`");
    public static final ConfigOption CFGID_TKENV_DEFAULT_CONFIG = addGlobalOption(
        "tkenv-default-config", CFG_STRING, null,
        "Specifies which config Tkenv should set up automatically on startup. The " +
        "default is to ask the user.");
    public static final ConfigOption CFGID_TKENV_DEFAULT_RUN = addGlobalOption(
        "tkenv-default-run", CFG_INT, "0",
        "Specifies which run (of the default config, see `tkenv-default-config`) " +
        "Tkenv should set up automatically on startup. The default is to ask the " +
        "user.");
    public static final ConfigOption CFGID_TKENV_EXTRA_STACK = addGlobalOptionU(
        "tkenv-extra-stack", "B", "48KiB",
        "Specifies the extra amount of stack that is reserved for each `activity()` " +
        "simple module when the simulation is run under Tkenv.");
    public static final ConfigOption CFGID_TKENV_PLUGIN_PATH = addGlobalOption(
        "tkenv-plugin-path", CFG_PATH, null,
        "Specifies the search path for Tkenv plugins. Tkenv plugins are .tcl files " +
        "that get evaluated on startup.");
    public static final ConfigOption CFGID_TOTAL_STACK = addGlobalOptionU(
        "total-stack", "B", null,
        "Specifies the maximum memory for `activity()` simple module stacks. You " +
        "need to increase this value if you get a \"Cannot allocate coroutine stack\" " +
        "error.");
    public static final ConfigOption CFGID_TYPENAME = addPerObjectOption(
        "typename", KIND_UNSPECIFIED_TYPE, CFG_STRING, null,
        "Specifies type for submodules and channels declared with 'like <>'.");
    public static final ConfigOption CFGID_USER_INTERFACE = addGlobalOption(
        "user-interface", CFG_STRING, null,
        "Selects the user interface to be started. Possible values are Cmdenv, Tkenv " +
        "and Qtenv. This option is normally left empty, as it is more convenient to " +
        "specify the user interface via a command-line option or the IDE's Run and " +
        "Debug dialogs. New user interfaces can be defined by subclassing " +
        "`cRunnableEnvir`.");
    public static final ConfigOption CFGID_VECTOR_BUFFER = addPerObjectOptionU(
        "vector-buffer", KIND_VECTOR, "B", "1MiB",
        "For output vectors: the maximum per-vector buffer space used for storing " +
        "values before writing them out as a block into the output vector file. " +
        "There is also a total limit, see `output-vectors-memory-limit`.\n" +
        "Usage: `<module-full-path>.<vector-name>.vector-buffer=<amount>`.");
    public static final ConfigOption CFGID_VECTOR_RECORD_EVENTNUMBERS = addPerObjectOption(
        "vector-record-eventnumbers", KIND_VECTOR, CFG_BOOL, "true",
        "Whether to record event numbers for an output vector. (Values and " +
        "timestamps are always recorded.) Event numbers are needed by the Sequence " +
        "Chart Tool, for example.\n" +
        "Usage: " +
        "`<module-full-path>.<vector-name>.vector-record-eventnumbers=true/false`.\n" +
        "Example: `**.ping.roundTripTime:vector.vector-record-eventnumbers=false`");
    public static final ConfigOption CFGID_VECTOR_RECORDING = addPerObjectOption(
        "vector-recording", KIND_VECTOR, CFG_BOOL, "true",
        "Whether data written into an output vector should be recorded.\n" +
        "Usage: `<module-full-path>.<vector-name>.vector-recording=true/false`. To " +
        "control vector recording from a `@statistic`, use `<statistic-name>:vector " +
        "for <vector-name>`. Example: " +
        "`**.ping.roundTripTime:vector.vector-recording=false`");
    public static final ConfigOption CFGID_VECTOR_RECORDING_INTERVALS = addPerObjectOption(
        "vector-recording-intervals", KIND_VECTOR, CFG_CUSTOM, null,
        "Allows one to restrict recording of an output vector to one or more " +
        "simulation time intervals. Usage: " +
        "`<module-full-path>.<vector-name>.vector-recording-intervals=<intervals>`. " +
        "The syntax for `<intervals>` is: `[<from>]..[<to>],...` That is, both start " +
        "and end of an interval are optional, and intervals are separated by " +
        "comma.\n" +
        "Example: `**.roundTripTime:vector.vector-recording-intervals=..100, " +
        "200..400, 900..`");
    public static final ConfigOption CFGID_WARMUP_PERIOD = addPerRunOptionU(
        "warmup-period", "s", null,
        "Length of the initial warm-up period. When set, results belonging to the " +
        "first x seconds of the simulation will not be recorded into output vectors, " +
        "and will not be counted into output scalars (see option " +
        "`**.result-recording-modes`). This option is useful for steady-state " +
        "simulations. The default is 0s (no warmup period). Note that models that " +
        "compute and record scalar results manually (via `recordScalar()`) will not " +
        "automatically obey this setting.");
    public static final ConfigOption CFGID_WARNINGS = addPerRunOption(
        "warnings", CFG_BOOL, "true",
        "Enables warnings.");
    public static final ConfigOption CFGID_WITH_AKAROA = addPerObjectOption(
        "with-akaroa", KIND_VECTOR, CFG_BOOL, "false",
        "Whether the output vector should be under Akaroa control.");

    public static final String CFGVAR_RUNID = addConfigVariable("runid", "A reasonably globally unique identifier for the run, produced by concatenating the configuration name, run number, date/time, etc.");
    public static final String CFGVAR_INIFILE = addConfigVariable("inifile", "Name of the (primary) inifile");
    public static final String CFGVAR_CONFIGNAME = addConfigVariable("configname", "Name of the active configuration");
    public static final String CFGVAR_RUNNUMBER = addConfigVariable("runnumber", "Sequence number of the current run within all runs in the active configuration");
    public static final String CFGVAR_NETWORK = addConfigVariable("network", "Value of the `network` configuration option");
    public static final String CFGVAR_EXPERIMENT = addConfigVariable("experiment", "Value of the `experiment-label` configuration option");
    public static final String CFGVAR_MEASUREMENT = addConfigVariable("measurement", "Value of the `measurement-label` configuration option");
    public static final String CFGVAR_REPLICATION = addConfigVariable("replication", "Value of the `replication-label` configuration option");
    public static final String CFGVAR_PROCESSID = addConfigVariable("processid", "PID of the simulation process");
    public static final String CFGVAR_DATETIME = addConfigVariable("datetime", "Date and time the simulation run was started");
    public static final String CFGVAR_RESULTDIR = addConfigVariable("resultdir", "Value of the `result-dir` configuration option");
    public static final String CFGVAR_REPETITION = addConfigVariable("repetition", "The iteration number in `0..N-1`, where `N` is the value of the `repeat` configuration option");
    public static final String CFGVAR_SEEDSET = addConfigVariable("seedset", "Value of the `seed-set` configuration option");
    public static final String CFGVAR_ITERATIONVARS = addConfigVariable("iterationvars", "Concatenation of all user-defined iteration variables in `name=value` form");
    public static final String CFGVAR_ITERATIONVARSF = addConfigVariable("iterationvarsf", "Like ${iterationvars}, but sanitized for use as part of file names");

    static {
        EXTENDS = CFGID_EXTENDS.getName();
        TYPENAME = CFGID_TYPENAME.getName();

        // if you've just updated the config registry with "opp_run -h jconfig" and now you're
        // getting compile errors below, you've accidentally removed some entries due to
        // not having Akaroa or MPI configured ;-)
        CFGID_WITH_AKAROA.toString();
        CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER.toString();
    }

    public static final String[] SIMTIME_SCALE_CHOICES = new String[] {
        "0", "-3", "-6", "-9", "-12", "-15", "-18"
    };

    public static final String[] SIMTIME_RESOLUTION_CHOICES = new String[] {
        "s",
        "100ms", "10ms", "ms",
        "100us", "10us", "us",
        "100ns", "10ns", "ns",
        "100ps", "10ps", "ps",
        "100fs", "10fs", "fs",
        "100as", "10as", "as",
    };

    public static final String[] LOGLEVEL_CHOICES = new String[] {
        "OFF", "FATAL", "ERROR", "WARN", "INFO", "DETAILS", "DEBUG", "TRACE"
    };

    public static final Map<String,String> OBSOLETE_OPTIONS;  // (key, error message) pairs

    static {
        HashMap<String,String> map = new HashMap<>();
        map.put("type-name", "Configuration option \"type-name\" has been renamed to \"typename\", please update the ini file");
        map.put("cmdenv-module-messages", "Obsolete configuration option, use **.cmdenv-log-level instead");
        map.put("cmdenv-message-trace", "Obsolete configuration option");
        map.put("cmdenv-ev-output", "Obsolete configuration option, use **.cmdenv-log-level instead");
        OBSOLETE_OPTIONS = map;
    }

}
