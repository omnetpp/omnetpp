/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;

import java.util.List;

/**
 * Constants for OMNeT++ launch configurations.
 */
public interface IOmnetppLaunchConstants {
    public static final String SIMULATION_LAUNCH_CONFIGURATION_TYPE = "org.omnetpp.launch.simulationLaunchConfigurationType";
    // the below IDs are copied from the CDT or LSP4E plugins
    public static final String CDT_LAUNCH_ID = "org.eclipse.cdt.launch";
    public static final String CDT_DEBUG_CORE_ID ="org.eclipse.cdt.debug.mi.core";
    public static final String CDT_DSF_GDB_ID ="org.eclipse.cdt.dsf.gdb";
    public static final String OMNETPP_LAUNCH_ID = "org.omnetpp.launch";
	public static final String ID_DSP_DEBUG_MODEL = "org.eclipse.lsp4e.debug.model";

    /**
     * Launch configuration attribute key. The value is a name of a C/C++
     * project associated with a C/C++ launch configuration.
     */
    public static final String ATTR_PROJECT_NAME = CDT_LAUNCH_ID + ".PROJECT_ATTR";

    /**
     * Launch configuration attribute key. The value is a string specifying
     * application a C/C++ launch configuration.
     */
    public static final String ATTR_PROGRAM_NAME = CDT_LAUNCH_ID + ".PROGRAM_NAME";

    /**
     * Launch configuration attribute key. The value is a string specifying
     * application arguments for a C/C++ launch configuration, as they should
     * appear on the command line.
     */
    public static final String ATTR_PROGRAM_ARGUMENTS = CDT_LAUNCH_ID + ".PROGRAM_ARGUMENTS";

	/**
	 * Attribute used to store the launched process' environment variables
	 */
	public static final String ATTR_PROGRAM_ENVIRONMENT_MAP = "org.eclipse.debug.core.environmentVariables";

    /**
     * Launch configuration attribute key. The value is a string specifying a
     * path to the working directory to use when launching the application.
     * When unspecified, the working directory is inherited from the current
     * process. When specified as an absolute path, the path represents a path
     * in the local file system. When specified as a full path, the path
     * represents a workspace relative path.
     */
    public static final String ATTR_WORKING_DIRECTORY = CDT_LAUNCH_ID + ".WORKING_DIRECTORY";

    /**
     * CDT debugger specific
     */
    public static final String ATTR_DEBUGGER_ID = CDT_LAUNCH_ID + ".DEBUGGER_ID";
    public static final String ATTR_DEBUGGER_START_MODE = CDT_LAUNCH_ID + ".DEBUGGER_START_MODE";
    public static final String ATTR_DEBUGGER_STOP_AT_MAIN = CDT_LAUNCH_ID + ".DEBUGGER_STOP_AT_MAIN";
    public static final String ATTR_DEBUGGER_STOP_AT_MAIN_SYMBOL = CDT_LAUNCH_ID + ".DEBUGGER_STOP_AT_MAIN_SYMBOL";
    public static final String ATTR_DEBUG_NAME = CDT_DSF_GDB_ID + ".DEBUG_NAME";
    public static final String ATTR_GDB_INIT = CDT_DSF_GDB_ID + ".GDB_INIT";
    
    /**
     * DAP debugger specific
     */
    public static final String ATTR_DSP_CMD = ID_DSP_DEBUG_MODEL + ".ATTR_DSP_CMD";
    public static final String ATTR_DSP_ARGS = ID_DSP_DEBUG_MODEL + ".ATTR_DSP_ARGS";
 
    // ========================================================================================

    /**
     * Working directory. Workspace relative path.
     */
    public static final String OPP_WORKING_DIRECTORY = OMNETPP_LAUNCH_ID + ".WORKING_DIRECTORY";

    /**
     * The executable file is always an absolute workspace path
     */
    public static final String OPP_EXECUTABLE = OMNETPP_LAUNCH_ID + ".EXECUTABLE";

    /**
     * The initialization files (relative to the working dir)
     */
    public static final String OPP_INI_FILES = OMNETPP_LAUNCH_ID + ".INI_FILES";

    /**
     * Configuration to start with (-c)
     */
    public static final String OPP_CONFIG_NAME = OMNETPP_LAUNCH_ID + ".CONFIG_NAME";

    /**
     * Launch configuration attribute key. The value specifies the run filter (-r command line option)
     */
    public static final String OPP_RUNFILTER = OMNETPP_LAUNCH_ID + ".RUN";

    /**
     * Whether multiple processes should be launched as opp_run does (true), or a single simulation process (false)
     */
    public static final String OPP_USE_BATCHING = OMNETPP_LAUNCH_ID + ".USE_BATCHING";

    /**
     * How many processes may run in parallel during batch execution
     */
    public static final String OPP_NUM_CONCURRENT_PROCESSES = OMNETPP_LAUNCH_ID + ".NUM_CONCURRENT_PROCESSES";

    /**
     * How many simulations to run per process (Cmdenv instance).
     */
    public static final String OPP_BATCH_SIZE = OMNETPP_LAUNCH_ID + ".BATCH_SIZE";

    /**
     * Which user interface to use (-u)
     */
    public static final String OPP_USER_INTERFACE = OMNETPP_LAUNCH_ID + ".USER_INTERFACE";

    /**
     * TCP port number for the embedded web server that also serves requests
     * from the Simulation Front-End (-p). Must be an integer.
     */
    public static final String OPP_HTTP_PORT = CDT_LAUNCH_ID + ".HTTP_PORT";

    /**
     * Shared libraries to be loaded (-l)
     */
    public static final String OPP_SHARED_LIBS = OMNETPP_LAUNCH_ID + ".SHARED_LIBS";

    /**
     * The NED path used to start the executable (-n option)
     */
    public static final String OPP_NED_PATH = OMNETPP_LAUNCH_ID + ".NED_PATH";

    /**
     * NED package exclusions (-x option)
     */
    public static final String OPP_NED_PACKAGE_EXCLUSIONS = OMNETPP_LAUNCH_ID + ".NED_PACKAGE_EXCLUSIONS";

    /**
     * The image path used to start the executable (--image-path option)
     */
    public static final String OPP_IMAGE_PATH = OMNETPP_LAUNCH_ID + ".IMAGE_PATH";

    /**
     * Whether to record cmdenv output (tri-state setting)
     */
    public static final String OPP_CMDENV_REDIRECT_STDOUT = OMNETPP_LAUNCH_ID + ".REDIRECT_STDOUT";

    /**
     * Whether to stop batch execution on first error
     */
    public static final String OPP_STOP_BATCH_ON_ERROR = OMNETPP_LAUNCH_ID + ".STOP_BATCH_ON_ERROR";

    /**
     * Whether to record eventlog (tri-state setting)
     */
    public static final String OPP_RECORD_EVENTLOG = OMNETPP_LAUNCH_ID + ".RECORD_EVENTLOG";

    /**
     * Whether to record scalar results (tri-state setting)
     */
    public static final String OPP_RECORD_SCALARS = OMNETPP_LAUNCH_ID + ".RECORD_SCALARS";

    /**
     * Whether to record vector results (tri-state setting)
     */
    public static final String OPP_RECORD_VECTORS = OMNETPP_LAUNCH_ID + ".RECORD_VECTORS";

    /**
     * Whether to set the cmdenv-express-mode config option (tri-state setting)
     */
    public static final String OPP_CMDENV_EXPRESS_MODE = OMNETPP_LAUNCH_ID + ".CMDENV_EXPRESS_MODE";

    /**
     * Whether to pass the "-s" option to the simulation (boolean)
     */
    public static final String OPP_SILENT = OMNETPP_LAUNCH_ID + ".SILENT";

    /**
     * Whether to drop to debugger on an simulation error; values are "true", "false", "auto", "".
     * "" means that the "Default" radio button should be selected in the launch dialog.
     * A missing (unset) value MUST be understood as "auto".
     */
    @Deprecated
    public static final String OPP_DEBUG_ON_ERRORS = OMNETPP_LAUNCH_ID + ".DEBUG_ON_ERRORS"; // NOTE: Obsolete/unused since version 5.1

    /**
     * Only for DEBUG mode: Whether to drop to debugger on an simulation error (tri-state setting).
     * Ignored in non-debug mode.
     */
    public static final String OPP_DEBUGMODE_DEBUG_ON_ERRORS = OMNETPP_LAUNCH_ID + ".OPP_DEBUGMODE_DEBUG_ON_ERRORS";

    /**
     * Simulation time limit (sim-time-limit option)
     */
    public static final String OPP_SIM_TIME_LIMIT = OMNETPP_LAUNCH_ID + ".OPP_SIM_TIME_LIMIT";

    /**
     * CPU time limit (cpu-time-limit option)
     */
    public static final String OPP_CPU_TIME_LIMIT = OMNETPP_LAUNCH_ID + ".OPP_CPU_TIME_LIMIT";

    /**
     * Additional hand specified arguments
     */
    public static final String OPP_ADDITIONAL_ARGS = OMNETPP_LAUNCH_ID + ".ADDITIONAL_ARG";

    /**
     * Check or ignore problems on launch
     */
    public static final String OPP_IGNORE_PROBLEMS_ON_LAUNCH = OMNETPP_LAUNCH_ID + ".IGNORE_PROBLEMS_ON_LAUNCH";

    /**
     * Whether to build before launch (0 = Project + dependencies, 1 = project only, 2 = Never build (nothing))
     */
    public static final String OPP_BUILD_BEFORE_LAUNCH = OMNETPP_LAUNCH_ID + ".BUILD_BEFORE_LAUNCH";
    public static final int OPP_BUILD_BEFORE_LAUNCH_DEPENDENCIES = 0;
    public static final int OPP_BUILD_BEFORE_LAUNCH_PROJECT_ONLY = 1;
    public static final int OPP_BUILD_BEFORE_LAUNCH_NONE = 2;

    /**
     * Whether to switch active configuration before building (0 = Ask, 1 = Automatic, 2 = Never switch)
     */
    public static final String OPP_SWITCH_CONFIG_BEFORE_BUILD = OMNETPP_LAUNCH_ID + ".SWITCH_CONFIG_BEFORE_BUILD";
    public static final int OPP_SWITCH_CONFIG_BEFORE_BUILD_ASK = 0;
    public static final int OPP_SWITCH_CONFIG_BEFORE_BUILD_AUTO = 1;
    public static final int OPP_SWITCH_CONFIG_BEFORE_BUILD_NEVER = 2;

    /**
     * The default value for the gdbinit file (containing pretty printers)
     */
    public static final String OPP_GDB_INIT_FILE = "${opp_root}/misc/gdb/gdbinit.py";

    /**
     * Attribute name used to store the list of .lldbinit files
     */
	public static final String OPP_LLDB_INIT_FILES = OMNETPP_LAUNCH_ID + ".OPP_LLDB_INIT_FILES";;

    /**
     * The default value for starting gdb command (with arguments) 
     */
    public static final String OPP_DEFAULT_GDB_CMD = "sh ${opp_root}/bin/opp_dbgmi";
    
    /*
     * The default value for starting the lldb-dap debugger (without arguments)
     */
    public static final String OPP_DEFAULT_DSP_CMD_NAME = "sh";
    
    /*
     * The default value for arguments when starting the lldb-dap debugger
     */
    public static final List<String> OPP_DEFAULT_DSP_CMD_ARGS = List.of("${opp_root}/bin/opp_dbgdap");
    
    /**
     * Value for OPP_USER_INTERFACE: launch the simulation program without explicit -u option
     */
    public static final String UI_DEFAULTEXTERNAL = "<default-external>";

    /**
     * Value for OPP_USER_INTERFACE: launch the simulation program with -u Cmdenv
     */
    public static final String UI_CMDENV = "Cmdenv";

    /**
     * Value for OPP_USER_INTERFACE: launch the simulation program with -u Qtenv
     */
    public static final String UI_QTENV = "Qtenv";
}
