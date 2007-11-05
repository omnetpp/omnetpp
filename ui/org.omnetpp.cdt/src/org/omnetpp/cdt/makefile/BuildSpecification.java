package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;

/**
 * Represents contents of the ".omnetppcproject" file
 * @author Andras
 */
public class BuildSpecification {
    public enum FolderType {GENERATED_MAKEFILE, SUPPLIED_MAKEFILE, EXCLUDED_FROM_BUILD};

    public static class FolderInfo {
        public FolderType folderType;
        public MakemakeOptions additionalMakeMakeOptions; //XXX refine
    }

    private String configFileLocation;
    private Map<IContainer,FolderInfo> folders = new HashMap<IContainer,FolderInfo>();

    public BuildSpecification() {
    }
    
    public String getConfigFileLocation() {
        return configFileLocation;
    }

    public void setConfigFileLocation(String configFileLocation) {
        this.configFileLocation = configFileLocation;
    }

    public FolderInfo getFolderInfo(IContainer folder) {
        return folders.get(folder);
    }

    public void setFolderInfo(IContainer folder, FolderInfo info) {
        folders.put(folder, info);
    }
    
    public void removeFolderInfo(IContainer folder) {
        folders.remove(folder);
    }
    
    public IContainer[] getFolders() {
        return folders.keySet().toArray(new IContainer[]{});
    }
    

}
