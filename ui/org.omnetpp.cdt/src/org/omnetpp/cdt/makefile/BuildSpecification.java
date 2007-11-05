package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IWorkspaceRoot;

/**
 * Represents contents of the ".omnetppcproject" file
 * @author Andras
 */
public class BuildSpecification {
    public enum FolderType {GENERATED_MAKEFILE, CUSTOM_MAKEFILE, EXCLUDED_FROM_BUILD};

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

    public FolderType getFolderType(IContainer folder) {
        // if folder type is not set, inherit it from parent folder
        while (!folders.containsKey(folder) && !(folder.getParent() instanceof IWorkspaceRoot))
                folder = folder.getParent();
        FolderInfo info = folders.get(folder);
        return info==null ? FolderType.GENERATED_MAKEFILE : info.folderType;
    }

    public MakemakeOptions getMakemakeOptions(IContainer folder) {
        FolderInfo info = folders.get(folder);
        return info==null ? null : info.additionalMakeMakeOptions;
    }

    public void setFolderInfo(IContainer folder, FolderType folderType, MakemakeOptions options) {
        FolderInfo info = new FolderInfo();
        info.folderType = folderType;
        info.additionalMakeMakeOptions = options;
        folders.put(folder, info);
    }
    
    public void removeFolderInfo(IContainer folder) {
        folders.remove(folder);
    }
    
    public IContainer[] getFolders() {
        return folders.keySet().toArray(new IContainer[]{});
    }
    

}
