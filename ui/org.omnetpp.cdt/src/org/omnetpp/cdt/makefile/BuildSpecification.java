package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;

/**
 * Represents contents of the ".omnetppcproject" file
 * @author Andras
 */
public class BuildSpecification {
    // folder types:
    public static final int GENERATED_MAKEFILE = 0;
    public static final int SUPPLIED_MAKEFILE = 1;
    public static final int EXCLUDED_FROM_BUILD = 2;

    public static class FolderInfo {
        public int folderType;
        public String additionalMakeMakeOptions; //XXX refine
    }

    private String configuserLocation;
    private Map<IContainer,FolderInfo> folders = new HashMap<IContainer,FolderInfo>();

    public BuildSpecification() {
    }
    
    public String getConfiguserLocation() {
        return configuserLocation;
    }

    public void setConfiguserLocation(String configuserLocation) {
        this.configuserLocation = configuserLocation;
    }

    public FolderInfo getFolderInfo(IContainer folder) {
        return folders.get(folder);
    }

}
