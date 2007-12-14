package org.omnetpp.cdt.makefile;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;


/**
 * Represents contents of the OMNeT++ build specification file
 * @author Andras
 */
public class BuildSpecification {
    private String configFileLocation;
    private Map<IContainer,MakemakeOptions> folderOptions = new HashMap<IContainer,MakemakeOptions>();

    public BuildSpecification() {
    }
    
    public String getConfigFileLocation() {
        return configFileLocation;
    }

    public void setConfigFileLocation(String configFileLocation) {
        this.configFileLocation = configFileLocation;
    }

    /** 
     * Returns the set of folders for which there's some explicitly set
     * folder type or option. Child folders of those inherit the settings.
     */
    public IContainer[] getFolders() {
        return folderOptions.keySet().toArray(new IContainer[]{});
    }

    public boolean isMakemakeFolder(IContainer folder) {
        // FIXME should return true ONLY for folders within a deep makefiles source tree
//        return folderOptions.containsKey(folder);
        return true;
    }

    public MakemakeOptions getFolderOptions(IContainer folder) {
        return folderOptions.get(folder);
    }

    public void setFolderOptions(IContainer folder, MakemakeOptions options) {
        if (options == null)
            folderOptions.remove(folder);
        else 
            folderOptions.put(folder, options);
    }

}
