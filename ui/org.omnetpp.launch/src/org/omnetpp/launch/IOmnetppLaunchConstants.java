package org.omnetpp.launch;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public interface IOmnetppLaunchConstants {
	public static final String SIMULATION_LAUNCH_CONFIGURATION_TYPE = "org.omnetpp.launch.simulationLaunchConfigurationType";
    public static final String CDT_LAUNCH_ID = "org.eclipse.cdt.launch"; 
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
     * Launch configuration attribute key. The value specifies the run command line parameter is
     * a standalone launcher was used.
     */
    public static final String ATTR_RUNNUMBER = OMNETPP_LAUNCH_ID + ".RUN"; 

    /**
     * Attribute used to store the run number when debugging is used.
     */
    public static final String ATTR_RUNNUMBER_FOR_DEBUG = OMNETPP_LAUNCH_ID + ".RUN.FOR_DEBUG"; 
    
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
     * Whether to show the the debug view on launch
     */
    public static final String ATTR_SHOWDEBUGVIEW = OMNETPP_LAUNCH_ID + ".SHOW_DEBUG_VIEW";

    /**
     * How many simulations may run in parallel during batch execution
     */
    public static final String ATTR_NUM_CONCURRENT_PROCESSES = OMNETPP_LAUNCH_ID + ".NUM_CONCURRENT_PROCESSES";
}
