package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.macros.BuildMacroException;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacro;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacroProvider;
import org.eclipse.cdt.managedbuilder.macros.IProjectBuildMacroSupplier;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;

import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.common.util.StringUtils;

public class OmnetProjectMacroSupplier implements IProjectBuildMacroSupplier {
    private static String INCLUDE_DIRS_MACRO = "IncludeDirs";
    private static String SOURCE_DIRS_MACRO = "SourceDirs";

    private static final String fProjectMacros[] = new String[]{
        INCLUDE_DIRS_MACRO, //$NON-NLS-1$
        SOURCE_DIRS_MACRO, //$NON-NLS-1$
    };
    
    class SourcePathMacro implements IBuildMacro {
        private IManagedProject project;
        private String optionPrefix;
        private String name;

        public SourcePathMacro(String name, IManagedProject project, String optionPrefix) {
            this.name = name;
            this.project = project;
            this.optionPrefix = optionPrefix;
        }

        public int getMacroValueType() {
            return getValueType();
        }

        public String[] getStringListValue() throws BuildMacroException {
            return null;
        }

        public String getStringValue() throws BuildMacroException {
            List<String> dirs = new ArrayList<String>();
            collectIncludeDirs((IProject)project.getOwner(), dirs);
            return StringUtils.join(dirs, " ");
        }

        public String getName() {
            return name;
        }

        public int getValueType() {
            return VALUE_TEXT;
        }
        
        void collectIncludeDirs(IProject proj, final List<String> result) {
            // FIXME should handle dependent projects and leave out excluded folder
            try {
                proj.accept(new IResourceVisitor() {
                    public boolean visit(IResource resource) throws CoreException {
                        if (MakefileTools.isGoodFolder(resource)) {
                            result.add(optionPrefix+resource.getLocation().toString());
                        }
                        return true;
                    }
                });
            }
            catch (CoreException e) {
                // FIXME
            }            
        }
    }
    
    public IBuildMacro getMacro(String macroName, IManagedProject project, IBuildMacroProvider provider) {
        IBuildMacro macro = null;
        if(INCLUDE_DIRS_MACRO.equals(macroName))
            macro = new SourcePathMacro(macroName, project, "-I");
        if(SOURCE_DIRS_MACRO.equals(macroName))
            macro = new SourcePathMacro(macroName, project, "");
        return macro;
    }

    public IBuildMacro[] getMacros(IManagedProject project, IBuildMacroProvider provider) {
        List<IBuildMacro> result = new ArrayList<IBuildMacro>();
        for(String name : fProjectMacros) {
            IBuildMacro macro = getMacro(name, project, provider);
            Assert.isNotNull(macro, "Wrong macro name: "+name);
            result.add(macro);
        }
        return result.toArray(new IBuildMacro[] {});
    }

}
