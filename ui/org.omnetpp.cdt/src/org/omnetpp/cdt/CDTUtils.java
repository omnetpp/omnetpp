package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

/**
 * Utility class dealing with CDT's managed build classes.
 * 
 * @author rhornig, andras
 */
public class CDTUtils {
    /**
     * Checks if the currently active tool-chain is MSVC or not.
     * NOTE: Do NOT use from property pages, as it returns the saved state not the edited one.
     */
    public static boolean isMsvcToolchainActive(IProject project) {
        // we identify the tool-chain by assuming that its id contains the string below.
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IToolChain toolchain = buildInfo.getDefaultConfiguration().getToolChain();
        return toolchain.getId().contains(".msvc.");
    }

    /**
     * Returns the source entries (source folders with exclusion patterns) for 
     * the given project and the active configuration. Returns empty array
     * for non-CDT projects.
     * 
     * NOTE: Do NOT use from property pages (or from code like MetaMakemake that gets
     * invoked from property pages), as it returns the saved state not the edited one.
     */
    public static ICSourceEntry[] getSourceEntries(IProject project) {
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IConfiguration activeConfiguration = buildInfo==null ? null : buildInfo.getDefaultConfiguration();
        return activeConfiguration==null ? new ICSourceEntry[0] : activeConfiguration.getSourceEntries();
    }
    
    /**
     * Returns the source locations from the given source entries. (This method does not access CDT state.)
     */
    public static List<IContainer> getSourceLocations(IProject project, ICSourceEntry[] sourceEntries) {
        sourceEntries = CDataUtil.makeRelative(project, sourceEntries);
        List<IContainer> sourceFolders = new ArrayList<IContainer>();
        for (ICSourceEntry sourceEntry : sourceEntries)
            sourceFolders.add(sourceEntry.getFullPath().isEmpty() ? project : project.getFolder(sourceEntry.getFullPath()));
        return sourceFolders;
    }
 
    /**
     * Returns the source entry which exactly corresponds to the given folder.
     * (This method does not access CDT state.) 
     */
    public static ICSourceEntry getSourceEntryFor(IContainer folder, ICSourceEntry[] sourceEntries) {
        IPath folderPath = folder.getProjectRelativePath();
        for (ICSourceEntry sourceEntry : sourceEntries)
            if (CDataUtil.makeRelative(folder.getProject(), sourceEntry).getFullPath().equals(folderPath))
                return sourceEntry;
        return null;
    }

    /** 
     * Returns true if the given resource is excluded from its containing source location,
     * or is outside all source locations.
     * 
     * Replaces similar function in CDataUtil (CDT), because that one cannot properly
     * handle nested source folders.
     * 
     * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=251846
     * 
     * (This method does not access CDT state.)
     */
    public static boolean isExcluded(IResource resource, ICSourceEntry[] entries) {
        ICSourceEntry entry = getSourceEntryThatCovers(resource, entries);
        if (entry == null) 
            return true;
        entry = CDataUtil.makeRelative(resource.getProject(), entry);
        return CDataUtil.isExcluded(resource.getProjectRelativePath(), entry);
    }
    
    /**
     * Returns the source entry whose subdirtree the resource is located in, or null. 
     * Exclude patterns are ignored. (This method does not access CDT state.)
     */
    public static ICSourceEntry getSourceEntryThatCovers(IResource resource, ICSourceEntry[] entries) {
        entries = CDataUtil.makeRelative(resource.getProject(), entries);  // convert everything to relative path
        IPath path = resource.getProjectRelativePath();
        while (true) {
            for (int i = 0; i < entries.length; i++)
                if (path.equals(new Path(entries[i].getName())))
                    return entries[i];
            if (path.segmentCount()==0)
                break;
            path = path.removeLastSegments(1);
        }
        return null; //none
    }

    /**
     * Excludes/includes the given resource in the given source entries, and returns 
     * the modified source entries. (This method does not access CDT state.)
     */
    public static ICSourceEntry[] setExcluded(IResource resource, boolean exclude, ICSourceEntry[] sourceEntries) throws CoreException {
        sourceEntries = CDataUtil.makeRelative(resource.getProject(), sourceEntries);  // convert everything to relative path
        ICSourceEntry[] newEntries = CDataUtil.setExcluded(resource.getProjectRelativePath(), (resource instanceof IFolder), exclude, sourceEntries);
        return newEntries;
    }
}
