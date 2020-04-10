package org.omnetpp.neddoc;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.preferences.InstanceScope;
import org.eclipse.equinox.app.IApplication;
import org.eclipse.equinox.app.IApplicationContext;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.neddoc.properties.DocumentationGeneratorPropertyPage;
import org.osgi.service.prefs.BackingStoreException;
import org.osgi.service.prefs.Preferences;

public class NeddocApplication implements IApplication {

    @Override
    public Object start(IApplicationContext context) throws Exception {
        String args[] = (String[])context.getArguments().get(IApplicationContext.APPLICATION_ARGS);
        String filenameArgs[] = getFilenameArgs(args);
        Set<String> switchArgs = getSwitchOptionArgs(args);
        Map<String,String> valueArgs = getValueOptionArgs(args);
        if (filenameArgs.length == 0) { // if no project given print usage
            System.err.println("Error: No project location argument specified\n");
            printUsage();
            return EXIT_OK;
        }
        
        // Disable the CDT indexer so it will not consume CPU in the background
        Preferences preferences = InstanceScope.INSTANCE.getNode("org.eclipse.cdt.core");
        Preferences prefNode = preferences.node("indexer");
        prefNode.put("indexerId", "org.eclipse.cdt.core.nullindexer");
        try {
            preferences.flush();
        } catch (BackingStoreException e) { 
            // we don't care if there was an error during write as this setting is not mission critical
        }
        // disable autobuild so other builder (like MakefileBuilder) will not start unnecessary       
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        IWorkspaceDescription wd = workspace.getDescription();
        wd.setAutoBuilding(false);  // turn off auto build otherwise the CDT builder would unnecessary kick in
        workspace.setDescription(wd);

        try {
            // import / open all specified projects and create a doc generator for each of them
            List<DocumentationGenerator> generators = new ArrayList<DocumentationGenerator>();
            for (String projectLocation: filenameArgs) {
                IProject project = null;
                IProjectDescription projectDescription = null;

                IPath projectFileLocation = new Path(projectLocation).append(".project");
                if (!projectFileLocation.isAbsolute())
                    projectFileLocation = new Path(System.getProperty("user.dir")).append(projectFileLocation);

                try {
                    projectDescription = workspace.loadProjectDescription(projectFileLocation);
                } catch (Exception e) {
                    System.err.println("Error: folder does not exist or does not contain a project description (.project) file: " + projectLocation);
                    return EXIT_OK;
                }
                project = workspace.getRoot().getProject(projectDescription.getName());
                if (project.exists())
                    project.delete(false, true, null);
                project.create(projectDescription, null);
                project.open(null);

                // generate documentation for the project
                DocumentationGenerator generator = new DocumentationGenerator(project);
                generator.setHeadless(true);
                configureGenerator(project, generator, switchArgs, valueArgs);
                generators.add(generator);                
            }

            // project are now open so we can generate doc for all of them
            for (DocumentationGenerator generator : generators)
                generator.generate(new NullProgressMonitor());

        } catch (Exception e) {
            System.err.println("Error: " + e.getMessage());
            return EXIT_OK;
        }

        return EXIT_OK;
    }

    @Override
    public void stop() {
    }
    
    private static void printOption(String option, String desc) {
        System.out.println("  " + option + ":");
        System.out.println(StringUtils.indentLines(WordUtils.wrap(desc, 80), "      "));
    }

    private static void printUsage() {
        System.out.println("opp_neddoc -- part of " + IConstants.PRODUCT_NAME + ", (C) 2019 OpenSim Ltd.\n");
        System.out.println("Generate NED documentation for a project. This command provides functionality");
        System.out.println("similar to the 'Generate NED Documentation' dialog in the IDE.\n");
        System.out.println("Usage:");
        System.out.println("   opp_neddoc [options] <project_directory> [project_directory2]...\n");
        System.out.println("Options:");
        
        printOption("--network-diagrams",
                "Include icons and compound module network diagrams in type pages (default)");
        printOption("--no-network-diagrams",
                "Do not include icons and compound module network diagrams in type pages");
        printOption("--inheritance-diagrams", 
                "Include NED and MSG inheritance diagrams in type pages (default)");
        printOption("--no-inheritance-diagrams",
                "Do not include NED and MSG inheritance diagrams in type pages");
        printOption("--usage-diagrams", 
                "Include NED and MSG usage diagrams in type pages (default)");
        printOption("--no-usage-diagrams",
                "Do not include NED and MSG usage diagrams in type pages");
        printOption("--full-inheritance-diagrams",
                "Generate full NED and MSG inheritance diagram pages");
        printOption("--no-full-inheritance-diagrams",
                "Do not generate full NED and MSG inheritance diagram pages (default)");
        printOption("--full-usage-diagrams",
                "Generate full NED and MSG usage diagram pages");
        printOption("--no-full-usage-diagrams",
                "Do not generate full NED and MSG usage diagram pages (default)");
        printOption("--message-definitions", 
                "Include MSG types in the documentation (default)");
        printOption("--no-message-definitions",
                "Do not include MSG types in the documentation");
        printOption("--source-snippets", 
                "Include NED and MSG source in type pages (default)");
        printOption("--no-source-snippets",
                "Do not include NED and MSG source in type pages");
        printOption("--source-files",
                "Include NED and MSG file listings (default)");
        printOption("--no-source-files",
                "Do not include NED and MSG file listings");
        printOption("--automatic-hyperlinks",
                "Automatically add hyperlinks to NED and MSG type names in descriptions (default)");
        printOption("--no-automatic-hyperlinks", 
                "Do not automatically add hyperlinks to NED and MSG type names in descriptions. "
                + "Tilde notation needs to be used for names to be hyperlinked: ~Sink, ~TCP.");
        printOption("--doxygen", 
                "Generate documentation from C++ code using Doxygen");
        printOption("--no-doxygen", 
                "Do not generate documentation from C++ code (default)");
        printOption("--cpp-source-files",
                "Include C++ file listings in the Doxygen documentation");
        printOption("--no-cpp-source-files",
                "Do not include C++ file listings in the Doxygen documentation (default)");
        printOption("--verbose",
                "Print out progress report");
        printOption("-x <excluded_directories>",
                "Exclude one or more workspace relative directories from NED path. Use comma separated glob patterns: '/project/samples, /*/examples, **/test'");
        printOption("-o <output_directory>",
                "The output directory for the generated documentation. "
                + "The documentation is created under the 'doc' folder in each project by default.");
        printOption("-f <fragment_file>",
                "A file (with project relative path) containing document fragments to be injected into the generated documentation.");
    }

    private static void configureGenerator(IProject project, DocumentationGenerator generator, Set<String> switchOptions, Map<String,String> valueOptions) {
        String outputDir = valueOptions.get("-o");
        IPath destinationDirPath = outputDir == null ? project.getLocation() : new Path(outputDir).append(project.getName());
        generator.setDocumentationRootPath(destinationDirPath);
        
        String extensionFilePath = valueOptions.get("-f");
        if (StringUtils.isNotBlank(extensionFilePath))
            generator.setExtensionFilePath(new Path(extensionFilePath));      
                
        generator.setExcludedDirs(valueOptions.getOrDefault("-x", ""));
        generator.setRootRelativeDoxyPath(new Path(DocumentationGeneratorPropertyPage.DEFAULT_DOXY_PATH));
        generator.setRootRelativeNeddocPath(new Path(DocumentationGeneratorPropertyPage.DEFAULT_NEDDOC_PATH));
        generator.setAbsoluteDoxyConfigFilePath(project.getFile(DocumentationGeneratorPropertyPage.DEFAULT_DOXY_CONFIG_FILE_PATH).getLocation());

        generator.setGenerateNedTypeFigures(getBoolOption(switchOptions, "network-diagrams", true));
        generator.setGeneratePerTypeInheritanceDiagrams(getBoolOption(switchOptions, "inheritance-diagrams", true));
        generator.setGeneratePerTypeUsageDiagrams(getBoolOption(switchOptions, "usage-diagrams", true));
        generator.setGenerateFullInheritanceDiagrams(getBoolOption(switchOptions, "full-inheritance-diagrams", false));
        generator.setGenerateFullUsageDiagrams(getBoolOption(switchOptions, "full-usage-diagrams", false));
        generator.setGenerateMsgDefinitions(getBoolOption(switchOptions, "message-definitions", true));
        generator.setGenerateSourceListings(getBoolOption(switchOptions, "source-snippets", true));
        generator.setGenerateFileListings(getBoolOption(switchOptions, "source-files", true));
        generator.setAutomaticHyperlinking(getBoolOption(switchOptions, "automatic-hyperlinks", true));
        generator.setGenerateDoxy(getBoolOption(switchOptions, "doxygen", false));
        generator.setGenerateCppSourceListings(getBoolOption(switchOptions, "cpp-source-files", false));
        generator.setVerboseMode(getBoolOption(switchOptions, "verbose", false));
    }
    
    private static boolean getBoolOption(Set<String> switchOptions, String optionName, boolean defaultValue) {
        if (switchOptions.contains("--" + optionName))
            return true;
        if (switchOptions.contains("--no-" + optionName))
            return false;
        return defaultValue;
    }
    /**
     * Picks the filename (non-option) args from the command line, and returns them.
     */
    private static String[] getFilenameArgs(String[] args) {
        List<String> fileArgs = new ArrayList<String>();
        for (int i = 0; i < args.length; i++) {
            // NOTE this is not necessarily a good solution to filter out the file names
            // we assume that - is used for options with an additional argument while
            // -- is used for standalone options.
            if (args[i].startsWith("--"))
                ;  // skip
            else if (args[i].startsWith("-"))
                i++; // skip the next item too (single hyphen args have an additional value)
            else
                fileArgs.add(args[i]);
        }
        return fileArgs.toArray(new String[]{});
    }

    /**
     * Returns all switch type options, i.e. those that start with a double hyphen
     */
    private static Set<String> getSwitchOptionArgs(String[] args) {
        Set<String> fileArgs = new TreeSet<String>();
        for (int i = 0; i < args.length; i++) {
            // we assume that - is used for options with an additional argument while
            // -- is used for standalone options.
            if (args[i].startsWith("--"))
                fileArgs.add(args[i]);
        }
        return fileArgs;
    }

    /**
     * Returns all value type options, i.e. those that start with a single hyphen
     */
    private static Map<String, String> getValueOptionArgs(String[] args) {
        Map<String,String> valueArgs = new TreeMap<String,String>();
        for (int i = 0; i < args.length; i++) {
            // we assume that - is used for options with an additional argument while
            // -- is used for standalone options.
            if (!args[i].startsWith("--") && args[i].startsWith("-") && i+1 < args.length) {
                valueArgs.put(args[i], args[++i]);
            }
        }
        return valueArgs;
    }
}
