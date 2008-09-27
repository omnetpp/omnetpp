package org.omnetpp.launch.tabs;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.math.NumberUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.swt.SWT;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.inifile.editor.model.ConfigRegistry;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

/**
 * Various utility methods for the launcher.
 */
public class OmnetppLaunchUtils {
    // copied from JavaCore.NATURE_ID (we don't want dependency on the JDT plugins) 
    private static final String JAVA_NATURE_ID = "org.eclipse.jdt.core.javanature"; 
    
	/**
	 * Reads the ini file and enumerates all config sections. resolves include directives recursively
	 */
	public static class ConfigEnumeratorCallback extends InifileParser.ParserAdapter {
		class Section {
			String name;
			String network;
			String extnds;
			String descr;
			@Override
			public String toString() {
				String additional = (StringUtils.isEmpty(descr)? "" : " "+descr)+
				(StringUtils.isEmpty(extnds)? "" : " (extends: "+extnds+")")+
				(StringUtils.isEmpty(network)? "" : " (network: "+network+")");
				return name +(StringUtils.isEmpty(additional) ? "" : " --"+additional);
			}
		}

		IFile currentFile;
		Section currentSection;
		Map<String, Section> result;

		public ConfigEnumeratorCallback(IFile file, Map<String, Section> result) {
			this.currentFile = file;
			this.result = result;
		}

		@Override
		public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String comment) {
			if (directive.equals("include")) {
				// recursively parse the included file
				try {
					IFile file = currentFile.getParent().getFile(new Path(args));
					new InifileParser().parse(file, new ConfigEnumeratorCallback(file, result));
				}
				catch (Exception e) {
					LaunchPlugin.logError("Error reading inifile: ", e);
				}
			}
		}

		@Override
		public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String comment) {
			if ("extends".equals(key))
				currentSection.extnds = value;

			if ("description".equals(key))
				currentSection.descr = value;

			if ("network".equals(key))
				currentSection.network = value;
		}

		@Override
		public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String comment) {
			String name = StringUtils.removeStart(sectionName,"Config ");
			if (result.containsKey(name))
				currentSection = result.get(name);
			else {
				currentSection = new Section();
				currentSection.name = name;
				result.put(name, currentSection);
			}
		}

	}

	/**
	 * A workbench content provider that returns only files with a given extension
	 */
	public static class FilteredWorkbenchContentProvider extends WorkbenchContentProvider {
		private final String regexp;

		/**
		 * @param regexp The regular expression where matches should be displayed
		 */
		public FilteredWorkbenchContentProvider(String regexp) {
			super();
			this.regexp = regexp;
		}

		@Override
		public Object[] getChildren(Object element) {
			List<Object> filteredChildren = new ArrayList<Object>();
			for (Object child : super.getChildren(element)) {
				if (child instanceof IFile && ((IFile)child).getName().matches(regexp)
						|| getChildren(child).length > 0)
					filteredChildren.add(child);
			}
			return filteredChildren.toArray();
		}
	};

	/**
	 * Content provider displaying possible projects in the workspace
	 * @author rhornig
	 */
	public static class ProjectWorkbenchContentProvider extends WorkbenchContentProvider {
		@Override
		public Object[] getChildren(Object element) {
			List<Object> filteredChildren = new ArrayList<Object>();
			for (Object child : super.getChildren(element)) {
				if (child instanceof IProject && ((IProject)child).isAccessible())
					filteredChildren.add(child);
			}
			return filteredChildren.toArray();
		}
	};

	/**
	 * A workbench content provider that returns only executable files
	 */
	public static class ExecutableWorkbenchContentProvider extends WorkbenchContentProvider {
		@Override
		public Object[] getChildren(Object element) {
			List<Object> filteredChildren = new ArrayList<Object>();
			for (Object child : super.getChildren(element)) {
				if (child instanceof IFile && isExecutable((IFile)child)
						|| getChildren(child).length > 0)
					filteredChildren.add(child);
			}
			return filteredChildren.toArray();
		}
	};

	/**
	 * Checks whether the resource is an executable file.
	 */
	public static boolean isExecutable(IResource file) {
		if (!(file instanceof IFile) || file.getResourceAttributes() == null)
			return false;
		return file.getResourceAttributes().isExecutable() ||
		StringUtils.containsIgnoreCase("exe.cmd.bat",file.getFileExtension()) && SWT.getPlatform().equals("win32");
	}

	/**
	 * Utility function: constructs a path that is relative to the relativeTo.
	 */
	public static IPath makeRelativePathTo(IPath path, IPath relativeTo) {

		int goUpLevels = relativeTo.segmentCount() - path.matchingFirstSegments(relativeTo);
		// walk up until the first common segment
		IPath resultPath = new Path("");
		for (int i=0; i<goUpLevels; ++i)
			resultPath = resultPath.append("../");
		// add the rest of the path (non common part)
		return resultPath.append(path.removeFirstSegments(path.matchingFirstSegments(relativeTo))).makeRelative();
	}

	/**
	 * Creates a new temporary configuration from a simulation config type that is compatible with CDT. 
	 * When an error happens during conversion, throws a CoreException.
	 */
	@SuppressWarnings("unchecked")
    public static ILaunchConfigurationWorkingCopy convertLaunchConfig(ILaunchConfiguration config, String mode) throws CoreException {
		ILaunchConfigurationWorkingCopy newCfg = config.copy("opp_run temporary configuration");

		// working directory (converted from path to location
		String wdirStr = config.getAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, "");
		newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, getLocationForWorkspacePath(wdirStr, "/", false).toString());

		// executable name
		String exeName = config.getAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, "");
		String projectName = null;
		if (StringUtils.isEmpty(exeName)) {
			// use the first library's project as main project
			projectName = newCfg.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIB_PROJECT, "");
			// no exe specified use opp_run and the first libraries project 
			exeName = OmnetppMainPlugin.getOmnetppBinDir()+"/opp_run"; 
		} else {
			projectName = new Path(exeName).segment(0);
			exeName = new Path(exeName).removeFirstSegments(1).toString();
		}
        newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, projectName);

		if (Platform.getOS().equals(Platform.OS_WIN32) && !exeName.matches("(?i).*\\.(exe|cmd|bat)$"))
			exeName += ".exe";
		newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, exeName);

		String args = ""; 
		String envirStr = config.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "").trim();
		if (StringUtils.isNotEmpty(envirStr))
			args += " -u "+envirStr;

		String configStr = config.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").trim();
		if (StringUtils.isNotEmpty(configStr))
			args += " -c "+configStr;

		if ( mode.equals( ILaunchManager.DEBUG_MODE ) ) {
			String runStr = config.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER_FOR_DEBUG, "").trim();
			if (StringUtils.isNotEmpty(runStr))
				args += " -r "+runStr;
		}

		// NED path
		String nedpathStr = config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim();
		nedpathStr = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(nedpathStr, false);
		if (StringUtils.isNotBlank(nedpathStr)) {
			String pathSep = System.getProperty("path.separator");
			String[] nedPaths = StringUtils.split(nedpathStr, pathSep);
			for (int i = 0 ; i< nedPaths.length; i++)
				nedPaths[i] = getLocationForWorkspacePath(nedPaths[i], wdirStr, false).toString();
			args += " -n " + StringUtils.join(nedPaths, pathSep)+" ";
		}

		// shared libraries
		String shLibStr = config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim();
		shLibStr = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(shLibStr, false);
		if (StringUtils.isNotBlank(shLibStr)) {
			String[] libs = StringUtils.split(shLibStr);
			// convert to file system location
			for (int i = 0 ; i< libs.length; i++)
				libs[i] = getLocationForWorkspacePath(libs[i], wdirStr, true).toString();
			args += " -l " + StringUtils.join(libs," -l ")+" ";
		}

		String recEventlog = config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, "").trim();
		if (StringUtils.isNotEmpty(recEventlog))
			args += "--"+ConfigRegistry.CFGID_RECORD_EVENTLOG.getKey()+" "+recEventlog+" ";

		String dbgOnErr = config.getAttribute(IOmnetppLaunchConstants.OPP_DEBUG_ON_ERRORS, "").trim();
		if (StringUtils.isNotEmpty(dbgOnErr))
			args += "--"+ConfigRegistry.CFGID_DEBUG_ON_ERRORS.getKey()+" "+dbgOnErr+" ";

		// ini files
		String iniStr = config.getAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, "").trim();
		iniStr = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(iniStr, false);
		if (StringUtils.isNotBlank(iniStr)) {
			String[] inifiles = StringUtils.split(iniStr);
			// convert to file system location
			for (int i = 0 ; i< inifiles.length; i++)
				inifiles[i] = getLocationForWorkspacePath(inifiles[i], wdirStr, true).toString();
			args += " " + StringUtils.join(inifiles," ")+" ";
		}

		// set the program arguments
		newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, 
				config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "")+args);

		// handle environment: add OMNETPP_BIN and LD_LIBRARY_PATH and CLASSPATH if necessary
        Map<String, String> envir = newCfg.getAttribute("org.eclipse.debug.core.environmentVariables", new HashMap<String, String>());
        String path = envir.get("PATH");
        // if the path was not set by hand, generate automatically
        if (StringUtils.isBlank(path))
        	envir.put("PATH", "${opp_bin_dir}${system_property:path.separator}${opp_ld_library_path_loc:"+wdirStr+"}${system_property:path.separator}${env_var:PATH}");

        String ldLibPath = envir.get("LD_LIBRARY_PATH");
        // if the path was not set by hand, generate automatically
        if (StringUtils.isBlank(ldLibPath))
        	envir.put("LD_LIBRARY_PATH", "${opp_lib_dir}${system_property:path.separator}${opp_ld_library_path_loc:"+wdirStr+"}${system_property:path.separator}${env_var:LD_LIBRARY_PATH}");

        // Java CLASSPATH
        IResource[] resources = newCfg.getMappedResources();
        if (resources != null && resources.length != 0) {
            String javaClasspath = getJavaClasspath(resources[0].getProject());
            if (javaClasspath != null)
                envir.put("CLASSPATH", javaClasspath);
        }

        newCfg.setAttribute("org.eclipse.debug.core.environmentVariables", envir);
        // do we need this ??? : configuration.setAttribute("org.eclipse.debug.core.appendEnvironmentVariables", true);

		return newCfg;
	}

	/**
	 * Returns the Java CLASSPATH based on the project settings where the class files are generated.
	 */
    public static String getJavaClasspath(IProject project) throws CoreException {
        if (!project.hasNature(JAVA_NATURE_ID))
	        return null;

        // We want to add the output folder (.class files) to the CLASSPATH. 
        // We can get it from the nature object, which is also an IJavaProject.
        // We use reflection because we don't want to add JDT to our plugin dependencies.
        // If JDT is not loaded, getNature() will throw a CoreException.
        IProjectNature javaNature;
        try {
            javaNature = project.getNature(JAVA_NATURE_ID);
        } catch (CoreException e) {
            //TODO should this become a show-once dialog?
            LaunchPlugin.logError("Cannot determine CLASSPATH for project with Java nature: JDT not available. Install JDT or set CLASSPATH manually.", e);
            return null;
        }
       
        IPath javaOutputLocation = (IPath)ReflectionUtils.invokeMethod(javaNature, "getOutputLocation");
        String result = ResourcesPlugin.getWorkspace().getRoot().getFile(javaOutputLocation).getLocation().toOSString();

        for (IProject referencedProject : project.getReferencedProjects())
            result += ";" + getJavaClasspath(referencedProject);

        return result;
	}

    /**
     * Convert a workspace path to absolute filesystem path. If pathString is a relative path,
     * it is prepended with the basePath before conversion. The named resource may be a file,
     * folder or project; and the file or folder does not need to exist in the workspace.
     * Throws CoreException if the path cannot be determined (see IResource.getLocation()).
     */
	public static final IPath getLocationForWorkspacePath(String pathString, String basePathString, boolean isFile) throws CoreException {
	    IPath path = new Path(pathString);
	    IPath basePath = StringUtils.isEmpty(basePathString) ? null : new Path(basePathString);
	    if (!path.isAbsolute() && basePath != null)
	        path = basePath.append(path);
	    IPath location;
	    IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
        if (isFile) 
	        location = workspaceRoot.getFile(path).getLocation();
	    else if (path.segmentCount()>=2)
	        location = workspaceRoot.getFolder(path).getLocation();
	    else
	        location = workspaceRoot.getProject(path.toString()).getLocation();
	    if (location == null)
	        throw new CoreException(new Status(Status.ERROR, LaunchPlugin.PLUGIN_ID, "Cannot determine location for "+pathString));
	    return location;
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

	/**
	 * Expands the provided run numbers into a array for example 1..4,7,9 to 1,2,3,4,7,9. 
	 * The "*" means ALL run number: 0-(maxRunNo-1). Empty runPar means: 0.
	 */
	public static boolean containsMultipleRuns(String runPar) throws CoreException  {
		return doParseRuns(runPar, -1).length > 1;
	}

	/**
	 * Expands the provided run numbers into a array for example 1..4,7,9 to 1,2,3,4,7,9. 
	 * The "*" means ALL run number: 0-(maxRunNo-1). Empty runPar means: 0.
	 */
	public static int[] parseRuns(String runPar, int maxRunNo) throws CoreException {
		Assert.isTrue(maxRunNo > 0);
		return doParseRuns(runPar, maxRunNo);
	}

	// Does not throw range error if maxRunNo = -1 and returns only the first two runs
	private static int[] doParseRuns(String runPar, int maxRunNo) throws CoreException {
		List<Integer> result = new ArrayList<Integer>();
		runPar = StringUtils.deleteWhitespace(runPar);
		if (StringUtils.isEmpty(runPar)) {
			// empty means: just the first run (0)
			result.add(0);
		}
		else if ("*".equals(runPar)) {
			if (maxRunNo == -1) 
				return new int[] {0, 1};
			// create ALL run numbers scenario
			for (int i=0; i<maxRunNo; i++)
				result.add(i);
		}
		else {
			// parse hand specified numbers
			for (String current : StringUtils.split(runPar, ',')) {
				try {
					if (current.contains("..")) {
						String lowerUpper[] = StringUtils.splitByWholeSeparator(current, "..");
						int low = 0;
						int high = maxRunNo < 0 ? Integer.MAX_VALUE : maxRunNo-1;
						if (lowerUpper.length > 0 && lowerUpper[0].length() > 0) 
							low = Integer.parseInt(lowerUpper[0]);

						// if we have an upper bound too
						if (lowerUpper.length > 1 && lowerUpper[1].length() > 0) 
							high = Integer.parseInt(lowerUpper[1]);

						if (low < 0 || low > high)
							throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Invalid run number or range: "+current)); 

						if (maxRunNo != -1 && high >= maxRunNo)
							throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Run number ("+current+") is greater than the number of runs ("+maxRunNo+") supported by the current configuration")); 


						// add all integers in the interval to the list
						int limitedHigh = maxRunNo == -1 ? low + 1 : high;

						for (int i = low; i<=limitedHigh; ++i)
							result.add(i);
					} 
					else {
						int number = Integer.parseInt(current);
						if (maxRunNo != -1 && number >= maxRunNo)
							throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Run number ("+current+") is greater than the number of runs ("+maxRunNo+") supported by the current configuration"));
						result.add(number);
					}
				} catch (NumberFormatException e) {
					throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Invalid run number or range: "+current+". The run number syntax: 0,2,7,9..11 or use * for all runs"));                
				}
			}
		}

		int[] resArray = new int [result.size()];
		for (int i = 0; i< resArray.length; i++)
			resArray[i] = result.get(i);

		// check for duplicate run numbers
		int[] sortedRes = ArrayUtils.clone(resArray);
		Arrays.sort(sortedRes);
		for(int i = 0; i< sortedRes.length-1; i++)
			if (sortedRes[i] == sortedRes[i+1])
				throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Duplicate run number: "+sortedRes[i]));                

		return resArray;
	}

	/**
	 * Starts the simulation program.
	 *
	 * @param configuration
	 * @param additionalArgs extra command line arguments to be prepended to the command line
	 * @param requestInfo Setting it to true runs the process in "INFO" mode (replaces the -c arg with -x)
	 * @return The created process object,
	 * @throws CoreException if process is not started correctly
	 */
	public static Process startSimulationProcess(ILaunchConfiguration configuration, String additionalArgs, boolean requestInfo) throws CoreException {
		String[] cmdLine = createCommandLine(configuration, additionalArgs);

		// System.out.println("starting with command line: "+StringUtils.join(cmdLine," "));
		
		if (requestInfo) {
			int i = ArrayUtils.indexOf(cmdLine, "-c");
			if (i >= 0)
				cmdLine[i] = "-x";   // replace the -c with -x
			else
				cmdLine = (String[]) ArrayUtils.addAll(cmdLine, new String[] {"-x", "General"});
		}

		IStringVariableManager varman2 = VariablesPlugin.getDefault().getStringVariableManager();
		String wdAttr = getWorkingDirectoryPath(configuration).toString();
		String expandedWd = varman2.performStringSubstitution(wdAttr);
		String environment[] = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
		return DebugPlugin.exec(cmdLine, new File(expandedWd), environment);
	}

	/**
	 * Create a full command line (including executable path/name) from the provided configuration.
	 * Additional arguments are right after the exe name so they can override the parameters
	 * specified in the configuration.
	 */
	public static String[] createCommandLine(ILaunchConfiguration configuration, String additionalArgs) throws CoreException {
		IStringVariableManager varman = VariablesPlugin.getDefault().getStringVariableManager();
		String projAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
		String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
		String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
		String expandedProj = varman.performStringSubstitution(projAttr);
		String expandedProg = varman.performStringSubstitution(progAttr);
		String expandedArg = varman.performStringSubstitution(argAttr);
		IPath projPath = new Path(expandedProj);
		IPath progPath = new Path(expandedProg);
		// put the additional arguments at the beginning so they override the other arguments
		expandedArg = additionalArgs +" "+expandedArg;
		String programLocation = expandedProg;
		// if it is workspace relative path, resolve it against the workspace and get the physical location
		if (!progPath.isAbsolute() ) {
			IFile executableFile = ResourcesPlugin.getWorkspace().getRoot().getFile(projPath.append(progPath));
			if (executableFile == null)
				throw new CoreException(Status.CANCEL_STATUS);
			programLocation = executableFile.getRawLocation().makeAbsolute().toString();
		}
		String cmdLine[] =DebugPlugin.parseArguments(programLocation + " " + expandedArg);
		return cmdLine;
	}

	/**
	 * Returns a string describing all runs in the scenario, or "" if an error occurred
	 */
	public static String getSimulationRunInfo(ILaunchConfiguration configuration) {
		try {
			configuration = convertLaunchConfig(configuration, ILaunchManager.RUN_MODE);
			Process proc = OmnetppLaunchUtils.startSimulationProcess(configuration, "-u Cmdenv -g", true);
			final int BUFFERSIZE = 8192;
			byte bytes[] = new byte[BUFFERSIZE];
			StringBuffer stringBuffer = new StringBuffer(BUFFERSIZE);
			BufferedInputStream is = new BufferedInputStream(proc.getInputStream(), BUFFERSIZE);
			int lastRead = 0;
			while((lastRead = is.read(bytes)) > 0) {
				stringBuffer.append(new String(bytes, 0, lastRead));
			}

			proc.waitFor();

			//FIXME parse out errors: they are the lines that start with "<!>" -- e.g. inifile might contain a syntax error etc
			// --Andras
			if (proc.exitValue() == 0)
				return "Number of runs: "+StringUtils.trimToEmpty(StringUtils.substringBetween(stringBuffer.toString(), "Number of runs:", "End run of OMNeT++"));

		} catch (CoreException e) {
			// LaunchPlugin.logError("Error starting the executable", e);
			// its not a big deal if we cannot start the simulation. don't put anything in the log
		} catch (IOException e) {
			LaunchPlugin.logError("Error getting output stream from the executable", e);
		} catch (InterruptedException e) {
			LaunchPlugin.logError("Error: thread interrupted while waiting for process to exit", e);
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
