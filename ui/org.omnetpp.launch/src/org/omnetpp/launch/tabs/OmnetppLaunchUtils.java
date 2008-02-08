package org.omnetpp.launch.tabs;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.SWT;
import org.eclipse.ui.model.WorkbenchContentProvider;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileParser;
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
        private boolean isExecutable(IFile file) {
            return file.getResourceAttributes().isExecutable() ||
                    StringUtils.contains("exe.cmd.bat",file.getFileExtension()) && SWT.getPlatform().equals("win32");
        }

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

}
