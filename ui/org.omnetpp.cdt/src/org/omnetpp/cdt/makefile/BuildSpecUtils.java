package org.omnetpp.cdt.makefile;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Utilities to manage the C++ build specifications for OMNeT++ projects.
 *
 * @author Andras
 */
public class BuildSpecUtils {
    private static final String BUILDSPEC_FILENAME = ".oppbuildspec";

    /**
     * Reads the build spec file from the given OMNeT++ project. Returns null 
     * if there is no build spec file in the project.
     * 
     * @throws CoreException on I/O errors and build spec file errors (syntax, invalid option etc) 
     */
    public static BuildSpecification readBuildSpecFile(IProject project) throws CoreException {
        BuildSpecification buildSpec = new BuildSpecification();
        IFile buildSpecFile = project.getFile(BUILDSPEC_FILENAME);
        if (!buildSpecFile.exists()) {
            return null;
        }
        else {
            String contents;
            try {
                contents = FileUtils.readTextFile(buildSpecFile.getContents());
            } 
            catch (IOException e) {
                throw Activator.wrapIntoCoreException(e);
            }
            for (String line : StringUtils.splitToLines(contents)) {
                line = line.trim();
                if (line.equals("") || line.startsWith("#"))
                    continue;

                if (line.startsWith("version ")) {
                    //TODO version check
                }
                else {
                    Matcher matcher = Pattern.compile("(.*?):(.*)").matcher(line);
                    if (matcher.matches()) {
                        String folderPath = matcher.group(1).trim();
                        String args = matcher.group(2).trim();
                        
                        IContainer folder = folderPath.equals(".") ? project : project.getFolder(new Path(folderPath));
                        if (!StringUtils.isEmpty(args)) {
                            MakemakeOptions makemakeOptions = new MakemakeOptions(args);
                            if (!makemakeOptions.getParseErrors().isEmpty())
                                throw Activator.wrapIntoCoreException(new MakemakeException(buildSpecFile.getFullPath() + ": " + makemakeOptions.getParseErrors().get(0)));
                            buildSpec.setMakemakeOptions(folder, makemakeOptions);
                        }
                    }
                    else {
                        String message = buildSpecFile.getFullPath() + ": invalid line syntax: " + line;
                        throw Activator.wrapIntoCoreException(new RuntimeException(message));
                    }
                }
            }
            return buildSpec;
        }
    }

    /**
     * Saves the build spec file in the given OMNeT++ project.
     */
    public static void saveBuildSpecFile(IProject project, BuildSpecification buildSpec) throws CoreException {
        // assemble file content to save
        String content = "version 4.0\n";
        for (IContainer folder : buildSpec.getMakemakeFolders()) {
            String options = "";
            MakemakeOptions makemakeOptions = buildSpec.getMakemakeOptions(folder);
            options = makemakeOptions == null ? "" : makemakeOptions.toString();
            if (options.trim().equals(""))
                options = "--";

            if (!StringUtils.isEmpty(options))
                content += getProjectRelativePathOf(project, folder) + ": " + options;
            content += "\n";
        }

        // save it
        IFile buildSpecFile = project.getFile(BUILDSPEC_FILENAME);
        if (!buildSpecFile.exists())
            buildSpecFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        else
            buildSpecFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
    }

    private static String getProjectRelativePathOf(IProject project, IContainer container) {
        return container.equals(project) ? "." : container.getProjectRelativePath().toString();
    }

}
