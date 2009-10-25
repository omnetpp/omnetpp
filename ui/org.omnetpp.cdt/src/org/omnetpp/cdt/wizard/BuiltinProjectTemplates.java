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
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootpackage", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "templates/package.ned", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SINGLE_DIR, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootpackage", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "templates/package.ned", context);
                createFileFromPluginResource("Txc.ned", "templates/Txc.ned", context);
                createFileFromPluginResource("Txc.h", "templates/Txc.h", context);
                createFileFromPluginResource("Txc.cc", "templates/Txc.cc", context);
                createFileFromPluginResource("Tictoc.ned", "templates/Tictoc.ned", context);
                createFileFromPluginResource("omnetpp.ini", "templates/Tictoc.ini", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example", SINGLE_DIR, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootpackage", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "templates/package.ned", context);
                createFileFromPluginResource("Source.ned", "templates/Source.ned", context);
                createFileFromPluginResource("Source.cc", "templates/Source.cc", context);
                createFileFromPluginResource("Source.h", "templates/Source.h", context);
                createFileFromPluginResource("Sink.ned", "templates/Sink.ned", context);
                createFileFromPluginResource("Sink.cc", "templates/Sink.cc", context);
                createFileFromPluginResource("Sink.h", "templates/Sink.h", context);
                createFileFromPluginResource("Network.ned", "templates/SourceSink.ned", context);
                createFileFromPluginResource("omnetpp.ini", "templates/SourceSink.ini", context);
            }
        }) ;

        final String SRC_AND_SIMULATIONS = "Project with \"src\" and \"simulations\" folders";
        
        result.add(new BuiltinProjectTemplate("Empty project", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootpackage", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("src/package.ned", "templates/package.ned", context);
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootpackage", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createFileFromPluginResource("src/package.ned", "templates/package.ned", context);
                createFileFromPluginResource("src/Txc.ned", "templates/Txc.ned", context);
                createFileFromPluginResource("src/Txc.h", "templates/Txc.h", context);
                createFileFromPluginResource("src/Txc.cc", "templates/Txc.cc", context);
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned", context);
                createFileFromPluginResource("simulations/Tictoc.ned", "templates/Tictoc.ned", context);
                createFileFromPluginResource("simulations/omnetpp.ini", "templates/Tictoc.ini", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example (with NED packages and C++ namespace)", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "${projectname}");
                context.getVariables().put("rootpackage", "org.example.${projectname}");
                context.getVariables().put("simulationspackage", "${rootpackage}.simulations");
                substituteNestedVariables(context);
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);
                createFileFromPluginResource("src/package.ned", "templates/package.ned", context);
                createFileFromPluginResource("src/Source.ned", "templates/Source.ned", context);
                createFileFromPluginResource("src/Source.cc", "templates/Source.cc", context);
                createFileFromPluginResource("src/Source.h", "templates/Source.h", context);
                createFileFromPluginResource("src/Sink.ned", "templates/Sink.ned", context);
                createFileFromPluginResource("src/Sink.cc", "templates/Sink.cc", context);
                createFileFromPluginResource("src/Sink.h", "templates/Sink.h", context);
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned", context);
                createFileFromPluginResource("simulations/Network.ned", "templates/SourceSink.ned", context);
                createFileFromPluginResource("simulations/omnetpp.ini", "templates/SourceSink.ini", context);
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
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createFileFromPluginResource("package.ned", "templates/simulationsPackage.ned", context);
            }
        });

        final String NONCPP_SIMULATIONS = "Project with a \"simulations\" folder";
        result.add(new BuiltinProjectTemplate("Empty project", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("simulationspackage", "");
                substituteNestedVariables(context);
                createAndSetNedSourceFolders(new String[] {"simulations"}, context);
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned", context);
            }
        });

        result.add(new BuiltinProjectTemplate("Empty project with packages", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("simulationspackage", "org.example.${projectname}.simulations");
                substituteNestedVariables(context);
                createAndSetNedSourceFolders(new String[] {"simulations"}, context);
                createFileFromPluginResource("simulations/package.ned", "templates/simulationsPackage.ned", context);
            }
        });
        return result;
    }
    
}
