package org.omnetpp.simulation.ui;

import org.apache.commons.lang.StringUtils;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;

/**
 *
 * @author Andras
 */
public class ModulePathsMessageFilter implements IMessageFilter {
    private cModule[] modules;

    public ModulePathsMessageFilter(cModule[] modules) {
        this.modules = modules;
    }

    public boolean matches(cMessage msg) {
        cModule module = msg.getArrivalModule();
        if (!module.isFilledIn())
            module.safeLoad();
        String moduleFullPath = module.getFullPath();

        // check if moduleFullPath is the same (or prefix of) one of the filter module paths
        for (cModule mod : modules) {
            // try to avoid calling String.startsWith(), so first check last-but-one char, as that is
            // likely to differ (it's the last digit of the index if filter ends in a vector submodule)
            String filterModulePath = mod.getFullPath(); 
            int filterModulePathLength = filterModulePath.length();
            if (moduleFullPath.length() >= filterModulePathLength)
                if (filterModulePathLength < 2 || moduleFullPath.charAt(filterModulePathLength-2) == filterModulePath.charAt(filterModulePathLength-2))
                    if (moduleFullPath.startsWith(filterModulePath))  // this is the most expensive operation
                        if (moduleFullPath.length() == filterModulePathLength || moduleFullPath.charAt(filterModulePathLength)=='.')
                            return true;
        }
        return false;
    }

    @Override
    public String toString() {
        return StringUtils.join(modules, ",");
    }
}
