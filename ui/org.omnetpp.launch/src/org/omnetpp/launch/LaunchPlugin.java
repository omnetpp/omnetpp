package org.omnetpp.launch;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.math.NumberUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import org.omnetpp.common.util.StringUtils;

/**
 * The activator class controls the plug-in life cycle
 *
 * @author rhornig
 */
public class LaunchPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static String PLUGIN_ID;

	// The shared instance
	private static LaunchPlugin plugin;

	/**
	 * The constructor
	 */
	public LaunchPlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	@Override
    public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
        PLUGIN_ID = getBundle().getSymbolicName();
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	@Override
    public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 */
	public static LaunchPlugin getDefault() {
		return plugin;
	}

    /**
     * Returns an image descriptor for the image file at the given
     * plug-in relative path.
     */
    public static ImageDescriptor getImageDescriptor(String path) {
        return imageDescriptorFromPlugin(PLUGIN_ID, path);
    }

    /**
     * Creates an image. IMPORTANT: The image is NOT cached! Callers
     * are responsible for disposal of the image.
     */
    public static Image getImage(String path) {
        return getImageDescriptor(path).createImage();
    }

    public static void logError(Throwable exception) {
        logError(exception.toString(), exception);
    }

    public static void logError(String message, Throwable exception) {
        if (plugin != null) {
            plugin.getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, message, exception));
        }
        else {
            System.err.println(message);
            exception.printStackTrace();
        }
    }

    /**
     * Expands and returns the working directory attribute of the given launch
     * configuration. Returns <code>null</code> if a working directory is not
     * specified. If specified, the working is verified to point to an existing
     * directory in the local file system.
     *
     * @return an absolute path to a directory in the local file system, or
     * <code>null</code> if unspecified
     *
     * @throws CoreException if unable to retrieve the associated launch
     * configuration attribute, if unable to resolve any variables, or if the
     * resolved location does not point to an existing directory in the local
     * file system
     */
    public static IPath getWorkingDirectoryPath(ILaunchConfiguration config){
        String location = "${workspace_loc}";
        try {
            // get the working directory. the default will be the project root of the currently selected exe file
            String projname = config.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
            String progname = config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
            if (StringUtils.isNotBlank(projname) && StringUtils.isNotBlank(progname))
                location = "${project_loc:/"+projname+"/"+progname+"}";
            if (StringUtils.isBlank(projname) && StringUtils.isNotBlank(progname))
                location = "${container_loc:/"+progname+"}";
            location = config.getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, location);

            if (location != null) {
                String expandedLocation = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(location);
                if (expandedLocation.length() > 0) {
                    IPath newPath = new Path(expandedLocation);
                    return newPath.makeAbsolute();
                }
            }
        } catch (CoreException e) {
            LaunchPlugin.logError("Error getting working directory from configuration", e);
        }
        return new Path(location);
    }

    /******************************************************************************************************/
    // run parameters and standard output parsing

    /**
     * TODO comment: what does it do?
     *
     * @param runPar The provided run string (eg.: 1,2,5,9-23,44,55-70)
     *  "*" means ALL run number: 0-(maxRunNo-1). Empty runPar means: (run#0)
     * @param maxRunNo The maximum run number accepted by the
     * @return An array containing all runs, or NULL on parse error
     */
    public static Integer[] parseRuns(String runPar, int maxRunNo) {
        List<Integer> result = new ArrayList<Integer>();
        runPar = StringUtils.deleteWhitespace(runPar);
        if (StringUtils.isEmpty(runPar)) {
            // empty means: just the first run (0)
            result.add(0);
        }
        else if ("*".equals(runPar)) {
            // create ALL run numbers scenario
            for (int i=0; i<maxRunNo; i++)
                result.add(i);
        }
        else {
            // parse hand specified numbers
            for (String current : StringUtils.split(runPar, ',')) {
                String lowerUpper[] = StringUtils.splitByWholeSeparator(current, "..");
                int low = 0;
                int high = 0;
                try {
                    if (lowerUpper.length > 0) {
                        low = high = Math.min(Integer.parseInt(lowerUpper[0]), maxRunNo);
                    }
                    // if we have an upper bound too
                    if (lowerUpper.length > 1) {
                        high = Math.min(Integer.parseInt(lowerUpper[1]), maxRunNo);
                    }
                    // add all integers in the interval to the list
                    for (int i = low; i<=high; ++i)
                        result.add(i);
                } catch (NumberFormatException e) {
                    return null;
                }
            }
        }
        return result.toArray(new Integer[result.size()]);
    }

    /**
     * Starts the simulation program.
     *
     * @param configuration
     * @param additionalArgs extra command line arguments to be prepended to the command line
<<<<<<< .mine
     * @param requestInfo Setting it to true runs the process in "INFO" mode (replaces the -c arg with -x)
=======
     * @param requestInfo Setting it to true runs the process in "INFO" mode (replaces the -c arg width -x)
>>>>>>> .r5465
     * @return The created process object,
     * @throws CoreException if process is not started correctly
     */
    public static Process startSimulationProcess(ILaunchConfiguration configuration, String additionalArgs, boolean requestInfo) throws CoreException {
        String[] cmdLine = createCommandLine(configuration, additionalArgs);

        if (requestInfo) {
            int i = ArrayUtils.indexOf(cmdLine, "-c");
            if (i >= 0)
            	cmdLine[i] = "-x";
            else
            	cmdLine = (String[]) ArrayUtils.addAll(cmdLine, new String[] {"-x", "General"});
        }

        IStringVariableManager varman2 = VariablesPlugin.getDefault().getStringVariableManager();
        String wdAttr = LaunchPlugin.getWorkingDirectoryPath(configuration).toString();
        String expandedWd = varman2.performStringSubstitution(wdAttr);
        String environment[] = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
        return DebugPlugin.exec(cmdLine, new File(expandedWd), environment);
    }

    /**
     * TODO document
     */
    public static String[] createCommandLine(ILaunchConfiguration configuration, String additionalArgs) throws CoreException {
        IStringVariableManager varman = VariablesPlugin.getDefault().getStringVariableManager();
        String projAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
        String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
        String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
        String expandedProj = varman.performStringSubstitution(projAttr);
        String expandedProg = varman.performStringSubstitution(progAttr);
        String expandedArg = varman.performStringSubstitution(argAttr);
        // put the additional arguments at the beginning so they override the other arguments
        expandedArg = additionalArgs +" "+expandedArg;
        IFile executableFile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(expandedProj).append(expandedProg));
        if (executableFile == null)
            throw new CoreException(Status.CANCEL_STATUS);
        String cmdLine[] =DebugPlugin.parseArguments(executableFile.getRawLocation().makeAbsolute().toString() + " " + expandedArg);
        return cmdLine;
    }

    /**
     * Returns a string describing all runs in the scenario, or "" if an error occurred
     */
    public static String getSimulationRunInfo(ILaunchConfiguration configuration) {
        try {
        	 Process proc = LaunchPlugin.startSimulationProcess(configuration, "-u Cmdenv -g", true);
             final int BUFFERSIZE = 8192;
             byte bytes[] = new byte[BUFFERSIZE];
             StringBuffer stringBuffer = new StringBuffer(BUFFERSIZE);
             BufferedInputStream is = new BufferedInputStream(proc.getInputStream(), BUFFERSIZE);
             int lastRead = 0;
             while((lastRead = is.read(bytes)) > 0) {
                 stringBuffer.append(new String(bytes, 0, lastRead));
             }

             //FIXME parse out errors: they are the lines that start with "<!>" -- e.g. inifile might contain a syntax error etc
             // --Andras
             if (proc.exitValue() == 0)
                 return "Number of runs: "+StringUtils.trimToEmpty(StringUtils.substringBetween(stringBuffer.toString(), "Number of runs:", "End run of OMNeT++"));

        } catch (CoreException e) {
            LaunchPlugin.logError("Error starting the executable", e);
        } catch (IOException e) {
            LaunchPlugin.logError("Error getting output stream from the executable", e);
        }
        return "";
    }

    /**
     * Returns the number of runs available in the given scenario
     */
    public static int getMaxNumberOfRuns(ILaunchConfiguration configuration) {
        return NumberUtils.toInt(StringUtils.trimToEmpty(
                        StringUtils.substringBetween(
                                getSimulationRunInfo(configuration), "Number of runs:", "\n")), 1);
    }

    /**
     * TODO document
     * @param text The text to be parsed for progress information
     * @return The process progress reported in the text or -1 if no progress info found
     */
    public static int getProgressInPercent(String text) {
        String tag = "% completed";
        if (!StringUtils.contains(text, tag))
            return -1;
        String pctStr = StringUtils.substringAfterLast(StringUtils.substringBeforeLast(text, tag)," ");
        return NumberUtils.toInt(pctStr, -1);
    }

    /**
     * @param text The process output
     * @return Whether the end of the text indicates that the process is waiting fore user input
     */
    public static boolean isWaitingForUserInput(String text) {
        // FIXME this is not correct because the user can specify any prompt text.
        // we should have a consistent marker char/tag during user input
        return text.contains("Enter parameter");
    }
}
