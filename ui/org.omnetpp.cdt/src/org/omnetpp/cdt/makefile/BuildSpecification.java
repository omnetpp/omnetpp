package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IWorkspaceRoot;


/**
 * Represents contents of the OMNeT++ build specification file
 * @author Andras
 */
//XXX  - property sheet: make it possible to edit the "makefrag" file
public class BuildSpecification {
    public enum FolderType {GENERATED_MAKEFILE, CUSTOM_MAKEFILE, EXCLUDED_FROM_BUILD};

    private String configFileLocation;
    private Map<IContainer,FolderType> folderTypes = new HashMap<IContainer,FolderType>();
    private Map<IContainer,MakemakeOptions> folderOptions = new HashMap<IContainer,MakemakeOptions>();

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
        FolderType folderType = null;
        while ((folderType=folderTypes.get(folder)) == null && !(folder.getParent() instanceof IWorkspaceRoot))
            folder = folder.getParent();
        return folderType != null ? folderType : FolderType.GENERATED_MAKEFILE;
    }

    public MakemakeOptions getFolderOptions(IContainer folder) {
        // if folder options are not set, inherit from parent folder
        MakemakeOptions options = null;
        while ((options=folderOptions.get(folder)) == null && !(folder.getParent() instanceof IWorkspaceRoot))
            folder = folder.getParent();
        return options;
    }

    public void setFolderType(IContainer folder, FolderType folderType) {
        if (folderType == null)
            folderTypes.remove(folder);
        else 
            folderTypes.put(folder, folderType);
    }

    public void setFolderOptions(IContainer folder, MakemakeOptions options) {
        if (options == null)
            folderOptions.remove(folder);
        else 
            folderOptions.put(folder, options);
    }

    public boolean isFolderTypeInherited(IContainer folder) {
        return !folderTypes.containsKey(folder);
    }

    public boolean isFolderOptionsInherited(IContainer folder) {
        return !folderOptions.containsKey(folder);
    }

    public IContainer[] getFolders() {
        // return the union of the two hashtables
        Set<IContainer> set = new HashSet<IContainer>();
        set.addAll(folderTypes.keySet());
        set.addAll(folderOptions.keySet());
        return set.toArray(new IContainer[]{});
    }

}
