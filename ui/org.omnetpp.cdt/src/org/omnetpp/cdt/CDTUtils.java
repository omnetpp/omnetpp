package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IPath;

/**
 * Utility class dealing with CDT's managed build classes.
 * 
 * @author rhornig, andras
 */
public class CDTUtils {
    /**
     * Returns the currently activated configuration associated with the given project
     */
    public static IConfiguration getActiveConfiguration(IProject project) {
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        return buildInfo.getDefaultConfiguration();
    }
    
    /**
     * Returns the tool-chain associated with the currently active configuration.
     */
    public static IToolChain getActiveToolChain(IProject project) {
        return getActiveConfiguration(project).getToolChain();
    }
    
    /**
     * Checks if the currently active tool-chain is MSVC or not.
     */
    public static boolean isMsvcToolchainActive(IProject project) {
        // we identify the tool-chain by assuming that its id contains the string below.
        return getActiveToolChain(project).getId().contains(".msvc.");
    }

    /**
     * Returns the source entries (source folders with exclusion patterns) for 
     * the given project and the active configuration. Returns empty array
     * for non-CDT projects.
     */
    public static ICSourceEntry[] getSourceEntries(IProject project) {
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IConfiguration activeConfiguration = buildInfo==null ? null : buildInfo.getDefaultConfiguration();
        return activeConfiguration==null ? new ICSourceEntry[0] : activeConfiguration.getSourceEntries();
    }
    
    /**
     * Returns the source folders for the given project. Paths are workspace-relative 
     * (not project-relative as with ICSourceEntry).
     */
    public static List<IPath> getSourcePaths(IProject project) {
        ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntries(project);
        List<IPath> sourceFolders = new ArrayList<IPath>();
        for (ICSourceEntry i : sourceEntries)
            sourceFolders.add(project.getFullPath().append(i.getFullPath()));
        return sourceFolders;
    }
 
    /**
     * Returns the source entry which exactly corresponds to the given folder.
     */
    public static ICSourceEntry getSourceEntryFor(IContainer folder) {
        for (ICSourceEntry sourceEntry : getSourceEntries(folder.getProject()))
            if (sourceEntry.getFullPath().equals(folder.getProjectRelativePath()))
                return sourceEntry;
        return null;
    }
}
