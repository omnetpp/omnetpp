package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.macros.BuildMacroException;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacro;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacroProvider;
import org.eclipse.cdt.managedbuilder.macros.IProjectBuildMacroSupplier;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Status;

import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.common.util.StringUtils;

public class OmnetppProjectMacroSupplier implements IProjectBuildMacroSupplier {
    private static String INCLUDE_DIRS_MACRO = "IncludeDirs"; // all non excluded dirs containing a *.h file in the project
    private static String MESSAGE_DIRS_MACRO = "MessageDirs";  // all non excluded dirs containing a *.msg file in the project
    private static String PRIMARY_SOURCE_DIR_MACRO = "PrimarySourceDir";  // the first source directory

    private static final String fProjectMacros[] = new String[]{
        INCLUDE_DIRS_MACRO,
        MESSAGE_DIRS_MACRO,
        PRIMARY_SOURCE_DIR_MACRO,
    };
    
    class SourcePathMacro implements IBuildMacro {
        private IManagedProject cdtProject;
        private String optionPrefix;
        private String name;
        private String pattern;

        public SourcePathMacro(String name, IManagedProject project, String optionPrefix, String pattern) {
            this.name = name;
            this.pattern = pattern;
            this.cdtProject = project;
            this.optionPrefix = optionPrefix;
        }

        public String[] getStringListValue() throws BuildMacroException {
            return null;
        }

        public String getStringValue() throws BuildMacroException {
            try {
            	IProject project = (IProject)cdtProject.getOwner();
            	String result ="";
				for(IContainer cont : MakefileTools.collectDirs(project, pattern))
					result += optionPrefix+cont.getLocation().toString()+" ";
				return result;
			} catch (CoreException e) {
				throw new BuildMacroException(e.getStatus());
			}
        }

        public String getName() {
            return name;
        }

        public int getMacroValueType() {
            return getValueType();
        }
        
        public int getValueType() {
            return VALUE_TEXT;
        }
    }
    
    /**
     * A macro that returns the first source directory (used to store the primary makefile)
     */
    class PrimarySourceDirMacro implements IBuildMacro {
        private IManagedProject project;

        public PrimarySourceDirMacro(IManagedProject project) {
            this.project = project;
        }

        public String[] getStringListValue() throws BuildMacroException {
            return null;
        }

        public String getStringValue() throws BuildMacroException {
            IProject prj = (IProject)project.getOwner();
            ICSourceEntry srcEntries[] = CDTUtils.getSourceEntries(prj);
            if (srcEntries.length > 0) {
                return prj.getFullPath().append(srcEntries[0]. getFullPath()).toString();
            }
            // if no source entry is specified we return the project root
            return prj.getFullPath().toString();
        }

        public String getName() {
            return PRIMARY_SOURCE_DIR_MACRO;
        }

        public int getMacroValueType() {
            return getValueType();
        }
        
        public int getValueType() {
            return VALUE_TEXT;
        }
    }
    
    public IBuildMacro getMacro(String macroName, IManagedProject project, IBuildMacroProvider provider) {
        IBuildMacro macro = null;
        if(INCLUDE_DIRS_MACRO.equals(macroName))
            macro = new SourcePathMacro(macroName, project, "-I", null);
        if(MESSAGE_DIRS_MACRO.equals(macroName)) 
            macro = new SourcePathMacro(macroName, project, "", ".*\\.msg");
        if(PRIMARY_SOURCE_DIR_MACRO.equals(macroName))
            macro = new PrimarySourceDirMacro(project);
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
