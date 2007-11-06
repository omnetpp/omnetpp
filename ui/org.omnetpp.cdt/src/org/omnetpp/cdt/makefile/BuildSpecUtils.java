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
    public static BuildSpecification readBuildSpecFile(IProject project) throws IOException, CoreException {
        BuildSpecification buildSpec = new BuildSpecification();
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
                    Matcher matcher = Pattern.compile("(.*?):(.*?)(,(.*))?").matcher(line);
                    if (matcher.matches()) {
                        String folderPath = matcher.group(1).trim();
                        String folderType = matcher.group(2).trim();
                        String argString = StringUtils.nullToEmpty(matcher.group(4));
                        
                        IContainer folder = folderPath.equals(".") ? project : project.getFolder(new Path(folderPath));
                        FolderType type;
                        if (folderType.equals("custom"))
                            type = FolderType.CUSTOM_MAKEFILE;
                        else if (folderType.equals("exclude"))
                            type = FolderType.EXCLUDED_FROM_BUILD;
                        else if (folderType.equals("makemake"))
                            type = FolderType.GENERATED_MAKEFILE;
                        else
                            type = null; // inherited
                        if (type != null)
                            buildSpec.setFolderType(folder, type);
                        
                        String[] args = argString.split(" ");  //XXX honor any quotes
                        if (!StringUtils.isEmpty(argString)) {
                            MakemakeOptions makemakeOptions = new MakemakeOptions(folder.getLocation().toFile(), args);
                            buildSpec.setFolderOptions(folder, makemakeOptions);
                        }
                    }
                    else {
                        //XXX log: wrong line format
                        System.out.println("error reading " + buildSpecFile.getFullPath() + ": unrecognized line format: " + line);
                    }
                }
            }
        }
        return buildSpec;
    }

    /**
     * Saves the build spec file in the given OMNeT++ project.
     */
    public static void saveBuildSpecFile(IProject project, BuildSpecification buildSpec) throws CoreException {
        // assemble file content to save
        String content = "version 4.0\n";
        for (IContainer folder : buildSpec.getFolders()) {
            FolderType folderType = buildSpec.getFolderType(folder);
            String folderTypeText;
            if (buildSpec.isFolderTypeInherited(folder))
                folderTypeText = "-";
            if (folderType == FolderType.CUSTOM_MAKEFILE)
                folderTypeText = "custom";
            else if (folderType == FolderType.EXCLUDED_FROM_BUILD)
                folderTypeText = "exclude";
            else
                folderTypeText = "makemake";


            String options = "";
            if (!buildSpec.isFolderOptionsInherited(folder)) {
                MakemakeOptions makemakeOptions = buildSpec.getFolderOptions(folder);
                String[] args = makemakeOptions == null ? new String[0] : makemakeOptions.toArgs();
                options = StringUtils.join(args, " "); //XXX add quotes if needed
            }

            content += getProjectRelativePathOf(project, folder) + ": " + folderTypeText; 
            if (!StringUtils.isEmpty(options))
                content += ", " + options;
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
