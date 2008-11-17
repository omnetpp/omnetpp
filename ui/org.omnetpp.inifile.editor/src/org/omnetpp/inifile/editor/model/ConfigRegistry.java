package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_BOOL;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_CUSTOM;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_DOUBLE;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_FILENAME;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_FILENAMES;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_PATH;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_INT;
import static org.omnetpp.inifile.editor.model.ConfigKey.DataType.CFG_STRING;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import org.omnetpp.inifile.editor.model.ConfigKey.DataType;

/**
 * Contains the list of supported configuration keys.
 *
 * This must be kept in sync with the C++ code. Procedure to do it:
 *   1. run any simulation executable with the "-q jconfig" option,
 *       and copy/paste the Java code it generates into this file
 *   2. run svn diff to see the damage.
 *   3. check that the form editor covers all config keys:
 *      open InifileFormEditor.java, and temporarily set
 *      DUMP_FORGOTTEN_CONFIG_KEYS = true. Then run the IDE and watch
 *      the output.
 *
 * @author Andras
 */
public class ConfigRegistry {
    /** Name of the [General] section */
    public static final String GENERAL = "General";

    /** Name prefix for the [Section <name>] sections; includes a trailing space. */
    public static final String CONFIG_ = "Config ";

    /** Name of the "extends=" config key */
    public static final String EXTENDS; // initialized at the bottom of this file

    /** Inifile value keyword*/
    public static final String DEFAULT = "default";

    /** Inifile value keyword*/
    public static final String ASK = "ask";

    private static Map<String, ConfigKey> entries = new HashMap<String, ConfigKey>();
    private static Map<String, ConfigKey> perObjectEntries = new HashMap<String, ConfigKey>();
    private static Map<String, String> configVars = new LinkedHashMap<String, String>(); // preserve order

    private static ConfigKey addGlobalEntry(String name, DataType type, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, false, true, type, null, defaultValue, description);
        entries.put(name, e);
        return e;
    }

    private static ConfigKey addGlobalEntryU(String name, String unit, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, false, true, CFG_DOUBLE, unit, defaultValue, description);
        entries.put(name, e);
        return e;
    }

    private static ConfigKey addPerRunEntry(String name, DataType type, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, false, false, type, null, defaultValue, description);
        entries.put(name, e);
        return e;
    }

    private static ConfigKey addPerRunEntryU(String name, String unit, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, false, false, CFG_DOUBLE, unit, defaultValue, description);
        entries.put(name, e);
        return e;
    }

    private static ConfigKey addPerObjectEntry(String name, DataType type, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, true, false, type, null, defaultValue, description);
        perObjectEntries.put(name, e);
        return e;
    }

    @SuppressWarnings("unused")
    private static ConfigKey addPerObjectEntryU(String name, String unit, String defaultValue, String description) {
        ConfigKey e = new ConfigKey(name, true, false, CFG_DOUBLE, unit, defaultValue, description);
        perObjectEntries.put(name, e);
        return e;
    }

    private static ConfigKey lookupWildcardConfigKey(Map<String, ConfigKey> map, String key) {
        // config keys may contain wildcards: '*' (any string) and '%' (numeric string)
        for (ConfigKey e : map.values()) {
            if (e.containsWildcard()) {
                String pattern = e.getKey().replace("*", ".+").replace("%", "[0-9]+");
                if (key.matches(pattern))
                    return e;
            }
        }
        return null;
    }

    /**
     * Returns the ConfigKey with the given name, or null. It also finds and returns
     * wildcard keys that match the given name, for example it will return "seed-%-mt"
     * for "seed-1-mt".
     */
    public static ConfigKey getEntry(String name) {
        ConfigKey e = entries.get(name);
        if (e == null)
            e = lookupWildcardConfigKey(entries, name);
        return e;
    }

    /**
     * Returns the per-object ConfigKey with the given name, or null. It also finds
     * and returns wildcard keys that match the given name, for example it will return
     * "rng-%" for "rng-1".
     */
    public static ConfigKey getPerObjectEntry(String name) {
        ConfigKey e = perObjectEntries.get(name);
        if (e == null)
            e = lookupWildcardConfigKey(perObjectEntries, name);
        return e;
    }

    public static ConfigKey[] getEntries() {
        return (ConfigKey[]) entries.values().toArray(new ConfigKey[entries.size()]);
    }

    public static ConfigKey[] getPerObjectEntries() {
        return (ConfigKey[]) perObjectEntries.values().toArray(new ConfigKey[perObjectEntries.size()]);
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

    /*=== The following code has been generated by "some-simulation-program -h jconfig" ===*/

    public static final ConfigKey CFGID_CMDENV_AUTOFLUSH = addPerRunEntry(
        "cmdenv-autoflush", CFG_BOOL, "false",
        "Call fflush(stdout) after each event banner or status update; affects both " +
        "express and normal mode. Turning on autoflush may have a performance " +
        "penalty, but it can be useful with printf-style debugging for tracking down " +
        "program crashes.");
    public static final ConfigKey CFGID_CMDENV_CONFIG_NAME = addGlobalEntry(
        "cmdenv-config-name", CFG_STRING, null,
        "Specifies the name of the configuration to be run (for a value `Foo', " +
        "section [Config Foo] will be used from the ini file). See also " +
        "cmdenv-runs-to-execute=. The -c command line option overrides this setting.");
    public static final ConfigKey CFGID_CMDENV_EV_OUTPUT = addPerObjectEntry(
        "cmdenv-ev-output", CFG_BOOL, "true",
        "When cmdenv-express-mode=false: whether Cmdenv should print debug messages " +
        "(ev<<) from the selected modules.");
    public static final ConfigKey CFGID_CMDENV_EVENT_BANNER_DETAILS = addPerRunEntry(
        "cmdenv-event-banner-details", CFG_BOOL, "false",
        "When cmdenv-express-mode=false: print extra information after event " +
        "banners.");
    public static final ConfigKey CFGID_CMDENV_EVENT_BANNERS = addPerRunEntry(
        "cmdenv-event-banners", CFG_BOOL, "true",
        "When cmdenv-express-mode=false: turns printing event banners on/off.");
    public static final ConfigKey CFGID_CMDENV_EXPRESS_MODE = addPerRunEntry(
        "cmdenv-express-mode", CFG_BOOL, "true",
        "Selects ``normal'' (debug/trace) or ``express'' mode.");
    public static final ConfigKey CFGID_CMDENV_EXTRA_STACK = addGlobalEntryU(
        "cmdenv-extra-stack", "B", "8KB",
        "Specifies the extra amount of stack that is reserved for each activity() " +
        "simple module when the simulation is run under Cmdenv.");
    public static final ConfigKey CFGID_CMDENV_INTERACTIVE = addGlobalEntry(
        "cmdenv-interactive", CFG_BOOL, "false",
        "Defines what Cmdenv should do when the model contains unassigned " +
        "parameters. In interactive mode, it asks the user. In non-interactive mode " +
        "(which is more suitable for batch execution), Cmdenv stops with an error.");
    public static final ConfigKey CFGID_CMDENV_MESSAGE_TRACE = addPerRunEntry(
        "cmdenv-message-trace", CFG_BOOL, "false",
        "When cmdenv-express-mode=false: print a line per message sending (by " +
        "send(),scheduleAt(), etc) and delivery on the standard output.");
    public static final ConfigKey CFGID_CMDENV_MODULE_MESSAGES = addPerRunEntry(
        "cmdenv-module-messages", CFG_BOOL, "true",
        "When cmdenv-express-mode=false: turns printing module ev<< output on/off.");
    public static final ConfigKey CFGID_CMDENV_OUTPUT_FILE = addGlobalEntry(
        "cmdenv-output-file", CFG_FILENAME, null,
        "When a filename is specified, Cmdenv redirects standard output into the " +
        "given file. This is especially useful with parallel simulation. See the " +
        "`fname-append-host' option as well.");
    public static final ConfigKey CFGID_CMDENV_PERFORMANCE_DISPLAY = addPerRunEntry(
        "cmdenv-performance-display", CFG_BOOL, "true",
        "When cmdenv-express-mode=true: print detailed performance information. " +
        "Turning it on results in a 3-line entry printed on each update, containing " +
        "ev/sec, simsec/sec, ev/simsec, number of messages created/still " +
        "present/currently scheduled in FES.");
    public static final ConfigKey CFGID_CMDENV_RUNS_TO_EXECUTE = addGlobalEntry(
        "cmdenv-runs-to-execute", CFG_STRING, null,
        "Specifies which runs to execute from the selected configuration (see " +
        "cmdenv-config-name=). It accepts a comma-separated list of run numbers or " +
        "run number ranges, e.g. 1,3..4,7..9. If the value is missing, Cmdenv " +
        "executes all runs in the selected configuration. The -r command line option " +
        "overrides this setting.");
    public static final ConfigKey CFGID_CMDENV_STATUS_FREQUENCY = addPerRunEntry(
        "cmdenv-status-frequency", CFG_INT, "100000",
        "When cmdenv-express-mode=true: print status update every n events. Typical " +
        "values are 100,000...1,000,000.");
    public static final ConfigKey CFGID_CONFIGURATION_CLASS = addGlobalEntry(
        "configuration-class", CFG_STRING, null,
        "Part of the Envir plugin mechanism: selects the class from which all " +
        "configuration information will be obtained. This option lets you replace " +
        "omnetpp.ini with some other implementation, e.g. database input. The " +
        "simulation program still has to bootstrap from an omnetpp.ini (which " +
        "contains the configuration-class setting). The class should implement the " +
        "cConfiguration interface.");
    public static final ConfigKey CFGID_CONSTRAINT = addPerRunEntry(
        "constraint", CFG_STRING, null,
        "For scenarios. Contains an expression that iteration variables (${} syntax) " +
        "must satisfy for that simulation to run. Example: $i < $j+1.");
    public static final ConfigKey CFGID_CPU_TIME_LIMIT = addPerRunEntryU(
        "cpu-time-limit", "s", null,
        "Stops the simulation when CPU usage has reached the given limit. The " +
        "default is no limit.");
    public static final ConfigKey CFGID_DEBUG_ON_ERRORS = addGlobalEntry(
        "debug-on-errors", CFG_BOOL, "false",
        "When set to true, runtime errors will cause the simulation program to break " +
        "into the C++ debugger (if the simulation is running under one, or " +
        "just-in-time debugging is activated). Once in the debugger, you can view " +
        "the stack trace or examine variables.");
    public static final ConfigKey CFGID_DESCRIPTION = addPerRunEntry(
        "description", CFG_STRING, null,
        "Descriptive name for the given simulation configuration. Descriptions get " +
        "displayed in the run selection dialog.");
    public static final ConfigKey CFGID_EVENTLOG_FILE = addPerRunEntry(
        "eventlog-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.elog",
        "Name of the event log file to generate.");
    public static final ConfigKey CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN = addPerRunEntry(
        "eventlog-message-detail-pattern", CFG_CUSTOM, null,
        "A list of patterns separated by '|' character which will be used to write " +
        "message detail information into the event log for each message sent during " +
        "the simulation. The message detail will be presented in the sequence chart " +
        "tool. Each pattern starts with an object pattern optionally followed by ':' " +
        "character and a comma separated list of field patterns. In both patterns " +
        "and/or/not/* and various field match expressions can be used. The object " +
        "pattern matches to class name, the field pattern matches to field name by " +
        "default.\n" +
        "  EVENTLOG-MESSAGE-DETAIL-PATTERN := ( DETAIL-PATTERN '|' )* " +
        "DETAIL_PATTERN\n" +
        "  DETAIL-PATTERN := OBJECT-PATTERN [ ':' FIELD-PATTERNS ]\n" +
        "  OBJECT-PATTERN := MATCH-EXPRESSION\n" +
        "  FIELD-PATTERNS := ( FIELD-PATTERN ',' )* FIELD_PATTERN\n" +
        "  FIELD-PATTERN := MATCH-EXPRESSION\n" +
        "Examples (enter them without quotes):\n" +
        "  \"*\": captures all fields of all messages\n" +
        "  \"*Frame:*Address,*Id\": captures all fields named somethingAddress and " +
        "somethingId from messages of any class named somethingFrame\n" +
        "  \"MyMessage:declaredOn(MyMessage)\": captures instances of MyMessage " +
        "recording the fields declared on the MyMessage class\n" +
        "  \"*:(not declaredOn(cMessage) and not declaredOn(cNamedObject) and not " +
        "declaredOn(cObject))\": records user-defined fields from all messages");
    public static final ConfigKey CFGID_EVENTLOG_RECORDING_INTERVALS = addPerRunEntry(
        "eventlog-recording-intervals", CFG_CUSTOM, null,
        "Simulation time interval(s) when events should be recorded. Syntax: " +
        "[<from>]..[<to>],... That is, both start and end of an interval are " +
        "optional, and intervals are separated by comma. Example: ..10.2, 22.2..100, " +
        "233.3..");
    public static final ConfigKey CFGID_EXPERIMENT_LABEL = addPerRunEntry(
        "experiment-label", CFG_STRING, "${configname}",
        "Identifies the simulation experiment (which consists of several, " +
        "potentially repeated measurements). This string gets recorded into result " +
        "files, and may be referred to during result analysis.");
    public static final ConfigKey CFGID_EXTENDS = addPerRunEntry(
        "extends", CFG_STRING, null,
        "Name of the configuration this section is based on. Entries from that " +
        "section will be inherited and can be overridden. In other words, " +
        "configuration lookups will fall back to the base section.");
    public static final ConfigKey CFGID_FINGERPRINT = addPerRunEntry(
        "fingerprint", CFG_STRING, null,
        "The expected fingerprint of the simulation. When provided, a fingerprint " +
        "will be calculated from the simulation event times and other quantities " +
        "during simulation, and checked against the given one. Fingerprints are " +
        "suitable for crude regression tests. As fingerprints occasionally differ " +
        "across platforms, more than one fingerprint values can be specified here, " +
        "separated by spaces, and a match with any of them will be accepted. To " +
        "calculate the initial fingerprint, enter any dummy string (such as \"none\"), " +
        "and run the simulation.");
    public static final ConfigKey CFGID_FNAME_APPEND_HOST = addGlobalEntry(
        "fname-append-host", CFG_BOOL, "false",
        "Turning it on will cause the host name and process Id to be appended to the " +
        "names of output files (e.g. omnetpp.vec, omnetpp.sca). This is especially " +
        "useful with distributed simulation.");
    public static final ConfigKey CFGID_LOAD_LIBS = addGlobalEntry(
        "load-libs", CFG_FILENAMES, null,
        "A space-separated list of dynamic libraries to be loaded on startup. The " +
        "libraries should be given without the `.dll' or `.so' suffix -- that will " +
        "be automatically appended.");
    public static final ConfigKey CFGID_MAX_MODULE_NESTING = addPerRunEntry(
        "max-module-nesting", CFG_INT, "50",
        "The maximum allowed depth of submodule nesting. This is used to catch " +
        "accidental infinite recursions in NED.");
    public static final ConfigKey CFGID_MEASUREMENT_LABEL = addPerRunEntry(
        "measurement-label", CFG_STRING, "${iterationvars}",
        "Identifies the measurement within the experiment. This string gets recorded " +
        "into result files, and may be referred to during result analysis.");
    public static final ConfigKey CFGID_MODULE_EVENTLOG_RECORDING = addPerObjectEntry(
        "module-eventlog-recording", CFG_BOOL, "true",
        "Enables recording events on a per module basis. This is meaningful for " +
        "simple modules only. \n" +
        "Example:\n" +
        " **.router[10..20].**.module-eventlog-recording = true\n" +
        " **.module-eventlog-recording = false");
    public static final ConfigKey CFGID_NED_PATH = addGlobalEntry(
        "ned-path", CFG_PATH, null,
        "A semicolon-separated list of directories. The directories will be regarded " +
        "as roots of the NED package hierarchy, and all NED files will be loaded " +
        "from their subdirectory trees. This option is normally left empty, as the " +
        "OMNeT++ IDE sets the NED path automatically, and for simulations started " +
        "outside the IDE it is more convenient to specify it via a command-line " +
        "option or the NEDPATH environment variable.");
    public static final ConfigKey CFGID_NETWORK = addPerRunEntry(
        "network", CFG_STRING, null,
        "The name of the network to be simulated.  The package name can be omitted " +
        "if the ini file is in the same directory as the NED file that contains the " +
        "network.");
    public static final ConfigKey CFGID_NUM_RNGS = addPerRunEntry(
        "num-rngs", CFG_INT, "1",
        "The number of random number generators.");
    public static final ConfigKey CFGID_OUTPUT_SCALAR_FILE = addPerRunEntry(
        "output-scalar-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.sca",
        "Name for the output scalar file.");
    public static final ConfigKey CFGID_OUTPUT_SCALAR_FILE_APPEND = addPerRunEntry(
        "output-scalar-file-append", CFG_BOOL, "false",
        "What to do when the output scalar file already exists: append to it " +
        "(OMNeT++ 3.x behavior), or delete it and begin a new file (default).");
    public static final ConfigKey CFGID_OUTPUT_SCALAR_PRECISION = addPerRunEntry(
        "output-scalar-precision", CFG_INT, "14",
        "The number of significant digits for recording data into the output scalar " +
        "file. The maximum value is ~15 (IEEE double precision).");
    public static final ConfigKey CFGID_OUTPUT_VECTOR_FILE = addPerRunEntry(
        "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.vec",
        "Name for the output vector file.");
    public static final ConfigKey CFGID_OUTPUT_VECTOR_PRECISION = addPerRunEntry(
        "output-vector-precision", CFG_INT, "14",
        "The number of significant digits for recording data into the output vector " +
        "file. The maximum value is ~15 (IEEE double precision). This setting has no " +
        "effect on the \"time\" column of output vectors, which are represented as " +
        "fixed-point numbers and always get recorded precisely.");
    public static final ConfigKey CFGID_OUTPUT_VECTORS_MEMORY_LIMIT = addPerRunEntryU(
        "output-vectors-memory-limit", "B", "16MB",
        "Total memory that can be used for buffering output vectors. Larger values " +
        "produce less fragmented vector files (i.e. cause vector data to be grouped " +
        "into larger chunks), and therefore allow more efficient processing later.");
    public static final ConfigKey CFGID_OUTPUTSCALARMANAGER_CLASS = addGlobalEntry(
        "outputscalarmanager-class", CFG_STRING, "cFileOutputScalarManager",
        "Part of the Envir plugin mechanism: selects the output scalar manager class " +
        "to be used to record data passed to recordScalar(). The class has to " +
        "implement the cOutputScalarManager interface.");
    public static final ConfigKey CFGID_OUTPUTVECTORMANAGER_CLASS = addGlobalEntry(
        "outputvectormanager-class", CFG_STRING, "cIndexedFileOutputVectorManager",
        "Part of the Envir plugin mechanism: selects the output vector manager class " +
        "to be used to record data from output vectors. The class has to implement " +
        "the cOutputVectorManager interface.");
    public static final ConfigKey CFGID_PARALLEL_SIMULATION = addGlobalEntry(
        "parallel-simulation", CFG_BOOL, "false",
        "Enables parallel distributed simulation.");
    public static final ConfigKey CFGID_PARAM_RECORD_AS_SCALAR = addPerObjectEntry(
        "param-record-as-scalar", CFG_BOOL, "false",
        "Applicable to module parameters: specifies whether the module parameter " +
        "should be recorded into the output scalar file. Set it for parameters whose " +
        "value you'll need for result analysis.");
    public static final ConfigKey CFGID_PARSIM_COMMUNICATIONS_CLASS = addGlobalEntry(
        "parsim-communications-class", CFG_STRING, "cFileCommunications",
        "If parallel-simulation=true, it selects the class that implements " +
        "communication between partitions. The class must implement the " +
        "cParsimCommunications interface.");
    public static final ConfigKey CFGID_PARSIM_DEBUG = addGlobalEntry(
        "parsim-debug", CFG_BOOL, "true",
        "With parallel-simulation=true: turns on printing of log messages from the " +
        "parallel simulation code.");
    public static final ConfigKey CFGID_PARSIM_FILECOMMUNICATIONS_PREFIX = addGlobalEntry(
        "parsim-filecommunications-prefix", CFG_STRING, "comm/",
        "When cFileCommunications is selected as parsim communications class: " +
        "specifies the prefix (directory+potential filename prefix) for creating the " +
        "files for cross-partition messages.");
    public static final ConfigKey CFGID_PARSIM_FILECOMMUNICATIONS_PRESERVE_READ = addGlobalEntry(
        "parsim-filecommunications-preserve-read", CFG_BOOL, "false",
        "When cFileCommunications is selected as parsim communications class: " +
        "specifies that consumed files should be moved into another directory " +
        "instead of being deleted.");
    public static final ConfigKey CFGID_PARSIM_FILECOMMUNICATIONS_READ_PREFIX = addGlobalEntry(
        "parsim-filecommunications-read-prefix", CFG_STRING, "comm/read/",
        "When cFileCommunications is selected as parsim communications class: " +
        "specifies the prefix (directory) where files will be moved after having " +
        "been consumed.");
    public static final ConfigKey CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE = addGlobalEntry(
        "parsim-idealsimulationprotocol-tablesize", CFG_INT, "100000",
        "When cIdealSimulationProtocol is selected as parsim synchronization class: " +
        "specifies the memory buffer size for reading the ISP event trace file.");
    public static final ConfigKey CFGID_PARSIM_NAMEDPIPECOMMUNICATIONS_PREFIX = addGlobalEntry(
        "parsim-namedpipecommunications-prefix", CFG_STRING, "omnetpp",
        "When cNamedPipeCommunications is selected as parsim communications class: " +
        "selects the name prefix for Windows named pipes created.");
    public static final ConfigKey CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS = addGlobalEntry(
        "parsim-nullmessageprotocol-laziness", CFG_DOUBLE, "0.5",
        "When cNullMessageProtocol is selected as parsim synchronization class: " +
        "specifies the laziness of sending null messages. Values in the range [0,1) " +
        "are accepted. Laziness=0 causes null messages to be sent out immediately as " +
        "a new EOT is learned, which may result in excessive null message traffic.");
    public static final ConfigKey CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS = addGlobalEntry(
        "parsim-nullmessageprotocol-lookahead-class", CFG_STRING, "cLinkDelayLookahead",
        "When cNullMessageProtocol is selected as parsim synchronization class: " +
        "specifies the C++ class that calculates lookahead. The class should " +
        "subclass from cNMPLookahead.");
    public static final ConfigKey CFGID_PARSIM_SYNCHRONIZATION_CLASS = addGlobalEntry(
        "parsim-synchronization-class", CFG_STRING, "cNullMessageProtocol",
        "If parallel-simulation=true, it selects the parallel simulation algorithm. " +
        "The class must implement the cParsimSynchronizer interface.");
    public static final ConfigKey CFGID_PARTITION_ID = addPerObjectEntry(
        "partition-id", CFG_STRING, null,
        "With parallel simulation: in which partition the module should be " +
        "instantiated. Specify numeric partition ID, or a comma-separated list of " +
        "partition IDs for compound modules that span across multiple partitions. " +
        "Ranges (\"5..9\") and \"*\" (=all) are accepted too.");
    public static final ConfigKey CFGID_PRINT_UNDISPOSED = addGlobalEntry(
        "print-undisposed", CFG_BOOL, "true",
        "Whether to report objects left (that is, not deallocated by simple module " +
        "destructors) after network cleanup.");
    public static final ConfigKey CFGID_REALTIMESCHEDULER_SCALING = addGlobalEntry(
        "realtimescheduler-scaling", CFG_DOUBLE, null,
        "When cRealTimeScheduler is selected as scheduler class: ratio of simulation " +
        "time to real time. For example, scaling=2 will cause simulation time to " +
        "progress twice as fast as runtime.");
    public static final ConfigKey CFGID_RECORD_EVENTLOG = addPerRunEntry(
        "record-eventlog", CFG_BOOL, "false",
        "Enables recording an eventlog file, which can be later visualized on a " +
        "sequence chart. See eventlog-file= option too.");
    public static final ConfigKey CFGID_REPEAT = addPerRunEntry(
        "repeat", CFG_INT, "1",
        "For scenarios. Specifies how many replications should be done with the same " +
        "parameters (iteration variables). This is typically used to perform " +
        "multiple runs with different random number seeds. The loop variable is " +
        "available as ${repetition}. See also: seed-set= key.");
    public static final ConfigKey CFGID_REPLICATION_LABEL = addPerRunEntry(
        "replication-label", CFG_STRING, "#${repetition}",
        "Identifies one replication of a measurement (see repeat= and " +
        "measurement-label= as well). This string gets recorded into result files, " +
        "and may be referred to during result analysis.");
    public static final ConfigKey CFGID_RESULT_DIR = addPerRunEntry(
        "result-dir", CFG_STRING, "results",
        "Value for the ${resultdir} variable, which is used as the default directory " +
        "for result files (output vector file, output scalar file, eventlog file, " +
        "etc.)");
    public static final ConfigKey CFGID_RNG_n = addPerObjectEntry(
        "rng-%", CFG_INT, null,
        "Maps a module-local RNG to one of the global RNGs. Example: **.gen.rng-1=3 " +
        "maps the local RNG 1 of modules matching `**.gen' to the global RNG 3. The " +
        "default is one-to-one mapping.");
    public static final ConfigKey CFGID_RNG_CLASS = addPerRunEntry(
        "rng-class", CFG_STRING, "cMersenneTwister",
        "The random number generator class to be used. It can be `cMersenneTwister', " +
        "`cLCG32', `cAkaroaRNG', or you can use your own RNG class (it must be " +
        "subclassed from cRNG).");
    public static final ConfigKey CFGID_RUNNUMBER_WIDTH = addPerRunEntry(
        "runnumber-width", CFG_INT, "0",
        "Setting a nonzero value will cause the $runnumber variable to get padded " +
        "with leading zeroes to the given length.");
    public static final ConfigKey CFGID_SCALAR_RECORDING = addPerObjectEntry(
        "scalar-recording", CFG_BOOL, "true",
        "Whether the matching output scalars should be recorded. Syntax: " +
        "<module-full-path>.<scalar-name>.scalar-recording=true/false. Example: " +
        "**.queue.packetsDropped.scalar-recording=true");
    public static final ConfigKey CFGID_SCHEDULER_CLASS = addGlobalEntry(
        "scheduler-class", CFG_STRING, "cSequentialScheduler",
        "Part of the Envir plugin mechanism: selects the scheduler class. This " +
        "plugin interface allows for implementing real-time, hardware-in-the-loop, " +
        "distributed and distributed parallel simulation. The class has to implement " +
        "the cScheduler interface.");
    public static final ConfigKey CFGID_SEED_n_LCG32 = addPerRunEntry(
        "seed-%-lcg32", CFG_INT, null,
        "When cLCG32 is selected as random number generator: seed for the kth RNG. " +
        "(Substitute k for '%' in the key.)");
    public static final ConfigKey CFGID_SEED_n_MT = addPerRunEntry(
        "seed-%-mt", CFG_INT, null,
        "When Mersenne Twister is selected as random number generator (default): " +
        "seed for RNG number k. (Substitute k for '%' in the key.)");
    public static final ConfigKey CFGID_SEED_n_MT_Pn = addPerRunEntry(
        "seed-%-mt-p%", CFG_INT, null,
        "With parallel simulation: When Mersenne Twister is selected as random " +
        "number generator (default): seed for RNG number k in partition number p. " +
        "(Substitute k for the first '%' in the key, and p for the second.)");
    public static final ConfigKey CFGID_SEED_SET = addPerRunEntry(
        "seed-set", CFG_INT, "${runnumber}",
        "Selects the kth set of automatic random number seeds for the simulation. " +
        "Meaningful values include ${repetition} which is the repeat loop counter " +
        "(see repeat= key), and ${runnumber}.");
    public static final ConfigKey CFGID_SIM_TIME_LIMIT = addPerRunEntryU(
        "sim-time-limit", "s", null,
        "Stops the simulation when simulation time reaches the given limit. The " +
        "default is no limit.");
    public static final ConfigKey CFGID_SIMTIME_SCALE = addGlobalEntry(
        "simtime-scale", CFG_INT, "-12",
        "Sets the scale exponent, and thus the resolution of time for the 64-bit " +
        "fixed-point simulation time representation. Accepted values are -18..0; for " +
        "example, -6 selects microsecond resolution. -12 means picosecond " +
        "resolution, with a maximum simtime of ~110 days.");
    public static final ConfigKey CFGID_SNAPSHOT_FILE = addPerRunEntry(
        "snapshot-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.sna",
        "Name of the snapshot file.");
    public static final ConfigKey CFGID_SNAPSHOTMANAGER_CLASS = addGlobalEntry(
        "snapshotmanager-class", CFG_STRING, "cFileSnapshotManager",
        "Part of the Envir plugin mechanism: selects the class to handle streams to " +
        "which snapshot() writes its output. The class has to implement the " +
        "cSnapshotManager interface.");
    public static final ConfigKey CFGID_TKENV_DEFAULT_CONFIG = addGlobalEntry(
        "tkenv-default-config", CFG_STRING, null,
        "Specifies which config Tkenv should set up automatically on startup. The " +
        "default is to ask the user.");
    public static final ConfigKey CFGID_TKENV_DEFAULT_RUN = addGlobalEntry(
        "tkenv-default-run", CFG_INT, "0",
        "Specifies which run (of the default config, see tkenv-default-config) Tkenv " +
        "should set up automatically on startup. The default is to ask the user.");
    public static final ConfigKey CFGID_TKENV_EXTRA_STACK = addGlobalEntryU(
        "tkenv-extra-stack", "B", "48KB",
        "Specifies the extra amount of stack that is reserved for each activity() " +
        "simple module when the simulation is run under Tkenv.");
    public static final ConfigKey CFGID_TKENV_IMAGE_PATH = addGlobalEntry(
        "tkenv-image-path", CFG_PATH, null,
        "Specifies the path for loading module icons.");
    public static final ConfigKey CFGID_TKENV_PLUGIN_PATH = addGlobalEntry(
        "tkenv-plugin-path", CFG_PATH, null,
        "Specifies the search path for Tkenv plugins. Tkenv plugins are .tcl files " +
        "that get evaluated on startup.");
    public static final ConfigKey CFGID_TOTAL_STACK = addGlobalEntryU(
        "total-stack", "B", null,
        "Specifies the maximum memory for activity() simple module stacks. You need " +
        "to increase this value if you get a ``Cannot allocate coroutine stack'' " +
        "error.");
    public static final ConfigKey CFGID_TYPE_NAME = addPerObjectEntry(
        "type-name", CFG_STRING, null,
        "Specifies type for submodules and channels declared with 'like <>'.");
    public static final ConfigKey CFGID_USER_INTERFACE = addGlobalEntry(
        "user-interface", CFG_STRING, null,
        "Selects the user interface to be started. Possible values are Cmdenv and " +
        "Tkenv. This option is normally left empty, as it is more convenient to " +
        "specify the user interface via a command-line option or the IDE's Run and " +
        "Debug dialogs.");
    public static final ConfigKey CFGID_VECTOR_MAX_BUFFERED_VALUES = addPerObjectEntry(
        "vector-max-buffered-values", CFG_INT, null,
        "For output vectors: the maximum number of values to buffer per vector, " +
        "before writing out a block into the output vector file. The default is no " +
        "per-vector limit (i.e. only the total memory limit is in effect)");
    public static final ConfigKey CFGID_VECTOR_RECORD_EVENTNUMBERS = addPerObjectEntry(
        "vector-record-eventnumbers", CFG_BOOL, "true",
        "Whether to record event numbers for an output vector. Simulation time and " +
        "value are always recorded. Event numbers are needed by the Sequence Chart " +
        "Tool, for example.");
    public static final ConfigKey CFGID_VECTOR_RECORDING = addPerObjectEntry(
        "vector-recording", CFG_BOOL, "true",
        "Whether data written into an output vector should be recorded.");
    public static final ConfigKey CFGID_VECTOR_RECORDING_INTERVAL = addPerObjectEntry(
        "vector-recording-interval", CFG_CUSTOM, null,
        "Recording interval(s) for an output vector. Syntax: [<from>]..[<to>],... " +
        "That is, both start and end of an interval are optional, and intervals are " +
        "separated by comma. Example: ..100, 200..400, 900..");
    public static final ConfigKey CFGID_WARNINGS = addPerRunEntry(
        "warnings", CFG_BOOL, "true",
        "Enables warnings.");
    public static final ConfigKey CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER = addGlobalEntry(
        "parsim-mpicommunications-mpibuffer", CFG_INT, null,
        "When cMPICommunications is selected as parsim communications class: " +
        "specifies the size of the MPI communications buffer. The default is to " +
        "calculate a buffer size based on the number of partitions.");

    public static final String CFGVAR_RUNID = addConfigVariable("runid", "A reasonably globally unique identifier for the run, produced by concatenating the configuration name, run number, date/time, etc.");
    public static final String CFGVAR_INIFILE = addConfigVariable("inifile", "Name of the (primary) inifile");
    public static final String CFGVAR_CONFIGNAME = addConfigVariable("configname", "Name of the active configuration");
    public static final String CFGVAR_RUNNUMBER = addConfigVariable("runnumber", "Sequence number of the current run within all runs in the active configuration");
    public static final String CFGVAR_NETWORK = addConfigVariable("network", "Value of the \"network\" configuration option");
    public static final String CFGVAR_EXPERIMENT = addConfigVariable("experiment", "Value of the \"experiment-label\" configuration option");
    public static final String CFGVAR_MEASUREMENT = addConfigVariable("measurement", "Value of the \"measurement-label\" configuration option");
    public static final String CFGVAR_REPLICATION = addConfigVariable("replication", "Value of the \"replication-label\" configuration option");
    public static final String CFGVAR_PROCESSID = addConfigVariable("processid", "PID of the simulation process");
    public static final String CFGVAR_DATETIME = addConfigVariable("datetime", "Date and time the simulation run was started");
    public static final String CFGVAR_RESULTDIR = addConfigVariable("resultdir", "Value of the \"result-dir\" configuration option");
    public static final String CFGVAR_REPETITION = addConfigVariable("repetition", "The iteration number in 0..N-1, where N is the value of the \"repeat\" configuration option");
    public static final String CFGVAR_SEEDSET = addConfigVariable("seedset", "Value of the \"seed-set\" configuration option");
    public static final String CFGVAR_ITERATIONVARS = addConfigVariable("iterationvars", "Concatenation of all user-defined iteration variables in name=value form");
    public static final String CFGVAR_ITERATIONVARS2 = addConfigVariable("iterationvars2", "Concatenation of all user-defined iteration variables in name=value form, plus ${repetition}");

    static {
        EXTENDS = CFGID_EXTENDS.getKey();
    }
}
