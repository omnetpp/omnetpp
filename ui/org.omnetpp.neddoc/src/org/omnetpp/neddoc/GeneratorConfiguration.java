/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.neddoc;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.omnetpp.common.util.StringUtils;

public class GeneratorConfiguration {
    private static final String GENERATE_NED_TYPE_FIGURES = "GenerateNedTypeFigures";
    private static final String GENERATE_PER_TYPE_USAGE_DIAGRAMS = "GeneratePerTypeUsageDiagrams";
    private static final String GENERATE_FULL_USAGE_DIAGRAMS = "GenerateFullUsageDiagrams";
    private static final String GENERATE_PER_TYPE_INHERITANCE_DIAGRAMS = "GeneratePerTypeInheritanceDiagrams";
    private static final String GENERATE_FULL_INHERITANCE_DIAGRAMS = "GenerateFullInheritanceDiagrams";
    private static final String GENERATE_NED_SOURCE_LISTINGS = "GenerateNedSourceListings";
    private static final String GENERATE_EXPLICIT_LINKS_ONLY = "GenerateExplicitLinks";
    private static final String GENERATE_EXCLUDED_DIRS = "GenerateExcludedPackages";
    private static final String GENERATE_DOXY = "GenerateDoxy";
    private static final String DOXY_SOURCE_BROWSER = "DoxySourceBrowser";
    private static final String OUTPUT_DIRECTORY_PATH = "OutputDirectoryPath";

    public IProject[] projects;

    public boolean generateNedTypeFigures = true;
    public boolean generatePerTypeUsageDiagrams = true;
    public boolean generateFullUsageDiagrams = false;
    public boolean generatePerTypeInheritanceDiagrams = true;
    public boolean generateFullInheritanceDiagrams = false;
    public boolean nedSourceListings = true;
    public boolean generateExplicitLinksOnly = false;
    public String excludedDirs;

    public boolean generateDoxy = true;
    public boolean cppSourceListings = false;

    public String outputDirectoryPath;

    public void store(IDialogSettings settings) {
        settings.put(GENERATE_NED_TYPE_FIGURES, generateNedTypeFigures);
        settings.put(GENERATE_PER_TYPE_USAGE_DIAGRAMS, generatePerTypeUsageDiagrams);
        settings.put(GENERATE_FULL_USAGE_DIAGRAMS, generateFullUsageDiagrams);
        settings.put(GENERATE_PER_TYPE_INHERITANCE_DIAGRAMS, generatePerTypeInheritanceDiagrams);
        settings.put(GENERATE_FULL_INHERITANCE_DIAGRAMS, generateFullInheritanceDiagrams);
        settings.put(GENERATE_NED_SOURCE_LISTINGS, nedSourceListings);
        settings.put(GENERATE_EXPLICIT_LINKS_ONLY, generateExplicitLinksOnly);
        settings.put(GENERATE_EXCLUDED_DIRS, excludedDirs);

        settings.put(GENERATE_DOXY, generateDoxy);
        settings.put(DOXY_SOURCE_BROWSER, cppSourceListings);

        settings.put(OUTPUT_DIRECTORY_PATH, outputDirectoryPath);
    }

    public void restore(IDialogSettings settings) {
        if (settings.get(GENERATE_NED_TYPE_FIGURES) != null)
            generateNedTypeFigures = settings.getBoolean(GENERATE_NED_TYPE_FIGURES);
        if (settings.get(GENERATE_PER_TYPE_USAGE_DIAGRAMS) != null)
            generatePerTypeUsageDiagrams = settings.getBoolean(GENERATE_PER_TYPE_USAGE_DIAGRAMS);
        if (settings.get(GENERATE_PER_TYPE_INHERITANCE_DIAGRAMS) != null)
            generatePerTypeInheritanceDiagrams = settings.getBoolean(GENERATE_PER_TYPE_INHERITANCE_DIAGRAMS);
        if (settings.get(GENERATE_FULL_USAGE_DIAGRAMS) != null)
            generateFullUsageDiagrams = settings.getBoolean(GENERATE_FULL_USAGE_DIAGRAMS);
        if (settings.get(GENERATE_FULL_INHERITANCE_DIAGRAMS) != null)
            generateFullInheritanceDiagrams = settings.getBoolean(GENERATE_FULL_INHERITANCE_DIAGRAMS);
        if (settings.get(GENERATE_NED_SOURCE_LISTINGS) != null)
            nedSourceListings = settings.getBoolean(GENERATE_NED_SOURCE_LISTINGS);
        if (settings.get(GENERATE_EXPLICIT_LINKS_ONLY) != null)
            generateExplicitLinksOnly = settings.getBoolean(GENERATE_EXPLICIT_LINKS_ONLY);

        if (settings.get(GENERATE_DOXY) != null)
            generateDoxy = settings.getBoolean(GENERATE_DOXY);
        if (settings.get(DOXY_SOURCE_BROWSER) != null)
            cppSourceListings = settings.getBoolean(DOXY_SOURCE_BROWSER);

        excludedDirs = settings.get(GENERATE_EXCLUDED_DIRS);
        if (StringUtils.isEmpty(excludedDirs))
            excludedDirs = null;

        outputDirectoryPath = settings.get(OUTPUT_DIRECTORY_PATH);
        if (StringUtils.isEmpty(outputDirectoryPath))
            outputDirectoryPath = null;
    }
}
