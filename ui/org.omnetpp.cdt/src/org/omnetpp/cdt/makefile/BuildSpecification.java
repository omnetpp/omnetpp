package org.omnetpp.cdt.makefile;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Represents contents of the OMNeT++ build specification file
 * @author Andras
 */
public class BuildSpecification {
    public static final String BUILDSPEC_FILENAME = ".oppbuildspec";

    private IProject project;
    private Map<IContainer,MakemakeOptions> folderMakemakeOptions = new HashMap<IContainer,MakemakeOptions>();

    protected BuildSpecification(IProject project) {
        this.project = project;
    }
    
    /** 
     * Returns the set of folders where a makefile will be generated.
     * (This is currently the same as the list of CDT source folders.)
     */
    public IContainer[] getMakemakeFolders() {
        // currently, we generate a Makefile in all folders designated as "CDT Source Entry"
        return CDTUtils.getSourceFolders(project).toArray(new IContainer[]{});
    }

    /**
     * Returns the options for the given folder. Never returns null. (If this folder
     * was not originally contained in the build spec, a new options object is
     * created with some default settings.)
     */
    public MakemakeOptions getMakemakeOptions(IContainer folder) {
        if (!folderMakemakeOptions.containsKey(folder))
            folderMakemakeOptions.put(folder, MakemakeOptions.createInitial());
        return folderMakemakeOptions.get(folder);
    }

    public void setMakemakeOptions(IContainer folder, MakemakeOptions options) {
        if (options == null)
            folderMakemakeOptions.remove(folder);
        else 
            folderMakemakeOptions.put(folder, options);
    }

    public static BuildSpecification createBlank(IProject project) {
        return new BuildSpecification(project);
    }

    /**
     * Reads the build spec file from the given OMNeT++ project. Returns null 
     * if there is no build spec file in the project.
     * 
     * @throws CoreException on I/O errors and build spec file errors (syntax, invalid option etc) 
     */
    public static BuildSpecification readBuildSpecFile(IProject project) throws CoreException {
        BuildSpecification buildSpec = new BuildSpecification(project);
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
     * Saves the build spec to file.
     */
    public void save() throws CoreException {
        // assemble file content to save
        String content = "version 4.0\n";
        for (IContainer folder : getMakemakeFolders()) {
            MakemakeOptions makemakeOptions = getMakemakeOptions(folder);
            String optionsText = makemakeOptions.toString();
            if (optionsText.trim().equals(""))
                optionsText = "--";
    
            if (!StringUtils.isEmpty(optionsText)) {
                String projectRelativePath = folder.equals(project) ? "." : folder.getProjectRelativePath().toString();
                content += projectRelativePath + ": " + optionsText;
            }
            content += "\n";
        }
    
        // save it
        IFile buildSpecFile = project.getFile(BUILDSPEC_FILENAME);
        if (!buildSpecFile.exists())
            buildSpecFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        else
            buildSpecFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
    }
}
