package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.util.FileUtils;

/**
 * Scans all C++ files, etc....
 * TODO
 *  
 * @author Andras
 */
public class CppTools {
    /**
     * Represents an #include in a C++ file
     */
    public static class Include {
        public String filename;
        public boolean isSysInclude; // true: <foo.h>, false: "foo.h" 

        public Include(String filename, boolean isSysInclude) {
            this.isSysInclude = isSysInclude;
            this.filename = filename;
        }

        @Override
        public String toString() {
            return isSysInclude ? ("<" + filename + ">") : ("\"" + filename + "\"");
        }
    }

    public void generateMakefiles(IContainer container, IProgressMonitor monitor) throws CoreException {
        Map<IFile, List<Include>> includes = processFilesIn(container, monitor);
        Map<IContainer,List<IContainer>> deps = calculateDependencies(includes);
    }

    /**
     * For each folder, it determines which other folders it depends on (i.e. includes files from).
     */
    public static Map<IContainer,List<IContainer>> calculateDependencies(Map<IFile,List<Include>> includes) {
        return null;
    }

    public static Map<IFile,List<Include>> processFilesIn(IContainer container, final IProgressMonitor monitor) throws CoreException {
        final Map<IFile,List<Include>> result = new HashMap<IFile,List<Include>>();
        container.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (isCppFile(resource)) {
                    monitor.subTask(resource.getFullPath().toString());
                    try {
                        IFile file = (IFile)resource;
                        List<Include> includes = CppTools.parseIncludes(file);
                        result.put(file, includes);
                    }
                    catch (IOException e) {
                        throw new RuntimeException("Could not process file " + resource.getFullPath().toString(), e);
                    }
                    monitor.worked(1);
                }
                if (monitor.isCanceled())
                    return false;
                return true;
            }
        });
        return result;
    }

    public static boolean isCppFile(IResource resource) {
        if (resource instanceof IFile) {
            //TODO: ask CDT about registered file extensions?
            String fileExtension = ((IFile)resource).getFileExtension();
            if ("cc".equalsIgnoreCase(fileExtension) || "cpp".equals(fileExtension) || "h".equals(fileExtension))
                return true;
        }
        return false;
    }

    /**
     * Collect #includes from a C++ source file
     */
    public static List<Include> parseIncludes(IFile file) throws CoreException, IOException {
        String contents = FileUtils.readTextFile(file.getContents()) + "\n";
        return parseIncludes(contents);
    }

    /**
     * Collect #includes from C++ source file contents
     */
    public static List<Include> parseIncludes(String source) {
        List<Include> result = new ArrayList<Include>();
        Matcher matcher = Pattern.compile("(?m)^\\s*#\\s*include\\s+([\"<])(.*?)[\">].*$").matcher(source);
        while (matcher.find()) {
            boolean isSysInclude = matcher.group(1).equals("<");
            String fileName = matcher.group(2);
            result.add(new Include(fileName, isSysInclude));
        }
        return result;
    }


}
