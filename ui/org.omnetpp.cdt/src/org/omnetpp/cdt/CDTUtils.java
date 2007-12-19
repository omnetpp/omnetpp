package org.omnetpp.cdt;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IProject;

/**
 * Utility class dealing with CDT's managed build classes.
 * @author rhornig
 */
public class CDTUtils {
    /**
     * Returns the currently activated configuration associated with the given project
     * @param project
     * @return
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
        // we identify the tool-chain by assuming that its id contains the below string.
        return getActiveToolChain(project).getId().contains(".msvc.");
    }

    /**
     * Returns empty array for non-CDT projects.
     */
    public static ICSourceEntry[] getSourceEntriesIfExist(IProject project) {
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IConfiguration activeConfiguration = buildInfo==null ? null : buildInfo.getDefaultConfiguration();
        return activeConfiguration==null ? new ICSourceEntry[0] : activeConfiguration.getSourceEntries();
    }

}
