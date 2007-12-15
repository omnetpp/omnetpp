package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;

/**
 * A bridge between the IDE and opp_makemake. It takes meta-options in MakemakeOptions,
 * and translates them into opp_makemake options; translates CDT's "exclude from build"
 * options into opp_makemake -X options.
 *
 * @author Andras
 */
//FIXME exceptDirs must be implicit ("Excluded from build")
public class MetaMakemake {
    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public static boolean generateMakefile(IContainer folder, String args, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps, String configFileLocation) throws IOException, CoreException {
        return generateMakefile(folder, new MakemakeOptions(args), perFileDeps, configFileLocation);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public static boolean generateMakefile(IContainer folder, String[] argv, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps, String configFileLocation) throws IOException, CoreException {
        return generateMakefile(folder, new MakemakeOptions(argv), perFileDeps, configFileLocation);
    }

    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public static boolean generateMakefile(IContainer folder, MakemakeOptions options, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps, String configFileLocation) throws IOException, CoreException {
        MakemakeOptions translatedOptions = options.clone();

        // add -X option for each excluded folder in CDT
        //XXX makemake doesn't allow files to be excluded! issue warning if we find one? or support it in makemake?
        translatedOptions.exceptSubdirs.addAll(getExcludedPathsWithinFolder(folder));

        //TODO interpret meta-options
        
        System.out.println("Translated makemake options for " + folder + ": " + translatedOptions.toString());
        return new Makemake0().generateMakefile(folder, translatedOptions, perFileDeps, configFileLocation);
    }

    /**
     * Returns the (folder-relative) paths of directories and files excluded from build 
     * in the active CDT configuration under the given folder . 
     */
    protected static List<String> getExcludedPathsWithinFolder(IContainer folder) {
        List<String> result = new ArrayList<String>();
        IPath folderPath = folder.getFullPath();
        IProject project = folder.getProject();
        ICSourceEntry[] sourceEntries = ManagedBuildManager.getBuildInfo(project).getDefaultConfiguration().getSourceEntries();//XXX can something be null here?
        for (ICSourceEntry sourceEntry : sourceEntries) {
            IPath sourceFolderPath = project.getFullPath().append(sourceEntry.getFullPath());
            for (IPath exclusionPattern : sourceEntry.getExclusionPatterns()) {
                IPath exclusionPath = sourceFolderPath.append(exclusionPattern);
                if (folderPath.isPrefixOf(exclusionPath)) {
                    IPath subpath = exclusionPath.removeFirstSegments(folderPath.segmentCount());
                    result.add(subpath.segmentCount()==0 ? "." : subpath.toString());
                }
            }
        }
        return result;
    }
}
