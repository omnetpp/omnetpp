/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.wizard.CreationContext;

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
                context.getVariables().put("rootPackage", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "package.ned.ftl", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SINGLE_DIR, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootPackage", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "package.ned.ftl", context);
                createFileFromPluginResource("Txc.ned", "Txc.ned.ftl", context);
                createFileFromPluginResource("Txc.h", "Txc.h.ftl", context);
                createFileFromPluginResource("Txc.cc", "Txc.cc.ftl", context);
                createFileFromPluginResource("Tictoc.ned", "Tictoc.ned.ftl", context);
                createFileFromPluginResource("omnetpp.ini", "Tictoc.ini.ftl", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example", SINGLE_DIR, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootPackage", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("package.ned", "package.ned.ftl", context);
                createFileFromPluginResource("Source.ned", "Source.ned.ftl", context);
                createFileFromPluginResource("Source.cc", "Source.cc.ftl", context);
                createFileFromPluginResource("Source.h", "Source.h.ftl", context);
                createFileFromPluginResource("Sink.ned", "Sink.ned.ftl", context);
                createFileFromPluginResource("Sink.cc", "Sink.cc.ftl", context);
                createFileFromPluginResource("Sink.h", "Sink.h.ftl", context);
                createFileFromPluginResource("Network.ned", "SourceSink.ned.ftl", context);
                createFileFromPluginResource("omnetpp.ini", "SourceSink.ini.ftl", context);
            }
        }) ;

        final String SRC_AND_SIMULATIONS = "Project with \"src\" and \"simulations\" folders";
        
        result.add(new BuiltinProjectTemplate("Empty project", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootPackage", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createFileFromPluginResource("src/package.ned", "package.ned.ftl", context);
                createFileFromPluginResource("simulations/package.ned", "simulationsPackage.ned.ftl", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Tictoc example", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("rootPackage", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);                
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createFileFromPluginResource("src/package.ned", "package.ned.ftl", context);
                createFileFromPluginResource("src/Txc.ned", "Txc.ned.ftl", context);
                createFileFromPluginResource("src/Txc.h", "Txc.h.ftl", context);
                createFileFromPluginResource("src/Txc.cc", "Txc.cc.ftl", context);
                createFileFromPluginResource("simulations/package.ned", "simulationsPackage.ned.ftl", context);
                createFileFromPluginResource("simulations/Tictoc.ned", "Tictoc.ned.ftl", context);
                createFileFromPluginResource("simulations/omnetpp.ini", "Tictoc.ini.ftl", context);
            }
        });
        
        result.add(new BuiltinProjectTemplate("Source-sink example (with NED packages and C++ namespace)", SRC_AND_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "${projectname}");
                context.getVariables().put("rootPackage", "org.example.${projectname}");
                context.getVariables().put("simulationsPackage", "${rootPackage}.simulations");
                substituteNestedVariables(context);
                createAndSetSourceFolders(new String[]{"src"}, context);
                createAndSetNedSourceFolders(new String[] {"src", "simulations"}, context);
                createBuildSpec(new String[] {".", DEFAULT_ROOTFOLDER_OPTIONS, "src", DEFAULT_SRCFOLDER_OPTIONS}, context);
                createFileFromPluginResource("src/package.ned", "package.ned.ftl", context);
                createFileFromPluginResource("src/Source.ned", "Source.ned.ftl", context);
                createFileFromPluginResource("src/Source.cc", "Source.cc.ftl", context);
                createFileFromPluginResource("src/Source.h", "Source.h.ftl", context);
                createFileFromPluginResource("src/Sink.ned", "Sink.ned.ftl", context);
                createFileFromPluginResource("src/Sink.cc", "Sink.cc.ftl", context);
                createFileFromPluginResource("src/Sink.h", "Sink.h.ftl", context);
                createFileFromPluginResource("simulations/package.ned", "simulationsPackage.ned.ftl", context);
                createFileFromPluginResource("simulations/Network.ned", "SourceSink.ned.ftl", context);
                createFileFromPluginResource("simulations/omnetpp.ini", "SourceSink.ini.ftl", context);
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
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createFileFromPluginResource("package.ned", "simulationsPackage.ned.ftl", context);
            }
        });

        final String NONCPP_SIMULATIONS = "Project with a \"simulations\" folder";
        result.add(new BuiltinProjectTemplate("Empty project", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("simulationsPackage", "");
                substituteNestedVariables(context);
                createAndSetNedSourceFolders(new String[] {"simulations"}, context);
                createFileFromPluginResource("simulations/package.ned", "simulationsPackage.ned.ftl", context);
            }
        });

        result.add(new BuiltinProjectTemplate("Empty project with packages", NONCPP_SIMULATIONS, null) {
            @Override
            public void doConfigure(CreationContext context) throws CoreException {
                context.getVariables().put("namespace", "");
                context.getVariables().put("simulationsPackage", "org.example.${projectname}.simulations");
                substituteNestedVariables(context);
                createAndSetNedSourceFolders(new String[] {"simulations"}, context);
                createFileFromPluginResource("simulations/package.ned", "simulationsPackage.ned.ftl", context);
            }
        });
        return result;
    }
    
}
