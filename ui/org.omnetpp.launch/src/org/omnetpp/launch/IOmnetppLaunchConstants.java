package org.omnetpp.launch;

/**
 * @author rhornig
 */
/**
 * @author rhornig
 *
 */
public interface IOmnetppLaunchConstants {
	public static final String SIMULATION_LAUNCH_CONFIGURATION_TYPE = "org.omnetpp.launch.simulationLaunchConfigurationType";
    public static final String CDT_LAUNCH_ID = "org.eclipse.cdt.launch"; 
    public static final String CDT_DEBUG_CORE_ID ="org.eclipse.cdt.debug.mi.core";
    public static final String OMNETPP_LAUNCH_ID = "org.omnetpp.launch"; 

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
    public static final String ATTR_DEBUGGER_STOP_AT_MAIN = CDT_LAUNCH_ID + ".DEBUGGER_STOP_AT_MAIN"; 
    public static final String ATTR_DEBUGGER_STOP_AT_MAIN_SYMBOL = CDT_LAUNCH_ID + ".DEBUGGER_STOP_AT_MAIN_SYMBOL"; 
    
    // ========================================================================================
    
    /**
     * Working directory. Workspace relative path. 
     */
    public static final String OPP_WORKING_DIRECTORY = OMNETPP_LAUNCH_ID + ".WORKING_DIRECTORY"; 

    /**
     * The executable file can be relative (workspace) path or absolute (file system) location
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
     * Launch configuration attribute key. The value specifies the run command line parameter is
     * a standalone launcher was used. (-r)
     */
    public static final String OPP_RUNNUMBER = OMNETPP_LAUNCH_ID + ".RUN"; 
    
    /**
     * Attribute used to store the run number when debugging is used. (-r)
     */
    public static final String OPP_RUNNUMBER_FOR_DEBUG = OMNETPP_LAUNCH_ID + ".RUN.FOR_DEBUG"; 

    /**
     * How many simulations may run in parallel during batch execution
     */
    public static final String OPP_NUM_CONCURRENT_PROCESSES = OMNETPP_LAUNCH_ID + ".NUM_CONCURRENT_PROCESSES";
    
    /**
     * Which user interface to use (-u)
     */
    public static final String OPP_USER_INTERFACE = OMNETPP_LAUNCH_ID + ".USER_INTERFACE"; 
    
    /**
     * Shared libraries to be loaded (-l)
     */
    public static final String OPP_SHARED_LIBS = OMNETPP_LAUNCH_ID + ".SHARED_LIBS"; 
    
    /**
     * the project of the first dynamic library to be loaded (used if no executable is given in the configuration)
     */
    public static final String OPP_SHARED_LIB_PROJECT = OMNETPP_LAUNCH_ID + ".SHARED_LIB_PROJECT"; 
    
    /**
     * The ned path used to start the executable (-n option)
     */
    public static final String OPP_NED_PATH = OMNETPP_LAUNCH_ID + ".NED_PATH"; 
    
    /**
     * Whether to show the the debug view on launch
     */
    public static final String OPP_SHOWDEBUGVIEW = OMNETPP_LAUNCH_ID + ".SHOW_DEBUG_VIEW";

    /**
     * Whether to record eventlog
     */
    public static final String OPP_RECORD_EVENTLOG = OMNETPP_LAUNCH_ID + ".RECORD_EVENTLOG";

    /**
     * Whether to drop to debugger on an simulation error
     */
    public static final String OPP_DEBUG_ON_ERRORS = OMNETPP_LAUNCH_ID + ".DEBUG_ON_ERRORS";

    /**
     * Additional hand specified arguments
     */
    public static final String OPP_ADDITIONAL_ARGS = OMNETPP_LAUNCH_ID + ".ADDITIONAL_ARG";
}
