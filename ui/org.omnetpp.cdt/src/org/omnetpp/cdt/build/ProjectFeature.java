/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.LinkedHashSet;
import java.util.Set;

import org.eclipse.core.runtime.Assert;

/**
 * Immutable object that represents an OMNeT++ Project Feature.
 * @author Andras
 */
public class ProjectFeature {
    private String id;
    private String name;
    private String description;
    private boolean initiallyEnabled;
    private Set<String> labels = new LinkedHashSet<String>();
    private Set<String> dependencies = new LinkedHashSet<String>();
    private Set<String> nedPackages = new LinkedHashSet<String>();
    private Set<String> extraSourceFolders = new LinkedHashSet<String>(); // C++ folders outside those defined by NED packages
    private String compileFlags;
    private String linkerFlags;

    public ProjectFeature(
            String id,
            String name,
            String description,
            boolean initiallyEnabled,
            Set<String> labels,
            Set<String> dependencies,
            Set<String> nedPackages,
            Set<String> extraSourceFolders,
            String compileFlags,
            String linkerFlags)
    {
        this.id = id;
        this.name = name;
        this.description = description;
        this.initiallyEnabled = initiallyEnabled;
        this.labels = labels;
        this.dependencies = dependencies;
        this.nedPackages = nedPackages;
        this.extraSourceFolders = extraSourceFolders;
        this.compileFlags = compileFlags;
        this.linkerFlags = linkerFlags;

        Assert.isNotNull(id);
        Assert.isNotNull(name);
        Assert.isNotNull(description);
        Assert.isNotNull(labels);
        Assert.isNotNull(dependencies);
        Assert.isNotNull(nedPackages);
        Assert.isNotNull(extraSourceFolders);
        Assert.isNotNull(compileFlags);
        Assert.isNotNull(linkerFlags);
    }

    public static boolean isValidId(String id) {
        return id.matches("[a-zA-Z_][a-zA-Z0-9_]*");
    }

    /**
     * Feature ID. Also used as preprocessor symbol, with the "WITH_" prefix.
     */
    public String getId() {
        return id;
    }

    /**
     * Display name for this feature
     */
    public String getName() {
        return name;
    }

    /**
     * Detailed, typically multi-line information about this feature.
     */
    public String getDescription() {
        return description;
    }

    /**
     * Returns true of feature should be enabled initially.
     */
    public boolean getInitiallyEnabled() {
        return initiallyEnabled;
    }

    /**
     * Labels of this feature, e.g. "examples", "protocols", "routing", etc.
     * Suitable for filtering on the UI.
     */
    public Set<String> getLabels() {
        return labels;
    }

    /**
     * IDs of features this feature requires.
     */
    public Set<String> getDependencies() {
        return dependencies;
    }

    public Set<String> getNedPackages() {
        return nedPackages;
    }

    public Set<String> getExtraSourceFolders() {
        return extraSourceFolders;
    }

    public String getCompileFlags() {
        return compileFlags;
    }

    public String getLinkerFlags() {
        return linkerFlags;
    }

    @Override
    public String toString() {
        return getId();
    }
}
