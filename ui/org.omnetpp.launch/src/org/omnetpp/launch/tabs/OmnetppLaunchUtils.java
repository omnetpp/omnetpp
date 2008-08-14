package org.omnetpp.launch.tabs;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.swt.SWT;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.LaunchPlugin;

public class OmnetppLaunchUtils {
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
     */
    public static ILaunchConfigurationWorkingCopy convertLaunchConfig(ILaunchConfiguration config, String mode) 
    				throws CoreException {
		ILaunchConfigurationWorkingCopy newCfg = config.copy("opp_run temporary configuration");

		// working directory (converted from path to location
		String wdirStr = config.getAttribute(IOmnetppLaunchConstants.OPP_WORKING_DIRECTORY, "");
		newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, OmnetppLaunchUtils.path2location(new Path(wdirStr), new Path("/")).toString());
		
		// executable name
		String exeName = config.getAttribute(IOmnetppLaunchConstants.OPP_EXECUTABLE, "");
		if (StringUtils.isEmpty(exeName)) {
			// use the first library's project as main project
			newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, newCfg.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIB_PROJECT, ""));
			// no exe specified use opp_run and the first libraries project 
			exeName = OmnetppMainPlugin.getOmnetppBinDir()+"/opp_run"; 
		} else {
			newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, new Path(exeName).segment(0));
			exeName = new Path(exeName).removeFirstSegments(1).toString();
		}
		
		if(Platform.getOS().equals(Platform.OS_WIN32) && !exeName.endsWith(".exe"))
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

		String nedpathStr = config.getAttribute(IOmnetppLaunchConstants.OPP_NED_PATH, "").trim();
		// TODO convert to absolute location
		if (StringUtils.isNotEmpty(nedpathStr))
			args += " -n "+nedpathStr;

		// shared libraries
		String shLibStr = config.getAttribute(IOmnetppLaunchConstants.OPP_SHARED_LIBS, "").trim();
		shLibStr = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(shLibStr, false);
		if (StringUtils.isNotBlank(shLibStr)) {
			String[] libs = StringUtils.split(shLibStr);
			// convert to file system location
			for (int i = 0 ; i< libs.length; i++)
				libs[i] = path2location(new Path(libs[i]), new Path(wdirStr)).toString();
			args += " -l " + StringUtils.join(libs," -l ")+" ";
		}
		
		String iniStr = config.getAttribute(IOmnetppLaunchConstants.OPP_INI_FILES, "").trim();
		if (StringUtils.isNotEmpty(iniStr))
			args += " "+iniStr;

		// set the program arguments
		newCfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, 
				config.getAttribute(IOmnetppLaunchConstants.OPP_ADDITIONAL_ARGS, "/")+args);
				
		return newCfg;
	}
    
    /**
     * Convert a workspace path to absolute filesystem path (relative path is appended to the basePath before conversion)
     */
    public static final IPath path2location(IPath path, IPath basePath) {
    	if (!path.isAbsolute() && basePath != null)
    		path = basePath.append(path);
    	
    	IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(path);
    	// return the location of the resource
    	if (resource != null)
    		return resource.getLocation(); 
    	
    	// id resource do not exist it was an absolute location already, just return it
    	return path;
    }
}
