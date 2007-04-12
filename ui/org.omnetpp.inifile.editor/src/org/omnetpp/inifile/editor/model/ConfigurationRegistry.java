package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_BOOL;
import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_DOUBLE;
import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_FILENAME;
import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_FILENAMES;
import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_INT;
import static org.omnetpp.inifile.editor.model.ConfigurationEntry.Type.CFG_STRING;

import java.util.HashMap;

import org.omnetpp.inifile.editor.model.ConfigurationEntry.Type;

/**
 * Contains the names of supported configuration entries, e.g. [General]/sim-time-limit. 
 * @author Andras
 */
//TODO must be kept in sync with the C++ code
public class ConfigurationRegistry {
	/**
	 * Name of the [General] section
	 */
	public static final String GENERAL = "General";
	
	private static HashMap<String, ConfigurationEntry> entries = new HashMap<String, ConfigurationEntry>(); 
	
	private static ConfigurationEntry addGlobalEntry(String name, Type type, Object defaultValue, String description) {
    	ConfigurationEntry e = new ConfigurationEntry(name, true, type, null, defaultValue, description);
    	entries.put(name, e);
    	return e;
    }

	private static ConfigurationEntry addGlobalEntryU(String name, String unit, Object defaultValue, String description) {
    	ConfigurationEntry e = new ConfigurationEntry(name, false, CFG_DOUBLE, unit, defaultValue, description);
    	entries.put(name, e);
    	return e;
    }
    
	private static ConfigurationEntry addPerRunEntry(String name, Type type, Object defaultValue, String description) {
    	ConfigurationEntry e = new ConfigurationEntry(name, false, type, null, defaultValue, description);
    	entries.put(name, e);
    	return e;
    }

	private static ConfigurationEntry addPerRunEntryU(String name, String unit, Object defaultValue, String description) {
    	ConfigurationEntry e = new ConfigurationEntry(name, false, CFG_DOUBLE, unit, defaultValue, description);
    	entries.put(name, e);
    	return e;
    }

	public static ConfigurationEntry[] getEntries() {
		return (ConfigurationEntry[]) entries.values().toArray(new ConfigurationEntry[entries.size()]);
	}

	public static ConfigurationEntry getEntry(String name) {
		return entries.get(name);
	}
	
	private static final Object CMDENV_EXTRASTACK_KB = null;
    private static final Object DEFAULT_PRECISION = null;
    private static final Object TOTAL_STACK_KB = null;
    private static final Object TKENV_EXTRASTACK_KB = null;
    private static final Object TABLESIZE = null;
    private static final Object DEFAULT_MEMORY_LIMIT = null;

    public static final ConfigurationEntry CFGID_CMDENV_RUNS_TO_EXECUTE = addGlobalEntry(
    		"cmdenv-runs-to-execute", CFG_STRING, "",
    		"Specifies which simulation runs should be executed. It accepts a comma-separated list " +
    		"of run numbers or run number ranges, e.g. 1,3-4,7-9. If the value is missing, Cmdenv " +
    		"executes all runs that have ini file sections; if no runs are specified in the ini file, " +
    		"Cmdenv does one run. The -r command line option overrides this setting.");
    public static final ConfigurationEntry CFGID_CMDENV_EXTRA_STACK_KB = addGlobalEntry( 
    		"cmdenv-extra-stack-kb", CFG_INT, CMDENV_EXTRASTACK_KB, 
    		"Specifies the extra amount of stack (in kilobytes) that is reserved for each activity() " +
    		"simple module when the simulation is run under Cmdenv.");
    public static final ConfigurationEntry CFGID_CMDENV_OUTPUT_FILE = addGlobalEntry( 
    		"cmdenv-output-file", CFG_FILENAME, "", 
    		"When a filename is specified, Cmdenv redirects standard output into the given file. " +
    		"This is especially useful with parallel simulation. See `fname-append-host' option as well.");
    public static final ConfigurationEntry CFGID_LOAD_LIBS = addGlobalEntry( 
    		"load-libs", CFG_FILENAMES, "",
    		"Specifies dyamic libraries to be loaded on startup. The libraries should be given " +
    		"without the `.dll' or `.so' suffix -- that will be automatically appended.");
    public static final ConfigurationEntry CFGID_CONFIGURATION_CLASS = addGlobalEntry( 
    		"configuration-class", CFG_STRING, "",
    		"Part of the Envir plugin mechanism: selects the class from which all configuration " +
    		"will be obtained. This option lets you replace omnetpp.ini with some other implementation, " +
    		"e.g. database input. The simulation program still has to bootstrap from an omnetpp.ini " +
    		"(which contains the configuration-class setting). The class has to implement the " +
    		"cConfiguration interface.");
    public static final ConfigurationEntry CFGID_USER_INTERFACE = addGlobalEntry( 
    		"user-interface", CFG_STRING, "",
    		"Selects the user interface to be started. Possible values are Cmdenv and Tkenv, " +
    		"provided the simulation executable contains the respective libraries or loads " +
    		"them dynamically.");
    public static final ConfigurationEntry CFGID_OUTPUT_VECTOR_FILE = addGlobalEntry( 
    		"output-vector-file", CFG_FILENAME, "omnetpp.vec",
    		"Name for the output vector file.");
    public static final ConfigurationEntry CFGID_OUTPUT_VECTOR_PRECISION = addGlobalEntry( 
    		"output-vector-precision", CFG_INT, DEFAULT_PRECISION,
    		"Adjusts the number of significant digits for recording numbers into the output vector file.");
    public static final ConfigurationEntry CFGID_OUTPUT_SCALAR_FILE = addGlobalEntry( 
    		"output-scalar-file", CFG_FILENAME, "omnetpp.sca", 
    		"Name for the output scalar file.");
    public static final ConfigurationEntry CFGID_OUTPUT_SCALAR_PRECISION = addGlobalEntry( 
    		"output-scalar-precision", CFG_INT, DEFAULT_PRECISION, 
    		"Adjusts the number of significant digits for recording numbers into the output scalar file.");
    public static final ConfigurationEntry CFGID_SNAPSHOT_FILE = addGlobalEntry( 
    		"snapshot-file", CFG_FILENAME, "omnetpp.sna", 
    		"Name of the snapshot file.");
    public static final ConfigurationEntry CFGID_INI_WARNINGS = addGlobalEntry( 
    		"ini-warnings", CFG_BOOL, false, 
    		"Currently ignored. Accepted for backward compatibility.");
    public static final ConfigurationEntry CFGID_PRELOAD_NED_FILES = addGlobalEntry( 
    		"preload-ned-files", CFG_FILENAMES, "", 
    		"NED files to be loaded dynamically. Wildcards, @ and @@ listfiles accepted.");
    public static final ConfigurationEntry CFGID_TOTAL_STACK_KB = addGlobalEntry( 
    		"total-stack-kb", CFG_INT, TOTAL_STACK_KB,
    		"Specifies the maximum memory for activity() simple module stacks in kilobytes. " +
    		"You need to increase this value if you get a ``Cannot allocate coroutine stack'' error.");
    public static final ConfigurationEntry CFGID_PARALLEL_SIMULATION = addGlobalEntry( 
    		"parallel-simulation", CFG_BOOL, false,
    		"Enables parallel distributed simulation.");
    public static final ConfigurationEntry CFGID_SCHEDULER_CLASS = addGlobalEntry( 
    		"scheduler-class", CFG_STRING, "cSequentialScheduler", 
    		"Part of the Envir plugin mechanism: selects the scheduler class. This plugin interface " +
    		"allows for implementing real-time, hardware-in-the-loop, distributed and distributed " +
    		"parallel simulation. The class has to implement the cScheduler interface.");
    public static final ConfigurationEntry CFGID_PARSIM_COMMUNICATIONS_CLASS = addGlobalEntry(
    		"parsim-communications-class", CFG_STRING, "cFileCommunications",
    		"If parallel-simulation=true, it selects the class that implements communication between " +
    		"partitions. The class must implement the cParsimCommunications interface.");
    public static final ConfigurationEntry CFGID_PARSIM_SYNCHRONIZATION_CLASS = addGlobalEntry( 
    		"parsim-synchronization-class", CFG_STRING, "cNullMessageProtocol", 
    		"If parallel-simulation=true, it selects the parallel simulation algorithm. The class " +
    		"must implement the cParsimSynchronizer interface.");
    public static final ConfigurationEntry CFGID_NUM_RNGS = addGlobalEntry( 
    		"num-rngs", CFG_INT, 1, 
    		"Number of the random number generators.");
    public static final ConfigurationEntry CFGID_RNG_CLASS = addGlobalEntry( 
    		"rng-class", CFG_STRING, "cMersenneTwister",
    		"The random number generator class to be used. It can be `cMersenneTwister', `cLCG32', " +
    		"`cAkaroaRNG', or you can use your own RNG class (it must be subclassed from cRNG).");
    public static final ConfigurationEntry CFGID_OUTPUTVECTORMANAGER_CLASS = addGlobalEntry( 
    		"outputvectormanager-class", CFG_STRING, "cIndexedFileOutputVectorManager",
    		"Part of the Envir plugin mechanism: selects the output vector manager class " +
    		"to be used to record data from output vectors. The class has to implement the " +
    		"cOutputVectorManager interface.");
    public static final ConfigurationEntry CFGID_OUTPUTSCALARMANAGER_CLASS = addGlobalEntry( 
    		"outputscalarmanager-class", CFG_STRING, "cFileOutputScalarManager",
    		"Part of the Envir plugin mechanism: selects the output scalar manager class to be used " +
    		"to record data passed to recordScalar(). The class has to implement the " +
    		"cOutputScalarManager interface.");
    public static final ConfigurationEntry CFGID_SNAPSHOTMANAGER_CLASS = addGlobalEntry( 
    		"snapshotmanager-class", CFG_STRING, "cFileSnapshotManager",
    		"Part of the Envir plugin mechanism: selects the class to handle streams to which " +
    		"snapshot() writes its output. The class has to implement the cSnapshotManager interface.");
    public static final ConfigurationEntry CFGID_FNAME_APPEND_HOST = addGlobalEntry( 
    		"fname-append-host", CFG_BOOL, false,
    		"Turning it on will cause the host name and process Id to be appended to the names " +
    		"of output files (e.g. omnetpp.vec, omnetpp.sca). This is especially useful with " +
    		"distributed simulation.");
    public static final ConfigurationEntry CFGID_DEBUG_ON_ERRORS = addGlobalEntry( 
    		"debug-on-errors", CFG_BOOL, false,
    		"When set to true, runtime errors will cause the simulation program to break into the " +
    		"C++ debugger (if the simulation is running under one, or just-in-time debugging is activated). " +
    		"Once in the debugger, you can view the stack trace or examine variables.");
    public static final ConfigurationEntry CFGID_PERFORM_GC = addGlobalEntry( 
    		"perform-gc", CFG_BOOL, false,
    		"Whether the simulation kernel should delete on network cleanup the simulation objects " +
    		"not deleted by simple module destructors. Not recommended.");
    public static final ConfigurationEntry CFGID_PRINT_UNDISPOSED = addGlobalEntry( 
    		"print-undisposed", CFG_BOOL, true,
    		"Whether to report objects left (that is, not deallocated by simple module destructors) " +
    		"after network cleanup.");
    public static final ConfigurationEntry CFGID_SIMTIME_SCALE = addGlobalEntry( 
    		"simtime-scale", CFG_INT, -12,
    		"Sets the scale exponent, and thus the resolution of time for the 64-bit fixed-point " +
    		"simulation time representation. Accepted values are -18..0; for example, -6 selects " +
    		"microsecond resolution. -12 means picosecond resolution, with a maximum simtime of ~110 days.");
    public static final ConfigurationEntry CFGID_REALTIMESCHEDULER_SCALING = addGlobalEntry( 
    		"realtimescheduler-scaling", CFG_DOUBLE, 0,
    		"When cRealTimeScheduler is selected as scheduler class: ratio of simulation time to real time. " +
    		"For example, scaling=2 will cause simulation time to progress twice as fast as runtime.");
    public static final ConfigurationEntry CFGID_PARSIM_FILECOMM_PREFIX = addGlobalEntry( 
    		"parsim-filecommunications-prefix", CFG_STRING, "comm/",
    		"When cFileCommunications is selected as parsim communications class: specifies the prefix " +
    		"(directory+potential filename prefix) for creating the files for cross-partition messages.");
    public static final ConfigurationEntry CFGID_PARSIM_FILECOMM_READ_PREFIX = addGlobalEntry( 
    		"parsim-filecommunications-read-prefix", CFG_STRING, "comm/read/",
    		"When cFileCommunications is selected as parsim communications class: specifies the " +
    		"prefix (directory) where files will be moved after having been consumed.");
    public static final ConfigurationEntry CFGID_PARSIM_FILECOMM_PRESERVE_READ = addGlobalEntry( 
    		"parsim-filecommunications-preserve-read", CFG_BOOL, false,
    		"When cFileCommunications is selected as parsim communications class: specifies that " +
    		"consumed files should be moved into another directory instead of being deleted.");
    public static final ConfigurationEntry CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE = addGlobalEntry( 
    		"parsim-idealsimulationprotocol-tablesize", CFG_INT, TABLESIZE,
    		"When cIdealSimulationProtocol is selected as parsim synchronization class: specifies " +
    		"the memory buffer size for reading the ISP event trace file.");
    public static final ConfigurationEntry CFGID_PARSIM_MPICOMMUNICATIONS_MPIBUFFER = addGlobalEntry( 
    		"parsim-mpicommunications-mpibuffer", CFG_INT, -1,
    		"When cMPICommunications is selected as parsim communications class: specifies the size " +
    		"of the MPI communications buffer. The default is to calculate a buffer size based on the " +
    		"number of partitions.");
    public static final ConfigurationEntry CFGID_PARSIM_NAMEDPIPECOMM_PREFIX = addGlobalEntry( 
    		"parsim-namedpipecommunications-prefix", CFG_STRING, "comm/",
    		"When cNamedPipeCommunications is selected as parsim communications class: selects the " +
    		"prefix (directory+potential filename prefix) where name pipes are created in the file system.");
    //public static final InifileEntry CFGID_PARSIM_NAMEDPIPECOMM_PREFIX = addGlobalEntry( "parsim-namedpipecommunications-prefix", CFG_STRING, "omnetpp", 
    //      "When cNamedPipeCommunications is selected as parsim communications class: selects the" + 
    //      "name prefix for Windows named pipes created.");
    public static final ConfigurationEntry CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS = addGlobalEntry( 
    		"parsim-nullmessageprotocol-lookahead-class", CFG_STRING, "cLinkDelayLookahead",
    		"When cNullMessageProtocol is selected as parsim synchronization class: specifies " +
    		"the C++ class that calculates lookahead. The class should subclass from cNMPLookahead.");
    public static final ConfigurationEntry CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS = addGlobalEntry( 
    		"parsim-nullmessageprotocol-laziness", CFG_DOUBLE, 0.5,
    		"When cNullMessageProtocol is selected as parsim synchronization class: specifies the " +
    		"laziness of sending null messages. Values in the range [0,1) are accepted. " +
    		"Laziness=0 causes null messages to be sent out immediately as a new EOT is learned, " +
    		"which may result in excessive null message traffic.");
    public static final ConfigurationEntry CFGID_PARSIM_DEBUG = addGlobalEntry( 
    		"parsim-debug", CFG_BOOL, true,
    		"With parallel-simulation=true: turns on printing of log messages from the parallel simulation code.");
    public static final ConfigurationEntry CFGID_TKENV_EXTRA_STACK_KB = addGlobalEntry( 
    		"tkenv-extra-stack-kb", CFG_INT, TKENV_EXTRASTACK_KB,
    		"Specifies the extra amount of stack (in kilobytes) that is reserved for each activity() " +
    		"simple module when the simulation is run under Tkenv.");
    public static final ConfigurationEntry CFGID_TKENV_DEFAULT_RUN = addGlobalEntry( 
    		"tkenv-default-run", CFG_INT, 0,
    		"Specifies which run Tkenv should set up automatically on startup. The default is to ask the user.");
    public static final ConfigurationEntry CFGID_TKENV_SLOWEXEC_DELAY = addGlobalEntryU( 
    		"tkenv-slowexec-delay", "s", 0.3, 
    		"Delay between steps when you slow-execute the simulation.");
    public static final ConfigurationEntry CFGID_TKENV_UPDATE_FREQ_FAST = addGlobalEntry( 
    		"tkenv-update-freq-fast", CFG_INT, 50, 
    		"Number of events executed between two display updates when in Fast execution mode.");
    public static final ConfigurationEntry CFGID_TKENV_UPDATE_FREQ_EXPRESS = addGlobalEntry( 
    		"tkenv-update-freq-express", CFG_INT, 10000, 
    		"Number of events executed between two display updates when in Express execution mode.");
    public static final ConfigurationEntry CFGID_TKENV_ANIMATION_ENABLED = addGlobalEntry( 
    		"tkenv-animation-enabled", CFG_BOOL, true, 
    		"Enables/disables animation.");
    public static final ConfigurationEntry CFGID_TKENV_NEXT_EVENT_MARKERS = addGlobalEntry( 
    		"tkenv-next-event-markers", CFG_BOOL, true, 
    		"Whether to display the next event marker (red rectange).");
    public static final ConfigurationEntry CFGID_TKENV_SENDDIRECT_ARROWS = addGlobalEntry( 
    		"tkenv-senddirect-arrows", CFG_BOOL, true,
    		"Whether to display arrows during animation of sendDirect() calls.");
    public static final ConfigurationEntry CFGID_TKENV_ANIM_METHODCALLS = addGlobalEntry( 
    		"tkenv-anim-methodcalls", CFG_BOOL, true, 
    		"Whether to animate method calls across modules. Only calls to methods which contain " +
    		"Enter_Method() can be animated.");
    public static final ConfigurationEntry CFGID_TKENV_METHODCALLS_DELAY = addGlobalEntryU( 
    		"tkenv-methodcalls-delay", "s", 0.2, 
    		"Sets delay after method call animation.");
    public static final ConfigurationEntry CFGID_TKENV_ANIMATION_MSGNAMES = addGlobalEntry( 
    		"tkenv-animation-msgnames", CFG_BOOL, true, 
    		"Enables/disables displaying message names during message flow animation.");
    public static final ConfigurationEntry CFGID_TKENV_ANIMATION_MSGCLASSNAMES = addGlobalEntry( 
    		"tkenv-animation-msgclassnames", CFG_BOOL, true, 
    		"Enables/disables displaying the C++ class name of messages during animation.");
    public static final ConfigurationEntry CFGID_TKENV_ANIMATION_MSGCOLORS = addGlobalEntry( 
    		"tkenv-animation-msgcolors", CFG_BOOL, true, 
    		"Enables/disables using different colors for each message kind during message flow animation.");
    public static final ConfigurationEntry CFGID_TKENV_PENGUIN_MODE = addGlobalEntry( 
    		"tkenv-penguin-mode", CFG_BOOL, false, 
    		"Surprise surprise.");
    public static final ConfigurationEntry CFGID_TKENV_SHOW_LAYOUTING = addGlobalEntry( 
    		"tkenv-show-layouting", CFG_BOOL, false, 
    		"Show layouting process of network graphics.");
    public static final ConfigurationEntry CFGID_TKENV_USE_NEW_LAYOUTER = addGlobalEntry( 
    		"tkenv-use-new-layouter", CFG_BOOL, false, 
    		"Selects between the new and the old (3.x) network layouting algorithms.");
    public static final ConfigurationEntry CFGID_TKENV_SHOW_BUBBLES = addGlobalEntry( 
    		"tkenv-show-bubbles", CFG_BOOL, true, 
    		"Whether to honor the bubble() calls during animation.");
    public static final ConfigurationEntry CFGID_TKENV_ANIMATION_SPEED = addGlobalEntry( 
    		"tkenv-animation-speed", CFG_DOUBLE, 1.5, 
    		"Controls the speed of the animation; values in the range 0..3 are accepted.");
    public static final ConfigurationEntry CFGID_TKENV_PRINT_BANNERS = addGlobalEntry( 
    		"tkenv-print-banners", CFG_BOOL, true, 
    		"Enables/disables printing banners for each event.");
    public static final ConfigurationEntry CFGID_TKENV_USE_MAINWINDOW = addGlobalEntry( 
    		"tkenv-use-mainwindow", CFG_BOOL, true, 
    		"Enables/disables writing ev output to the Tkenv main window.");
    public static final ConfigurationEntry CFGID_TKENV_EXPRESSMODE_AUTOUPDATE = addGlobalEntry( 
    		"tkenv-expressmode-autoupdate", CFG_BOOL, true, 
    		"Enables/disables updating the inspectors during Express mode.");
    public static final ConfigurationEntry CFGID_TKENV_IMAGE_PATH = addGlobalEntry( 
    		"tkenv-image-path", CFG_FILENAME, "", 
    		"Specifies the path for loading module icons.");
    public static final ConfigurationEntry CFGID_TKENV_PLUGIN_PATH = addGlobalEntry( 
    		"tkenv-plugin-path", CFG_FILENAME, "", 
    		"Specifies the search path for Tkenv plugins. Tkenv plugins are .tcl files " +
    		"that get evaluated on startup.");
    public static final ConfigurationEntry CFGID_CMDENV_EXPRESS_MODE = addPerRunEntry(
    		"cmdenv-express-mode", CFG_BOOL, true, 
    		"Selects ``normal'' (debug/trace) or ``express'' mode.");
    public static final ConfigurationEntry CFGID_CMDENV_AUTOFLUSH = addPerRunEntry(
    		"cmdenv-autoflush", CFG_BOOL, false, 
    		"Call fflush(stdout) after each event banner or status update; affects both express " +
    		"and normal mode. Turning on autoflush can be useful with printf-style debugging for " +
    		"tracking down program crashes.");
    public static final ConfigurationEntry CFGID_CMDENV_MODULE_MESSAGES = addPerRunEntry(
    		"cmdenv-module-messages", CFG_BOOL, true, 
    		"When cmdenv-express-mode=false: turns printing module ev<< output on/off.");
    public static final ConfigurationEntry CFGID_CMDENV_EVENT_BANNERS = addPerRunEntry(
    		"cmdenv-event-banners", CFG_BOOL, true, 
    		"When cmdenv-express-mode=false: turns printing event banners on/off.");
    public static final ConfigurationEntry CFGID_CMDENV_EVENT_BANNER_DETAILS = addPerRunEntry(
    		"cmdenv-event-banner-details", CFG_BOOL, false, 
    		"When cmdenv-express-mode=false: print extra information after event banners.");
    public static final ConfigurationEntry CFGID_CMDENV_MESSAGE_TRACE = addPerRunEntry(
    		"cmdenv-message-trace", CFG_BOOL, false, 
    		"When cmdenv-express-mode=false: print a line per message sending (by send(),scheduleAt(), etc) " +
    		"and delivery on the standard output.");
    public static final ConfigurationEntry CFGID_CMDENV_STATUS_FREQUENCY = addPerRunEntry(
    		"cmdenv-status-frequency", CFG_INT, 100000, 
    		"When cmdenv-express-mode=true: print status update every n events. Typical values are 100,000...1,000,000.");
    public static final ConfigurationEntry CFGID_CMDENV_PERFORMANCE_DISPLAY = addPerRunEntry(
    		"cmdenv-performance-display", CFG_BOOL, true, 
    		"When cmdenv-express-mode=true: print detailed performance information. Turning it on results in " +
    		"a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of " +
    		"messages created/still present/currently scheduled in FES.");
    public static final ConfigurationEntry CFGID_OUTPUT_VECTORS_MEMORY_LIMIT = addPerRunEntryU(
    		"output-vectors-memory-limit", "B", DEFAULT_MEMORY_LIMIT, 
    		"Total memory that can be used for buffering output vectors. Larger values produce " +
    		"less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), " +
    		"and therefore allow more efficient processing later.");
    public static final ConfigurationEntry CFGID_DESCRIPTION = addPerRunEntry(
    		"description", CFG_STRING, "",
    		"Descriptive name for the given simulation configuration. Descriptions get displayed " +
    		"in the run selection dialog.");
    public static final ConfigurationEntry CFGID_EXTENDS = addPerRunEntry(
    		"extends", CFG_STRING, "",
    		"Specifies which section parameter and configuration lookups will fall back to. "+
    		"[Config X] is represented as extends=X. If missing, lookups fall back to the [General] section.");
    public static final ConfigurationEntry CFGID_NETWORK = addPerRunEntry(
    		"network", CFG_STRING, "default",
    		"The name of the network to be simulated.");
    public static final ConfigurationEntry CFGID_WARNINGS = addPerRunEntry(
    		"warnings", CFG_BOOL, true,
    		"Enables warnings.");
    public static final ConfigurationEntry CFGID_SIM_TIME_LIMIT = addPerRunEntryU(
    		"sim-time-limit", "s", 0.0,
    		"Stops the simulation when simulation time reaches the given limit. The default is no limit.");
    public static final ConfigurationEntry CFGID_CPU_TIME_LIMIT = addPerRunEntryU(
    		"cpu-time-limit", "s", 0.0,
    		"Stops the simulation when CPU usage has reached the given limit. The default is no limit.");
    public static final ConfigurationEntry CFGID_FINGERPRINT = addPerRunEntry(
    		"fingerprint", CFG_STRING, "",
    		"The expected fingerprint, suitable for crude regression tests. If present, the actual " +
    		"fingerprint is calculated during simulation, and compared against the expected one.");
    public static final ConfigurationEntry CFGID_EVENTLOG_FILE = addPerRunEntry(
    		"eventlog-file", CFG_FILENAME, "",
    		"Name of the event log file to generate. If emtpy, no file is generated.");
    
}
