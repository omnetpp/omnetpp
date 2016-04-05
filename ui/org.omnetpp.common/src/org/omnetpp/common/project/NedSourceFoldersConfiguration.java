package org.omnetpp.common.project;

import org.eclipse.core.resources.IContainer;
import org.omnetpp.common.util.StringUtils;

/**
 * Represents contents of a ".nedfolders" file
 *
 * @author Andras
 */
public class NedSourceFoldersConfiguration {
    private IContainer[] sourceFolders;
    private String[] excludedPackages;

    public NedSourceFoldersConfiguration(IContainer[] sourceFolders, String[] excludedPackages) {
        this.sourceFolders = sourceFolders;
        this.excludedPackages = excludedPackages;
    }

    public IContainer[] getSourceFolders() {
        return sourceFolders;
    }

    public void setSourceFolders(IContainer[] sourceFolders) {
        this.sourceFolders = sourceFolders;
    }

    public String[] getExcludedPackages() {
        return excludedPackages;
    }

    public void setExcludedPackages(String[] excludedPackages) {
        this.excludedPackages = excludedPackages;
    }

    @Override
    public String toString() {
        return "sourceFolders: [" + StringUtils.join(sourceFolders, " ") + "], excludedPackages: [" + StringUtils.join(excludedPackages, " ") + "]";
    }


}
