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
    private static final String GENERATE_SOURCE_LISTINGS = "GenerateSourceListings";
    private static final String GENERATE_FILE_LISTINGS = "GenerateFileListings";
    private static final String GENERATE_MSG_DEFINITIONS = "GenerateMsgDefinitions";
    private static final String AUTOMATIC_HYPERLINKING = "AutomaticHyperlinking";
    private static final String GENERATE_EXCLUDED_DIRS = "GenerateExcludedPackages";
    private static final String GENERATE_DOXY = "GenerateDoxy";
    private static final String DOXY_SOURCE_BROWSER = "DoxySourceBrowser";
    private static final String OUTPUT_DIRECTORY_PATH = "OutputDirectoryPath";
    private static final String EXTENSION_FILE_PATH = "ExtensionFilePath";

    public IProject[] projects;

    public boolean generateNedTypeFigures = true;
    public boolean generatePerTypeUsageDiagrams = true;
    public boolean generateFullUsageDiagrams = false;
    public boolean generatePerTypeInheritanceDiagrams = true;
    public boolean generateFullInheritanceDiagrams = false;
    public boolean generateMsgDefinitions = true;
    public boolean generateSourceListings = true;
    public boolean generateFileListings = true;
    public boolean automaticHyperlinking = true;
    public String excludedDirs;

    public boolean generateDoxy = true;
    public boolean cppSourceListings = false;

    public String outputDirectoryPath;
    public String extensionFilePath;

    public void store(IDialogSettings settings) {
        settings.put(GENERATE_NED_TYPE_FIGURES, generateNedTypeFigures);
        settings.put(GENERATE_PER_TYPE_USAGE_DIAGRAMS, generatePerTypeUsageDiagrams);
        settings.put(GENERATE_FULL_USAGE_DIAGRAMS, generateFullUsageDiagrams);
        settings.put(GENERATE_PER_TYPE_INHERITANCE_DIAGRAMS, generatePerTypeInheritanceDiagrams);
        settings.put(GENERATE_FULL_INHERITANCE_DIAGRAMS, generateFullInheritanceDiagrams);
        settings.put(GENERATE_SOURCE_LISTINGS, generateSourceListings);
        settings.put(GENERATE_FILE_LISTINGS, generateFileListings);
        settings.put(GENERATE_MSG_DEFINITIONS, generateMsgDefinitions);
        settings.put(AUTOMATIC_HYPERLINKING, automaticHyperlinking);
        settings.put(GENERATE_EXCLUDED_DIRS, excludedDirs);
        settings.put(EXTENSION_FILE_PATH, extensionFilePath);

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
        if (settings.get(GENERATE_SOURCE_LISTINGS) != null)
            generateSourceListings = settings.getBoolean(GENERATE_SOURCE_LISTINGS);
        if (settings.get(GENERATE_FILE_LISTINGS) != null)
            generateFileListings = settings.getBoolean(GENERATE_FILE_LISTINGS);
        if (settings.get(GENERATE_MSG_DEFINITIONS) != null)
            generateMsgDefinitions = settings.getBoolean(GENERATE_MSG_DEFINITIONS);
        if (settings.get(AUTOMATIC_HYPERLINKING) != null)
            automaticHyperlinking = settings.getBoolean(AUTOMATIC_HYPERLINKING);

        if (settings.get(GENERATE_DOXY) != null)
            generateDoxy = settings.getBoolean(GENERATE_DOXY);
        if (settings.get(DOXY_SOURCE_BROWSER) != null)
            cppSourceListings = settings.getBoolean(DOXY_SOURCE_BROWSER);

        excludedDirs = settings.get(GENERATE_EXCLUDED_DIRS);
        if (StringUtils.isEmpty(excludedDirs))
            excludedDirs = null;

        extensionFilePath = settings.get(EXTENSION_FILE_PATH);
        if (StringUtils.isEmpty(extensionFilePath))
            extensionFilePath = null; 

        outputDirectoryPath = settings.get(OUTPUT_DIRECTORY_PATH);
        if (StringUtils.isEmpty(outputDirectoryPath))
            outputDirectoryPath = null;
    }
}
