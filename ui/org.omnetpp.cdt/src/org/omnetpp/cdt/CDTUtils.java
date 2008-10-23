package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
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
     * Returns the source folders (ICSourceEntry) for the given project and the
     * active configuration.
     */
    public static List<IContainer> getSourceFolders(IProject project) {
        ICSourceEntry[] sourceEntries = CDTUtils.getSourceEntries(project);
        List<IContainer> sourceFolders = new ArrayList<IContainer>();
        for (ICSourceEntry sourceEntry : sourceEntries)
            sourceFolders.add(sourceEntry.getFullPath().isEmpty() ? project : project.getFolder(sourceEntry.getFullPath()));
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
 
    /**
     * Sets the project's source folders list to the given list of folders. 
     * (Previous source entries get overwritten.)  
     */
    public static void setSourceFolders(IProject project, IContainer[] folders) throws CoreException {
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project, true);
        int n = folders.length;
        for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
            ICSourceEntry[] entries = new CSourceEntry[n];
            for (int i=0; i<n; i++) {
                Assert.isTrue(folders[i].getProject().equals(project));
                entries[i] = new CSourceEntry(folders[i].getProjectRelativePath(), new IPath[0], 0);
            }
            try {
                configuration.setSourceEntries(entries);
            }
            catch (WriteAccessException e) {
                Activator.logError(e); // should not happen, as we called getProjectDescription() with write=true
            }
        }
        CoreModel.getDefault().setProjectDescription(project, projectDescription);
    }
    
    /** 
     * Replaces similar function in CDataUtil (CDT), because that one cannot properly
     * handle nested source folders. 
     * 
     * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=251846
     */
    public static boolean isExcluded(IPath path, ICSourceEntry[] entries){
        ICSourceEntry entry = getSourceEntryThatCovers(path, entries);
        return entry == null ? true : CDataUtil.isExcluded(path, entry);
    }
    
    public static ICSourceEntry getSourceEntryThatCovers(IPath path, ICSourceEntry[] entries) {
        while (path.segmentCount()>0) {
            for (int i = 0; i < entries.length; i++)
                if (path.equals(new Path(entries[i].getName())))
                    return entries[i];
            path = path.removeLastSegments(1);
        }
        return null; //none
    }

}
