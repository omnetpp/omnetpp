package org.omnetpp.neddoc;

import org.eclipse.core.resources.IProject;
import org.eclipse.jface.dialogs.IDialogSettings;

public class GeneratorConfiguration {
    private static final String GENERATE_NED_TYPE_FIGURES = "GenerateNedTypeFigures";

    private static final String GENERATE_USAGE_DIAGRAMS = "GenerateUsageDiagrams";

    private static final String GENERATE_INHERITANCE_DIAGRAMS = "GenerateInheritanceDiagrams";

    private static final String GENERATE_SOURCE_CONTENT = "GenerateSourceContent";

    private static final String GENERATE_DOXY = "GenerateDoxy";

    private static final String DOXY_SOURCE_BROWSER = "DoxySourceBrowser";

    private static final String OUTPUT_DIRECTORY_PATH = "OutputDirectoryPath";

    public IProject[] projects;

    public boolean generateNedTypeFigures = true;
    public boolean generateUsageDiagrams = true;
    public boolean generateInheritanceDiagrams = true;
    public boolean generateSourceContent = true;

    public boolean generateDoxy = true;
    public boolean doxySourceBrowser = false;

    public String outputDirectoryPath;
    
    public void store(IDialogSettings settings) {
        settings.put(GENERATE_NED_TYPE_FIGURES, generateNedTypeFigures);
        settings.put(GENERATE_USAGE_DIAGRAMS, generateUsageDiagrams);
        settings.put(GENERATE_INHERITANCE_DIAGRAMS, generateInheritanceDiagrams);
        settings.put(GENERATE_SOURCE_CONTENT, generateSourceContent);

        settings.put(GENERATE_DOXY, generateDoxy);
        settings.put(DOXY_SOURCE_BROWSER, doxySourceBrowser);
        
        settings.put(OUTPUT_DIRECTORY_PATH, outputDirectoryPath);
    }

    public void restore(IDialogSettings settings) {
        if (settings.get(GENERATE_NED_TYPE_FIGURES) != null)
            generateNedTypeFigures = settings.getBoolean(GENERATE_NED_TYPE_FIGURES);
        if (settings.get(GENERATE_USAGE_DIAGRAMS) != null)
            generateUsageDiagrams = settings.getBoolean(GENERATE_USAGE_DIAGRAMS);
        if (settings.get(GENERATE_INHERITANCE_DIAGRAMS) != null)
            generateInheritanceDiagrams = settings.getBoolean(GENERATE_INHERITANCE_DIAGRAMS);
        if (settings.get(GENERATE_SOURCE_CONTENT) != null)
            generateSourceContent = settings.getBoolean(GENERATE_SOURCE_CONTENT);

        if (settings.get(GENERATE_DOXY) != null)
            generateDoxy = settings.getBoolean(GENERATE_DOXY);
        if (settings.get(DOXY_SOURCE_BROWSER) != null)
            doxySourceBrowser = settings.getBoolean(DOXY_SOURCE_BROWSER);

        outputDirectoryPath = settings.get(OUTPUT_DIRECTORY_PATH);
    }
}
