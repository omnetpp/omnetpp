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
import org.eclipse.ui.internal.ReopenEditorMenu;

import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.common.util.StringUtils;

public class OmnetProjectMacroSupplier implements IProjectBuildMacroSupplier {
    private static String INCLUDE_DIRS_MACRO = "IncludeDirs";
    private static String MESSAGE_DIRS_MACRO = "MessageDirs";

    private static final String fProjectMacros[] = new String[]{
        INCLUDE_DIRS_MACRO, //$NON-NLS-1$
        MESSAGE_DIRS_MACRO, //$NON-NLS-1$
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

        public int getMacroValueType() {
            return getValueType();
        }

        public String[] getStringListValue() throws BuildMacroException {
            return null;
        }

        public String getStringValue() throws BuildMacroException {
            List<String> dirs = new ArrayList<String>();
            IProject project = (IProject)cdtProject.getOwner();

            collectDirs(project, dirs);
            return StringUtils.join(dirs, " ");
        }

        public String getName() {
            return name;
        }

        public int getValueType() {
            return VALUE_TEXT;
        }
        
        /**
         * true if the given container has a file which name matches the given pattern
         */
        private boolean containsFileMatchingPattern(IContainer container) {
            try {
                for (IResource member : container.members())
                    if (member.getFullPath().toPortableString().matches(pattern))
                        return true;
            }
            catch (CoreException e) {
                return true;
            }
            return false;
        }
        
        private void collectDirs(IProject proj, final List<String> result) throws BuildMacroException {
            IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(proj);
            final ICSourceEntry[] srcEntries = buildInfo.getDefaultConfiguration().getSourceEntries();
            
            try {
                proj.accept(new IResourceVisitor() {
                    public boolean visit(IResource resource) throws CoreException {
                        if (resource instanceof IContainer && MakefileTools.isGoodFolder(resource)) {
                            if (!CDataUtil.isExcluded(resource.getProjectRelativePath(), srcEntries)
                                    && containsFileMatchingPattern((IContainer)resource))
                                result.add(optionPrefix+resource.getLocation().toString());
                            
                            return true;
                        }
                        return false;
                    }
                });
                
                // collect directories from referenced projects too (recursively)
                for(IProject refProj : proj.getReferencedProjects())
                    collectDirs(refProj, result);
            }
            catch (CoreException e) {
                throw new BuildMacroException(Status.CANCEL_STATUS);
            }            
        }
    }
    
    public IBuildMacro getMacro(String macroName, IManagedProject project, IBuildMacroProvider provider) {
        IBuildMacro macro = null;
        if(INCLUDE_DIRS_MACRO.equals(macroName))
            macro = new SourcePathMacro(macroName, project, "-I", ".*\\.h");
        if(MESSAGE_DIRS_MACRO.equals(macroName)) {
            macro = new SourcePathMacro(macroName, project, "", ".*\\.msg");
        }
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
