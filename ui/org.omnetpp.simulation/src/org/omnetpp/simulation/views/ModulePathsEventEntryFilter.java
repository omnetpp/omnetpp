package org.omnetpp.simulation.views;

import org.omnetpp.simulation.controller.EventEntry;

/**
 *
 * @author Andras
 */
public class ModulePathsEventEntryFilter implements IEventEntryFilter {
    private String[] moduleFullPaths;

    public ModulePathsEventEntryFilter(String[] moduleFullPaths) {
        this.moduleFullPaths = moduleFullPaths;
    }

    public boolean matches(EventEntry entry) {
        String moduleFullPath = entry.moduleFullPath;
        if (moduleFullPath == null)
            return false;

        // check if moduleFullPath is the same (or prefix of) one of the filter module paths
        for (String filterModule : moduleFullPaths) {
            // try to avoid calling String.startsWith(), so first check last-but-one char, as that is
            // likely to differ (it's the last digit of the index if filter ends in a vector submodule)
            int filterModuleLength = filterModule.length();
            if (moduleFullPath.length() >= filterModuleLength)
                if (filterModuleLength < 2 || moduleFullPath.charAt(filterModuleLength-2) == filterModule.charAt(filterModuleLength-2))
                    if (moduleFullPath.startsWith(filterModule))  // this is the most expensive operation
                        if (moduleFullPath.length() == filterModuleLength || moduleFullPath.charAt(filterModuleLength)=='.')
                            return true;
        }
        return false;
    }
    
    @Override
    public String toString() {
        return moduleFullPaths.toString();
    }
}
