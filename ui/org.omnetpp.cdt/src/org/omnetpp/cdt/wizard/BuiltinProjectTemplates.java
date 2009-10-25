/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.CoreException;

/**
 * Provides built-in project templates
 * @author Andras
 */
public class BuiltinProjectTemplates {
   
    /**
     * Creates and returns built-in C++ project templates.
     */
    public static List<IProjectTemplate> getCppTemplates() {
        List<IProjectTemplate> result = new ArrayList<IProjectTemplate>();

        final String DEFAULT_SRCFOLDER_OPTIONS = "--deep --meta:recurse --meta:auto-include-path --meta:export-library --meta:use-exported-libs";
        final String DEFAULT_ROOTFOLDER_OPTIONS = "--nolink --meta:recurse";
        
        final String SINGLE_DIR = "Single-directory project, for small simulations";
        
        result.add(new BuiltinProjectTemplate("Empty project", SINGLE_DIR, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("rootpackage", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS});                
                createFileFromPluginResource("package.ned", "templates/package.ned");
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SINGLE_DIR, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("rootpackage", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS});                
                createFileFromPluginResource("package.ned", "templates/package.ned");
                createFileFromPluginResource("Txc.ned", "templates/Txc.ned");
                createFileFromPluginResource("Txc.h", "templates/Txc.h");
                createFileFromPluginResource("Txc.cc", "templates/Txc.cc");
                createFileFromPluginResource("Tictoc.ned", "templates/Tictoc.ned");
                createFileFromPluginResource("omnetpp.ini", "templates/Tictoc.ini");
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example", SINGLE_DIR, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("rootpackage", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS});                
                createFileFromPluginResource("package.ned", "templates/package.ned");
                createFileFromPluginResource("Source.ned", "templates/Source.ned");
                createFileFromPluginResource("Source.cc", "templates/Source.cc");
                createFileFromPluginResource("Source.h", "templates/Source.h");
                createFileFromPluginResource("Sink.ned", "templates/Sink.ned");
                createFileFromPluginResource("Sink.cc", "templates/Sink.cc");
                createFileFromPluginResource("Sink.h", "templates/Sink.h");
                createFileFromPluginResource("Network.ned", "templates/SourceSink.ned");
                createFileFromPluginResource("omnetpp.ini", "templates/SourceSink.ini");
            }
        }) ;

        final String SRC_AND_SIMULATIONS = "Project with \"src\" and \"simulations\" folders";
        
        result.add(new BuiltinProjectTemplate("Empty project", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("rootpackage", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createAndSetSourceFolders(new String[]{"src"});
                createAndSetNedSourceFolders(new String[] {"src", "simulations"});
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS});                
                createFileFromPluginResource("src/package.ned", "templates/package.ned");
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned");
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("rootpackage", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS});                
                createAndSetSourceFolders(new String[]{"src"});
                createAndSetNedSourceFolders(new String[] {"src", "simulations"});
                createFileFromPluginResource("src/package.ned", "templates/package.ned");
                createFileFromPluginResource("src/Txc.ned", "templates/Txc.ned");
                createFileFromPluginResource("src/Txc.h", "templates/Txc.h");
                createFileFromPluginResource("src/Txc.cc", "templates/Txc.cc");
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned");
                createFileFromPluginResource("simulations/Tictoc.ned", "templates/Tictoc.ned");
                createFileFromPluginResource("simulations/omnetpp.ini", "templates/Tictoc.ini");
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example (with NED packages and C++ namespace)", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "${projectname}");
                setVariable("rootpackage", "org.example.${projectname}");
                setVariable("simulationspackage", "${rootpackage}.simulations");
                substituteNestedVariables();
                createAndSetSourceFolders(new String[]{"src"});
                createAndSetNedSourceFolders(new String[] {"src", "simulations"});
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS});
                createFileFromPluginResource("src/package.ned", "templates/package.ned");
                createFileFromPluginResource("src/Source.ned", "templates/Source.ned");
                createFileFromPluginResource("src/Source.cc", "templates/Source.cc");
                createFileFromPluginResource("src/Source.h", "templates/Source.h");
                createFileFromPluginResource("src/Sink.ned", "templates/Sink.ned");
                createFileFromPluginResource("src/Sink.cc", "templates/Sink.cc");
                createFileFromPluginResource("src/Sink.h", "templates/Sink.h");
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned");
                createFileFromPluginResource("simulations/Network.ned", "templates/SourceSink.ned");
                createFileFromPluginResource("simulations/omnetpp.ini", "templates/SourceSink.ini");
            }
        });

        //final String SRC_AND_EXAMPLES = "Project with \"src\" and \"examples\" folders";
        //TODO like the above!
        
        return result;
    }

    /**
     * Creates and returns built-in non-C++ project templates.
     */
    public static List<IProjectTemplate> getNoncppTemplates() {
    	List<IProjectTemplate> result = new ArrayList<IProjectTemplate>();

    	final String NONCPP_SINGLEDIR = "Single-directory project";
        result.add(new BuiltinProjectTemplate("Empty project", NONCPP_SINGLEDIR, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createFileFromPluginResource("package.ned", "templates/simulationsPackage.ned");
            }
        });

        final String NONCPP_SIMULATIONS = "Project with a \"simulations\" folder";
        result.add(new BuiltinProjectTemplate("Empty project", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("simulationspackage", "");
                substituteNestedVariables();
                createAndSetNedSourceFolders(new String[] {"simulations"});
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned");
            }
        });

        result.add(new BuiltinProjectTemplate("Empty project with packages", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure() throws CoreException {
                setVariable("namespace", "");
                setVariable("simulationspackage", "org.example.${projectname}.simulations");
                substituteNestedVariables();
                createAndSetNedSourceFolders(new String[] {"simulations"});
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned");
            }
        });
        return result;
    }
    
}
