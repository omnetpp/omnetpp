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
import org.omnetpp.cdt.makefile.BuildSpecification.FolderInfo;
import org.omnetpp.cdt.makefile.BuildSpecification.FolderType;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Utilities to manage OMNeT++ projects.
 *
 * @author Andras
 */
public class BuildSpecUtils {
    private static final String BUILDSPEC_FILENAME = ".oppbuildspec";

    /**
     * Reads the build spec file from the given OMNeT++ project.
     */
    public static BuildSpecification readNedFoldersFile(IProject project) throws IOException, CoreException {
        BuildSpecification result = new BuildSpecification();
        IFile buildSpecFile = project.getFile(BUILDSPEC_FILENAME);
        if (buildSpecFile.exists()) {
            String contents = FileUtils.readTextFile(buildSpecFile.getContents());
            for (String line : StringUtils.splitToLines(contents)) {
                line = line.trim();
                if (line.equals("") || line.startsWith("#")) {
                    // ignore line
                }
                if (line.startsWith("version ")) {
                    //TODO version check
                }
                else {
                    Matcher matcher = Pattern.compile("(.*?):(.*?),(.*)").matcher(line);
                    if (matcher.matches()) {
                        String folderPath = matcher.group(1).trim();
                        String folderType = matcher.group(2).trim();
                        String optionString = matcher.group(3).trim();
                        
                        IContainer folder = project.getFolder(new Path(folderPath));
                        FolderInfo folderInfo = new FolderInfo();
                        folderInfo.folderType = FolderType.valueOf(folderType);
                        String[] options = optionString.split(" ");  //XXX honor any quotes
                        folderInfo.additionalMakeMakeOptions = new MakemakeOptions(folder.getLocation().toFile(), options);
                        result.setFolderInfo(folder, folderInfo);
                    }
                    else {
                        //XXX log: wrong line format
                        System.out.println("error reading " + buildSpecFile.getFullPath() + ": unrecognized line format: " + line);
                    }
                }
            }
        }
        return result;
    }

    /**
     * Saves the build spec file in the given OMNeT++ project.
     */
    public static void saveNedFoldersFile(IProject project, BuildSpecification spec) throws CoreException {
        // assemble file content to save
        String content = "version 4.0\n";
        for (IContainer folder : spec.getFolders()) {
            FolderInfo folderInfo = spec.getFolderInfo(folder);
            String[] args = folderInfo.additionalMakeMakeOptions.toArgs();
            String options = StringUtils.join(args, " "); //XXX add quotes if needed
            content += getProjectRelativePathOf(project, folder) + ": " + folderInfo.folderType + ", " + options + "\n";
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
