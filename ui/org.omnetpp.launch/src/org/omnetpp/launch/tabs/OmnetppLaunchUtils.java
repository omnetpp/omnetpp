/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch.tabs;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.math.NumberUtils;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.ui.newui.CDTPropertyManager;
import org.eclipse.core.resources.IContainer;
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
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.Debug;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
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
    private static final String CDT_CC_NATURE_ID = "org.eclipse.cdt.core.ccnature";
    public static final String DEBUG_EXE_SUFFIX = "_dbg";

    private static final int HTTP_PORT_START = 9000;
    private static final int HTTP_PORT_END = 9999;

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
        IFile[] includeStack;

        public ConfigEnumeratorCallback(IFile file, Map<String, Section> result) {
            this(file, result, new IFile[0]);
        }

        protected ConfigEnumeratorCallback(IFile file, Map<String, Section> result, IFile[] includeStack) {
            this.currentFile = file;
            this.result = result;
            this.includeStack = ArrayUtils.add(includeStack, currentFile);
        }

        @Override
        public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String comment) {
            if (directive.equals("include")) {
                // recursively parse the included file
                try {
                    IFile file = currentFile.getParent().getFile(new Path(args));
                    if (!ArrayUtils.contains(includeStack, file)) // ignore recursive include
                        new InifileParser().parse(file, new ConfigEnumeratorCallback(file, result, includeStack));
                }
                catch (CoreException e) {
                    LaunchPlugin.logError("Error reading inifile", e);
                }
            }
        }

        @Override
        public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String comment) {
            if (currentSection == null) {  // invalid syntax (file must begin with section header) but let's accept it anyway
                currentSection = new Section();
                currentSection.name = "General";
                result.put(currentSection.name, currentSection);
            }

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
            List<Object> filteredChildren = new ArrayList<>();
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
            List<Object> filteredChildren = new ArrayList<>();
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
            List<Object> filteredChildren = new ArrayList<>();
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
     * Converts inputPath to be relative to referenceDir. This is not possible
     * if the two paths are from different devices, so in this case the method
     * returns the original inputPath unchanged.
     *
     * Copied from MakefileTools.
     *
     * @author andras
     */
    public static IPath makeRelativePathTo(IPath inputPath, IPath referenceDir) {
        Assert.isTrue(inputPath.isAbsolute());
        Assert.isTrue(referenceDir.isAbsolute());
        if (referenceDir.equals(inputPath))
            return new Path(".");
        if (!StringUtils.equals(inputPath.getDevice(), referenceDir.getDevice()))
            return inputPath;
        int commonPrefixLen = inputPath.matchingFirstSegments(referenceDir);
        int upLevels = referenceDir.segmentCount() - commonPrefixLen;
        return new Path(StringUtils.removeEnd(StringUtils.repeat("../", upLevels), "/")).append(inputPath.removeFirstSegments(commonPrefixLen));
    }

    /**
     * Replaces the configuration object inside the launch object with another one.
     */
    public static void updateLaunchConfigurationWithProgramAttributes(String mode, ILaunch launch)
                throws CoreException {
        ILaunchConfigurationWorkingCopy newConfig = createUpdatedLaunchConfig(launch.getLaunchConfiguration(), mode, true);
        replaceConfigurationInLaunch(launch, newConfig);
    }

    public static void replaceConfigurationInLaunch(ILaunch launch, ILaunchConfiguration configuration) {
        // the ILaunchConfiguration object in 'launch' is immutable so we have to create a mutable
        // ILaunchConfigurationWorkingCopy, update it with the required additional attributes and then
        // we MUST set this configuration on the 'launch' object. This is required because some launchers
        // notably the GdbLaunchDelegate uses the configuration INSIDE the launch object to set up the
        // launched process.
        ReflectionUtils.setFieldValue(launch, "fConfiguration", configuration);
    }

    /**
     * Fill a launch config working copy with default values, based on the given resource (which will
     * usually come from the workbench selection). Also sets the launch attributes required by CDT for debugging.
     */
    public static void setLaunchConfigDefaults(ILaunchConfigurationWorkingCopy configuration, IResource selectedResource) {
        String workingDir = "";
        String executable = "";
        String iniFile = "omnetpp.ini";
        String projectPath = "";

        // check the current selection and figure out the initial values if possible
        if (selectedResource != null) {
            if (selectedResource instanceof IFile) {
                IFile selFile = (IFile)selectedResource;
                workingDir = selFile.getParent().getFullPath().toString();
                // an ini file selected but (not omnetpp.ini) set the ini file name too
                if (StringUtils.equals("ini", selFile.getFileExtension()))
                    iniFile = selFile.getName();
                // if executable set the project and program name
                if (isExecutable(selFile))
                    executable = selFile.getFullPath().toString();
            }
            // if a directory or project is selected, use it as working dir
            if (selectedResource instanceof IContainer)
                workingDir = selectedResource.getFullPath().toString();

            projectPath = selectedResource.getProject().getFullPath().toString();
        }

        if (StringUtils.isEmpty(executable) && !StringUtils.isEmpty(workingDir))
            executable = getDefaultExeName(workingDir);

        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, projectPath);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, workingDir);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, executable);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, iniFile);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "${"+VAR_NED_PATH+":"+workingDir+"}");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, "${"+VAR_IMAGE_PATH+":"+workingDir+"}");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "${"+VAR_SHARED_LIBS+":"+workingDir+"}");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_BATCH_SIZE, 1);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "");
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH, IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH_DEPENDENCIES);
        configuration.setAttribute(IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD, IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_ASK);

        // minimal CDT specific attributes required to start without errors
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_ID, "gdb");
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_STOP_AT_MAIN, false);
        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUGGER_STOP_AT_MAIN_SYMBOL, "main");
        if (Platform.getOS().equals(Platform.OS_MACOSX))  // use the bundled lldb-mi wrapper instead of gdb
            configuration.setAttribute(IOmnetppLaunchConstants.ATTR_DEBUG_NAME, "lldbmi2");

        configuration.setAttribute(IOmnetppLaunchConstants.ATTR_GDB_INIT, IOmnetppLaunchConstants.OPP_GDB_INIT_FILE);
    }

    protected static String getDefaultExeName(String workingDir) {
        try {
            // expand "${opp_simprogs}", then return the first program from the list
            String progs = StringUtils.substituteVariables("${opp_simprogs:"+workingDir+"}");
            String [] splitProgs  = StringUtils.split(progs, ' ');
            if (splitProgs.length > 0)
                return splitProgs[0];
        }
        catch (CoreException e) {
            LaunchPlugin.logError(e);
        }
        return "";
    }

    /**
     * Creates a new temporary configuration from a simulation config type that is compatible with CDT.
     * It adds ATTR_PROJECT_NAME, ATTR_PROGRAM_NAME and ATTR_PROGRAM_ARGUMENTS. They are
     * required to start a program correctly with a debugger launch delegate.
     */
    @SuppressWarnings("unchecked")
    public static ILaunchConfigurationWorkingCopy createUpdatedLaunchConfig(ILaunchConfiguration config, String mode, boolean mergeStderr) throws CoreException {
        ILaunchConfigurationWorkingCopy newCfg = config.copy(config.getName());
        newCfg.setAttributes(CollectionUtils.getDeepCopyOf(newCfg.getAttributes())); // otherwise attrs that are Collections themselves are not copied

        boolean isDebugLaunch = mode.equals(ILaunchManager.DEBUG_MODE);

        // working directory (converted from path to location)
        String workingdirStr = config.getAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, "");
        if (StringUtils.isEmpty(workingdirStr))
            throw new CoreException(new Status(Status.ERROR, LaunchPlugin.PLUGIN_ID, "Working directory must be set"));
        newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, getLocationForWorkspacePath(workingdirStr, "/", false).toString());

        // absolute filesystem location of the working directory
        IPath workingdirLocation = getLocationForWorkspacePath(StringUtils.substituteVariables(workingdirStr),"/",false);

        // executable name
        String exePathAndName = config.getAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, "");
        String exeName = "";
        boolean isReleaseBinary = isReleaseBinaryRequired(workingdirStr);

        IProject project;
        if (StringUtils.isEmpty(exePathAndName)) {  // this means opp_run (i.e. using a shared library)
            exeName = OmnetppDirs.getOmnetppBinDir()+"/opp_run";
            // detect if the current executable is release or debug
            // if we run a debug executable we have to use opp_run_dbg (instead of opp_run)
            if (!isReleaseBinary)
                exeName += DEBUG_EXE_SUFFIX;
            // A CDT project is required for the launcher in debug mode to start the application (using gdb).
            project = findFirstRelatedCDTProject(workingdirStr);
            newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, project != null ? project.getName() : null);
            if (isDebugLaunch && project == null)
                throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Cannot launch simulation in debug mode: no related open C++ project"));
        }
        else {
            String projectName = new Path(exePathAndName).segment(0);
            project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
            newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, projectName);
            if (!isReleaseBinary) {
                // check if there is a project as otherwise we cannot debug
                if (!project.exists() || !project.hasNature(CDT_CC_NATURE_ID))
                    throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Cannot launch simulation in debug mode: the executable's project ("+projectName+") is not an open C++ project"));

                // special handling of debug launch name (attach _dbg if not present as we always try to start and exe with a _dbg ending)
                if (!exePathAndName.endsWith(DEBUG_EXE_SUFFIX))
                    exePathAndName += DEBUG_EXE_SUFFIX;

// FIXME We don't need this check because a missing executable is detected later.

//                IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(exePathAndName));
//                if (!file.exists())
//                    throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID,
//                            "Cannot launch simulation in debug mode: the executable ("+exePathAndName+") does not exist. " +
//                            "Make sure the active configuration is 'debug' and the last build was successful!"));
            } else {
                // test if the release executable exists.
//                IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(exePathAndName));
//                if (!file.exists()) {
//                    throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID,
//                            "Cannot launch simulation in run mode: the executable ("+exePathAndName+") does not exist. " +
//                            "Make sure the active configuration is 'release' and the last build was successful!"));
//                }
            }
            exeName = new Path(exePathAndName).removeFirstSegments(1).toString(); // project-relative path
        }

        if (Platform.getOS().equals(Platform.OS_WIN32) && !exeName.matches("(?i).*\\.(exe|cmd|bat)$"))
            exeName += ".exe";
        newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, exeName);

        String args = "";

        if (mergeStderr)
            args += " -m ";  // report errors on stdout, because Console does not guarantee correct ordering of stdout and stderr output

        String envirStr = config.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "").trim();
        if (StringUtils.isNotEmpty(envirStr) && !envirStr.equals(IOmnetppLaunchConstants.UI_DEFAULTEXTERNAL))
            args += " -u " + envirStr;

        int portNumber = config.getAttribute(IOmnetppLaunchConstants.OPP_HTTP_PORT, -1);
        if (portNumber != -1)
            args += " -p " + portNumber;

        String configStr = config.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").trim();
        if (StringUtils.isNotEmpty(configStr))
            args += " -c "+configStr;

        if (isDebugLaunch) {
            String runFilter = config.getAttribute(IOmnetppLaunchConstants.OPP_RUNFILTER, "").trim();
            if (!runFilter.isEmpty())
                args += " -r " + runFilter;
            // expand the GDB init file path so we can use also variables there (if needed)
            String expandedGdbInitFile = StringUtils.substituteVariables(config.getAttribute(IOmnetppLaunchConstants.ATTR_GDB_INIT, ""));
            newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_GDB_INIT, expandedGdbInitFile);

            createGDBInitTmpFile(project);
        }

        String pathSep = System.getProperty("path.separator");

        // NED path
        String nedpathStr = config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim();
        nedpathStr = StringUtils.substituteVariables(nedpathStr);
        if (StringUtils.isNotBlank(nedpathStr)) {
            String[] nedPaths = StringUtils.split(nedpathStr, pathSep);
            for (int i = 0 ; i< nedPaths.length; i++)
                nedPaths[i] = makeRelativePathTo(getLocationForWorkspacePath(nedPaths[i], workingdirStr, false), workingdirLocation).toString();
            // always create ned path option if more than one path element is present. Do not create if it contains a single . only (that's the default)
            if (nedPaths.length>1 || !".".equals(nedPaths[0]))
                args += " -n " + StringUtils.join(nedPaths, pathSep)+" ";
        }

        // image path
        String imagePathStr = config.getAttribute(IOmnetppLaunchConstants.OPP_IMAGE_PATH, "").trim();
        imagePathStr = StringUtils.substituteVariables(imagePathStr);
        if (StringUtils.isNotBlank(imagePathStr)) {
            String[] imagePaths = StringUtils.split(imagePathStr, pathSep);
            for (int i = 0 ; i< imagePaths.length; i++)
                imagePaths[i] = makeRelativePathTo(getLocationForWorkspacePath(imagePaths[i], workingdirStr, false), workingdirLocation).toString();
            // always create image path option if more than one path element is present. Do not create if it contains a single . only (that's the default)
            if (imagePaths.length>1 || !".".equals(imagePaths[0]))
                args += " --" + ConfigRegistry.CFGID_IMAGE_PATH.getName() + "=" + StringUtils.join(imagePaths, pathSep) + " ";
        }

        // shared libraries
        String shLibStr = config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim();
        shLibStr = StringUtils.substituteVariables(shLibStr);
        if (StringUtils.isNotBlank(shLibStr)) {
            String[] libs = StringUtils.split(shLibStr);
            // convert to file system location
            for (int i = 0 ; i< libs.length; i++) {
                libs[i] = makeRelativePathTo(getLocationForWorkspacePath(libs[i], workingdirStr, true), workingdirLocation).toString();
            }
            args += " -l " + StringUtils.join(libs," -l ")+" ";
        }

        // debug-on-errors: use setting in DEBUG mode, let omnetpp.ini setting through in RUN mode (maybe user wants to use external debugger with a RUN-mode launch)
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_DEBUG_ON_ERRORS.getName(), isDebugLaunch ? config.getAttribute(IOmnetppLaunchConstants.OPP_DEBUGMODE_DEBUG_ON_ERRORS, "true") : "");

        if (config.getAttribute(IOmnetppLaunchConstants.OPP_SILENT, false))
            args += " -s ";

        // time limits
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_SIM_TIME_LIMIT.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_SIM_TIME_LIMIT, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_CPU_TIME_LIMIT.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_CPU_TIME_LIMIT, ""));

        // other options
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_CMDENV_REDIRECT_OUTPUT.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_CMDENV_REDIRECT_STDOUT, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_RECORD_EVENTLOG.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_EVENTLOG, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_SCALAR_RECORDING.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_SCALARS, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_VECTOR_RECORDING.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_RECORD_VECTORS, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_CMDENV_EXPRESS_MODE.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_CMDENV_EXPRESS_MODE, ""));
        args += getOptionalConfigOptionArg(ConfigRegistry.CFGID_CMDENV_STOP_BATCH_ON_ERROR.getName(), config.getAttribute(IOmnetppLaunchConstants.OPP_STOP_BATCH_ON_ERROR, ""));

        // ini files
        String iniStr = config.getAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, "").trim();
        iniStr = StringUtils.substituteVariables(iniStr);
        if (StringUtils.isNotBlank(iniStr)) {
            String[] inifiles = StringUtils.split(iniStr);
            // convert to file system location
            for (int i = 0 ; i< inifiles.length; i++)
                inifiles[i] = makeRelativePathTo(getLocationForWorkspacePath(inifiles[i], workingdirStr, true), workingdirLocation).toString();
            args += " " + StringUtils.join(inifiles," ")+" ";
        }

        // set the program arguments
        String additionalArgs = config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "");
        newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, args + additionalArgs); // note: "additionalArgs" comes last, to allow the user override settings in "args"

        // handle environment: add OMNETPP_BIN and (DY)LD_LIBRARY_PATH
        Map<String, String> envir = newCfg.getAttribute("org.eclipse.debug.core.environmentVariables", new HashMap<String, String>());
        String path = envir.get("PATH");
        // if the path was not set by hand, generate automatically
        if (StringUtils.isBlank(path)) {
        String win32_ld_library_path = Platform.getOS().equals(Platform.OS_WIN32) ? StringUtils.substituteVariables("${opp_ld_library_path_loc:"+workingdirStr+"}" + pathSep) : "";
            envir.put("PATH",win32_ld_library_path +
                             StringUtils.substituteVariables("${opp_bin_dir}" + pathSep) +
                             StringUtils.substituteVariables("${opp_additional_path}" + pathSep) +  // msys/bin, mingw/bin, etc
                             StringUtils.substituteVariables("${env_var:PATH}"));
        }

        if (!Platform.getOS().equals(Platform.OS_WIN32)) {
            String ldLibPathVar = Platform.getOS().equals(Platform.OS_MACOSX) ? "DYLD_LIBRARY_PATH" : "LD_LIBRARY_PATH";
            String ldLibPath = envir.get(ldLibPathVar);
            // if the path was not set by hand, generate automatically
            if (StringUtils.isBlank(ldLibPath))
                envir.put(ldLibPathVar, StringUtils.substituteVariables("${opp_lib_dir}"+pathSep) +
                          StringUtils.substituteVariables("${opp_ld_library_path_loc:"+workingdirStr+"}"+pathSep) +
                          StringUtils.substituteVariables("${env_var:"+ldLibPathVar+"}"));
        }

        String imagePath = envir.get("OMNETPP_IMAGE_PATH");
        if (StringUtils.isBlank(imagePath)) {
            imagePath = CommonPlugin.getConfigurationPreferenceStore().getString(IConstants.PREF_OMNETPP_IMAGE_PATH);
            if (StringUtils.isNotBlank(imagePath))
                envir.put("OMNETPP_IMAGE_PATH", imagePath);
        }

        // Java CLASSPATH
        //FIXME do not overwrite CLASSPATH if it's already set by the user!
        IResource[] resources = newCfg.getMappedResources();
        if (resources != null && resources.length != 0) {
            String javaClasspath = getJavaClasspath(OmnetppLaunchUtils.getMappedProject(newCfg));
            if (javaClasspath != null)
                envir.put("CLASSPATH", javaClasspath);
        }

        newCfg.setAttribute("org.eclipse.debug.core.environmentVariables", envir);
        // do we need this ??? : configuration.setAttribute("org.eclipse.debug.core.appendEnvironmentVariables", true);

        return newCfg;
    }

    private static String getOptionalConfigOptionArg(String configOptionName, String value) {
        if (value.isEmpty())
            return "";
        return " --" + configOptionName + "=" + value + " ";
    }

    /**
     * Creates a file in the tmp folder that sources all gdbinit.py files in the root of
     * all the referencing projects (including the project itself)
     * This temp file is actually sourced by the main gdbinit.py file (in misc/gdb)
     */
    private static void createGDBInitTmpFile(IProject project) {
        try {
            IProject[] projects = ProjectUtils.getAllReferencedProjects(project, true, true);
            String tmpGDBInit ="";
            for (IProject p : projects) {
                String gdbInit = p.getLocation().addTrailingSeparator().append(".gdbinit.py").toString();
                if (new File(gdbInit).isFile())
                    tmpGDBInit += "exec(open('"+gdbInit+"').read(), {}, {'__file__':'"+gdbInit+"'})\n";
            }
            String tmpDir = System.getProperty("java.io.tmpdir");
            FileUtils.writeTextFile(tmpDir+"/gdbinit.tmp", tmpGDBInit, null);  // NOTE: the file name must match with the file name in misc/gdb/gdbinit.py
        } catch (CoreException e) {
            LaunchPlugin.logError(e);
        } catch (IOException e) {
            LaunchPlugin.logError(e);
        }
    }

    /**
     * Return "debug", "release" or whatever mode the CDT project has.
     * Returns null if the detection was unsuccessful.
     * @throws CoreException
     */
    private static String getCdtProjectActiveMode(IProject project) throws CoreException {
        Assert.isTrue(project.hasNature(CDT_CC_NATURE_ID));

        // Config IDs look like "org.omnetpp.cdt.gnu.config.release", "org.omnetpp.cdt.gnu.config.debug",
        // or something; see IDs defined in the plugin.xml.
        //
        // NOTE: At many other places in the IDE we simply check the config's name (whether it contains the string "release" or "debug")
        //
        String CONFIGID_PREFIX = "org.omnetpp.cdt.gnu.config.";

        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IConfiguration cfg = buildInfo == null ? null : buildInfo.getDefaultConfiguration();
        while (cfg != null) {
            String configId = cfg.getId();
            if (configId.startsWith(CONFIGID_PREFIX)) {
                String rest = configId.substring(CONFIGID_PREFIX.length());
                if (!rest.contains("."))
                    return rest;
            }
            cfg = cfg.getParent();
        }

        // cannot detect
        return null;
    }

    /**
     * Searches for the first free TCP port number on localhost, and returns it.
     * Returns -1 if no available TCP port was found.
     */
    public static int findFirstAvailableTcpPort(int startPort, int endPort) {
        for (int port = startPort; port <= endPort; port++) {
            try {
                ServerSocket sock = new ServerSocket(port);
                sock.close();
                return port;
            }
            catch (SocketException e) {
                // ignore, and go on to next port
            }
            catch (Exception e) {
                return -1; // unexpected error -- give up
            }
        }
        return -1;  // no available port found
    }

    /**
     * Check if we have to use a release binary for the project specified by the path.
     * Checks all dependent CDT projects and throws an exception if either of them use different
     * build modes. (i.e. all of the CDT projects must be built as debug OR release)
     */
    private static boolean isReleaseBinaryRequired(String workingDirPath) throws CoreException {
        String resolvedWorkingDir = StringUtils.substituteVariables(workingDirPath);
        IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(resolvedWorkingDir);
        if (resource == null)
            throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Cannot launch simulation: the working directory path is not accessible."));

        IProject[] projects = ProjectUtils.getAllReferencedProjects(resource.getProject(), false, true);

        Map<String,String> projectsByMode = new HashMap<>();
        for (IProject project : projects) {
            if (project.hasNature(CDT_CC_NATURE_ID)) {
                String mode = getCdtProjectActiveMode(project);
                if (mode == null)
                    mode = "unknown";
                if (!projectsByMode.containsKey(mode))
                    projectsByMode.put(mode, project.getName());
                else
                    projectsByMode.put(mode, projectsByMode.get(mode) + " " + project.getName()); // append
            }
        }

        boolean inconsistency = projectsByMode.size() != 1;
        if (inconsistency) {
            String report = projectsByMode.entrySet().stream().map(e -> e.getValue() + ": " + e.getKey()).collect(Collectors.joining("\n"));
            throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Make sure to select consistent build configurations "
                    + "across all involved projects, i.e. they should be all debug or all release. Current settings:\n\n" + report));

        }
        boolean allProjectsAreRelease = projectsByMode.containsKey("release") && !inconsistency;
        return allProjectsAreRelease; // if mode not detected or there are other modes, use debug mode
    }

    /**
     * Returns a related CDT project or null if there is no open and related CDT project.
     */
    private static IProject findFirstRelatedCDTProject(String workspacePath) throws CoreException {
        String resolvedWorkingDir = StringUtils.substituteVariables(workspacePath);
        IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(resolvedWorkingDir);
        if (resource == null)
            return null;
        IProject[] projects = ProjectUtils.getAllReferencedProjects(resource.getProject(), false, true);
        for (IProject project : projects)
            if (project.hasNature(CDT_CC_NATURE_ID))
                return project;
        return null;
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
            LaunchPlugin.logError("Cannot determine CLASSPATH for project with Java nature: JDT not available. Install JDT or set CLASSPATH manually.", e);
            return null;
        }

        // FIXME return also exported libraries (JAR files) from the project
        IPath javaOutputLocation = (IPath)ReflectionUtils.invokeMethod(javaNature, "getOutputLocation");
        String result = ResourcesPlugin.getWorkspace().getRoot().getFile(javaOutputLocation).getLocation().toOSString();

        for (IProject referencedProject : project.getReferencedProjects())
            result += File.pathSeparator + getJavaClasspath(referencedProject);

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
        IPath location = null;
        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
        if (isFile)
            location = workspaceRoot.getFile(path).getLocation();
        else if (path.segmentCount()>=2)
            location = workspaceRoot.getFolder(path).getLocation();
        else if (path.segmentCount()==1)
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
                String expandedLocation = StringUtils.substituteVariables(location);
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
     * Starts the simulation program.
     */
    public static Process startSimulationProcess(ILaunchConfiguration configuration, String[] cmdLine) throws CoreException {
        String environment[] = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
        Debug.println("startSimulationProcess(): " + StringUtils.join(cmdLine, " "));
        return DebugPlugin.exec(cmdLine, new File(getWorkingDirectoryPath(configuration).toString()), environment);
    }

    /**
     * Create a full command line (including executable path/name) from the provided configuration.
     * Additional arguments are right after the exe name so they can override the parameters
     * specified in the configuration.
     */
    public static String[] createCommandLine(ILaunchConfiguration configuration, String additionalArgs) throws CoreException {
        String projAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
        String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
        String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
        String expandedProj = StringUtils.substituteVariables(projAttr);
        String expandedProg = StringUtils.substituteVariables(progAttr);
        String expandedArg = StringUtils.substituteVariables(argAttr);
        IPath progPath = new Path(expandedProg);
        // put the additional arguments at the beginning so they override the other arguments
        expandedArg = additionalArgs + " " + expandedArg;
        String programLocation = expandedProg;
        // if it is workspace relative path, resolve it against the workspace and get the physical location
        if (!progPath.isAbsolute() ) {
            IFile executableFile = ResourcesPlugin.getWorkspace().getRoot().getProject(expandedProj).getFile(progPath);
            programLocation = executableFile.getRawLocation().makeAbsolute().toString();
        }
        String cmdLine[] = DebugPlugin.parseArguments(programLocation + " " + expandedArg);
        Debug.println();
        return cmdLine;
    }

    /**
     * Runs the simulation with the '-q runnumbers' option, and returns the resulting run numbers.
     */
    public static List<Integer> queryRunNumbers(ILaunchConfiguration configuration, String runFilter) throws CoreException, InterruptedException {
        final String ERROR_PREFIX = "Could not resolve run filter by running the simulation program: ";
        try {
            String additionalArgs = "";
            if (configuration.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").isEmpty())
                additionalArgs += " -c General";
            if (!runFilter.isEmpty())
                additionalArgs += " -r " + StringUtils.quoteStringIfNeeded(runFilter);
            additionalArgs += " -s -q runnumbers";
            ProcessResult result = getSimulationOutput(configuration, additionalArgs);
            if (result.exitCode != 0)
                throw LaunchPlugin.wrapIntoCoreException(ERROR_PREFIX + "\n" + result.stderr.trim() + " (exit code " + result.exitCode + ")", null);
            String output = result.stdout.trim();
            if (!output.matches("[0-9 ]*"))
                throw LaunchPlugin.wrapIntoCoreException(ERROR_PREFIX + "Unexpected output", null);
            ArrayList<Integer> runNumbers = new ArrayList<>();
            for (String token : output.split(" +"))
                if (token.length() > 0)
                    runNumbers.add(Integer.parseInt(token));
            return runNumbers;
        }
        catch (IOException e) {
            throw LaunchPlugin.wrapIntoCoreException(ERROR_PREFIX, e);
        }
    }

    /**
     * Returns a HTML string describing all runs in the scenario, or "" if an error occurred
     */
    public static String getSimulationRunInfo(ILaunchConfiguration configuration, String runFilter) {
        try {
            String additionalArgs = "";
            if (configuration.getAttribute(IOmnetppLaunchConstants.OPP_CONFIG_NAME, "").isEmpty())
                additionalArgs += " -c General";
            if (!runFilter.isEmpty())
                additionalArgs += " -r " + StringUtils.quoteStringIfNeeded(runFilter);
            additionalArgs += " -s -q runs ";

            ProcessResult result = getSimulationOutput(configuration, additionalArgs);
            String info;
            String allOutput = StringUtils.join(result.stdout.trim(), "\n", result.stderr.trim());
            allOutput = StringUtils.quoteForHtml(allOutput).replace("\n", "<br>\n");
            if (result.exitCode != 0) {
                // error case: show both stdout and stderr
                info = "Error retrieving list of runs from the simulation.<br>\n" + allOutput;
            }
            else {
                // normal case: show run count and run descriptions
                int numRuns = StringUtils.countMatches("\n" + result.stdout, "\nRun ");
                if (runFilter.isEmpty())
                    info = "Configuration contains " + numRuns + " " + (numRuns < 2 ? "run" : "runs") + ".";
                else
                    info = "Run filter matches " + numRuns + " " + (numRuns < 2 ? "run" : "runs") + ".";
                info += "<br>\n" + allOutput;
            }
            return info;
        }
        catch (CoreException e) {
            // do not litter the log with the error: this method often fails because it is often
            // called while the user edits the configuration, and moreover it is only used to
            // bring up a tooltip
        }
        catch (IOException e) {
            LaunchPlugin.logError("Error getting output stream from the executable", e);
        }
        catch (InterruptedException e) {
            LaunchPlugin.logError("Error: thread interrupted while waiting for process to exit", e);
        }
        return "";
    }

    static class ProcessResult {
        int exitCode;
        String stdout;
        String stderr;
    }

    /**
     * Returns the output from running the simulation with the given extra command-lime arguments
     */
    private static ProcessResult getSimulationOutput(ILaunchConfiguration configuration, String additionalArgs) throws CoreException, IOException, InterruptedException {
        long startTime = System.currentTimeMillis();
        configuration = createUpdatedLaunchConfig(configuration, ILaunchManager.RUN_MODE, false);
        Process process = startSimulationProcess(configuration, createCommandLine(configuration, additionalArgs));

        // read output
        ProcessResult result = new ProcessResult();
        String stdout = readFully(process.getInputStream());
        String stderr = readFully(process.getErrorStream());
        Debug.println("getSimulationOutput(): read " + stdout.length() + " bytes from stdout and " +
                stderr.length() + " bytes from stderr in " + (System.currentTimeMillis() - startTime) + "ms");

        result.stdout = stdout.replace("\r", ""); // CRLF to LF conversion
        result.stderr = stderr.replace("\r", "");

        // wait until it exits
        process.waitFor();
        result.exitCode = process.exitValue();
        Debug.println("getSimulationOutput(): program exited with code=" + result.exitCode + " after total " + (System.currentTimeMillis() - startTime) + "ms");
        return result;
    }

    private static String readFully(InputStream stream) throws IOException {
        final int BUFFERSIZE = 8192;
        byte bytes[] = new byte[BUFFERSIZE];
        StringBuffer stringBuffer = new StringBuffer(BUFFERSIZE);
        BufferedInputStream is = new BufferedInputStream(stream, BUFFERSIZE);
        int lastRead = 0;
        while ((lastRead = is.read(bytes)) > 0)
            stringBuffer.append(new String(bytes, 0, lastRead));
        return stringBuffer.toString();
    }

    /**
     * Parses and returns progress percentage from the given console output string.
     * @param text The text to be parsed for progress information
     * @return The process progress reported in the text or -1 if no progress info found
     */
    public static int getProgressInPercent(String text) {
        String tag = "% total)";
        if (!StringUtils.contains(text, tag))
            return -1;
        String pctStr = StringUtils.substringAfterLast(StringUtils.substringBeforeLast(text, tag),"(");
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

    public final static String VAR_NED_PATH = "opp_ned_path";
    public final static String VAR_SHARED_LIBS = "opp_shared_libs";
    public final static String VAR_IMAGE_PATH = "opp_image_path";

    /**
     * Walk through all projects and set the active configuration to match the launch mode, in case the
     * launch config was configured to do this. It can also prompt the user in a dialog whether to switch or not.
     */
    public static void setActiveProjectConfigurationsIfNeeded(ILaunchConfiguration config, String mode, IProject[] projects) throws CoreException {
        // set all dependent projects according to the current mode
        int switchBeforeBuildValue = config.getAttribute(IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD, IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_ASK);
        if (switchBeforeBuildValue != IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_NEVER) {
            boolean configSwitchRequested = true;
            String configNameBasedOnMode = ILaunchManager.DEBUG_MODE.equals(mode) ? "debug" : "release";

            boolean askUser = switchBeforeBuildValue == IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_ASK;
            if (askUser) {
                configSwitchRequested = false;  // assume we don't have to switch configs (all configs are set properly)
                for (IProject p : projects) {
                    ICProjectDescription projDesc = CDTPropertyManager.getProjectDescription(p);
                    // check all active configurations and report if switching is required.
                    if (projDesc != null && projDesc.getActiveConfiguration() != null)
                        configSwitchRequested |= !projDesc.getActiveConfiguration().getName()
                                                .toLowerCase().contains(configNameBasedOnMode.toLowerCase());
                }
                if (configSwitchRequested) {
                    final int[] dialogResult = new int[2];
                    Display.getDefault().syncExec(new Runnable() {
                        @Override
                        public void run() {
                            String title = ILaunchManager.DEBUG_MODE.equals(mode) ?
                                    "Switch to Debug Configuration?" : "Switch to Release Configuration?";
                            String message = ILaunchManager.DEBUG_MODE.equals(mode) ?
                                    "Switch to 'debug' build configuration before launching? Release mode binaries are not meant for debugging." :
                                    "Switch to 'release' build configuration before launching? Your simulation will run faster in release mode.";
                            MessageDialogWithToggle dialog = MessageDialogWithToggle.openYesNoQuestion(
                                    Display.getCurrent().getActiveShell(), title, message,
                                    "Remember decision for this launch configuration",
                                    false, null, null);
                            dialogResult[0] = dialog.getReturnCode();
                            dialogResult[1] = dialog.getToggleState() ? 1 : 0;
                        }
                    });
                    if (dialogResult[0] == IDialogConstants.CANCEL_ID || dialogResult[0] == -1) // -1 is for the escape key
                        throw new CoreException(Status.CANCEL_STATUS);

                    configSwitchRequested = (dialogResult[0] == IDialogConstants.YES_ID);
                    // change the configuration if the toggle was set to "do that in the fututre"
                    ILaunchConfigurationWorkingCopy wc = config.getWorkingCopy();
                    if (dialogResult[1] == 1) {  // if the user selected to persist the choice
                        wc.setAttribute(IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD,
                                dialogResult[0] == IDialogConstants.YES_ID ?
                                        IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_AUTO :
                                        IOmnetppLaunchConstants.OPP_SWITCH_CONFIG_BEFORE_BUILD_NEVER);
                        wc.doSave();
                    }
                }
            }

            if (configSwitchRequested) {  // switch projects that are incorrectly set
                for (IProject p : projects) {
                    ICProjectDescription projDesc = CDTPropertyManager.getProjectDescription(p);
                    if (projDesc != null) {
                        String activeConfigName = projDesc.getActiveConfiguration().getName().toLowerCase();
                        if (!activeConfigName.contains(configNameBasedOnMode.toLowerCase())) {  // do we have to switch?
                            ICConfigurationDescription newConfigDesc = getICConfigurationDescriptionForNameSubstring(projDesc, configNameBasedOnMode); // find the config that should be activated
                            if (newConfigDesc != null) {
                                newConfigDesc.setActive();
                                CDTPropertyManager.performOk(null);
                            } else {
                                // show an error dialog (cannot switch 0 or multiple config)
                                Display.getDefault().syncExec(new Runnable() {
                                    @Override
                                    public void run() {
                                        MessageDialog.openError(
                                                Display.getCurrent().getActiveShell(),
                                                "Incosistent Configuration Naming for '"+projDesc.getName()+"'",
                                                "Cannot find or identify "+configNameBasedOnMode+" configuration for project '"+projDesc.getName()+"'."
                                                + "\n\nTo be able to automatically switch between configurations, "
                                                + "they should be named in a way that one (and only one) "
                                                + "of them contains the word 'release' while the "
                                                + "other contains 'debug'.");
                                    }
                                });
                                throw new CoreException(Status.CANCEL_STATUS);
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Returns a project configuration that contains (case insensitive) the given substring in its name.
     * Returns null if there are 0 or >=2 such configurations.
     */
    public static ICConfigurationDescription getICConfigurationDescriptionForNameSubstring(
            ICProjectDescription projDesc, String substringInName) {

        ICConfigurationDescription selectedConfig = null;
        for (ICConfigurationDescription config : projDesc.getConfigurations())
            if (config.getName().toLowerCase().contains(substringInName.toLowerCase()))
                if (selectedConfig == null)
                    selectedConfig = config;  // first occurrence
                else
                    return null;              // more than one is matching

        return selectedConfig;
    }

    /**
     * Return the project mapped to the LaunchConfiguration.
     */
    public static IProject getMappedProject(ILaunchConfiguration config) throws CoreException {
        IResource[] res = config.getMappedResources();
        for (IResource r : res)
            if (r.exists() && r instanceof IProject)
                return (IProject)r;

        // fall back if no project was associated. Use the associated resoure's project itself
        return (res.length > 0) ? res[0].getProject() : null;
    }

}
