package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICFolderDescription;
import org.eclipse.cdt.core.settings.model.ICLanguageSetting;
import org.eclipse.cdt.core.settings.model.ICLanguageSettingEntry;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * A bridge between the IDE and opp_makemake. What it does:
 * - translates meta-options in MakemakeOptions into opp_makemake options
 * - adds opp_makemake -X options from CDT's "exclude from build" settings
 * - adds opp_makemake -I and -L options from CDT's include paths and library paths settings
 *
 * @author Andras
 */
public class MetaMakemake {
    /**
     * Returns true if Makefile was overwritten, and false if it was already up to date.
     */
    public static boolean generateMakefile(IContainer folder, MakemakeOptions options, 
            Map<IContainer,Set<IContainer>> folderDeps, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) throws IOException, CoreException {
        MakemakeOptions translatedOptions = translateOptions(folder, options, folderDeps);
        return new Makemake().generateMakefile(folder, translatedOptions, perFileDeps);
    }

    /** 
     * Translates makemake options
     */
    public static MakemakeOptions translateOptions(IContainer folder, MakemakeOptions options, Map<IContainer, Set<IContainer>> folderDeps) {
        MakemakeOptions translatedOptions = options.clone();

        // add -f, and potentially --nmake 
        translatedOptions.force = true;
        translatedOptions.isNMake = CDTUtils.isMsvcToolchainActive(folder.getProject());

        // add -X option for each excluded folder in CDT
        translatedOptions.exceptSubdirs.addAll(getExcludedPathsWithinFolder(folder));

        // add -I, -L and -D options configured in CDT
        translatedOptions.includeDirs.addAll(getIncludePathsFor(folder));
        translatedOptions.libDirs.addAll(getLibraryPathsFor(folder));
        translatedOptions.defines.addAll(getMacrosFor(folder));

        //TODO interpret meta-options (currently none)

        // add dependent folders
        //XXX should this be a meta-option?
        if (folderDeps.containsKey(folder))
            for (IContainer dep : folderDeps.get(folder))
                translatedOptions.includeDirs.add(dep.getLocation().toString());

        System.out.println("Translated makemake options for " + folder + ": " + translatedOptions.toString());
        return translatedOptions;
    }

    /**
     * Returns the (folder-relative) paths of directories and files excluded from build 
     * in the active CDT configuration under the given folder . 
     */
    protected static List<String> getExcludedPathsWithinFolder(IContainer folder) {
        List<String> result = new ArrayList<String>();
        IPath folderPath = folder.getFullPath();
        IProject project = folder.getProject();
        ICSourceEntry[] sourceEntries = ManagedBuildManager.getBuildInfo(project).getDefaultConfiguration().getSourceEntries();//XXX can something be null here?
        for (ICSourceEntry sourceEntry : sourceEntries) {
            IPath sourceFolderPath = project.getFullPath().append(sourceEntry.getFullPath());
            for (IPath exclusionPattern : sourceEntry.getExclusionPatterns()) {
                IPath exclusionPath = sourceFolderPath.append(exclusionPattern);
                if (folderPath.isPrefixOf(exclusionPath)) {
                    IPath subpath = exclusionPath.removeFirstSegments(folderPath.segmentCount());
                    result.add(subpath.segmentCount()==0 ? "." : subpath.toString());
                }
            }
        }
        return result;
    }
    
    /**
     * Returns the include paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getIncludePathsFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder);
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.INCLUDE_PATH));
    }

    /**
     * Returns the library paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getLibraryPathsFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder);
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.LIBRARY_PATH));
    }

    /**
     * Returns the defined macros (in NAME or NAME=VALUE form) for the given folder,
     * in the active configuration and for the C++ language. Built-in ones are skipped.
     */
    protected static List<String> getMacrosFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder);
        ICLanguageSettingEntry[] settingEntries = languageSetting.getSettingEntries(ICSettingEntry.MACRO);
        List<String> result = new ArrayList<String>();
        for (ICLanguageSettingEntry pathEntry : settingEntries) {
            if (!pathEntry.isBuiltIn()) {
                if (StringUtils.isEmpty(pathEntry.getValue()))
                    result.add(pathEntry.getName());
                else
                    result.add(pathEntry.getName() + "=" + pathEntry.getValue());
            }
        }
        return result;
    }

    /**
     * Converts the given path entries to filesystem paths, skips built-in ones,
     * and returns the result.
     */
    protected static List<String> getPaths(ICLanguageSettingEntry[] pathEntries) {
        List<String> result = new ArrayList<String>();
        for (ICLanguageSettingEntry pathEntry : pathEntries) {
            if (!pathEntry.isBuiltIn()) {
                String value = pathEntry.getValue();
                boolean isWorkspacePath = (pathEntry.getFlags() & ICSettingEntry.VALUE_WORKSPACE_PATH) != 0;
                if (isWorkspacePath) {
                    // convert to filesystem path
                    IFolder pathFolder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(value));
                    IPath location = pathFolder.getLocation(); // null if project doesn't exist etc
                    value = location == null ? null : location.toString();
                }
                if (value != null) {
                    result.add(value);
                }
            }
        }
        return result;
    }

    /**
     * Returns the C++ language settings for the given folder in the active configuration, or null if not found. 
     */
    protected static ICLanguageSetting findCCLanguageSettingFor(IContainer folder) {
        // find folder description
        IProject project = folder.getProject();
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
        ICConfigurationDescription activeConfiguration = projectDescription.getActiveConfiguration();
        ICFolderDescription folderDescription = (ICFolderDescription)activeConfiguration.getResourceDescription(folder.getProjectRelativePath(), false);

        // find C++ language settings for this folder
        ICLanguageSetting[] languageSettings = folderDescription.getLanguageSettings();
        ICLanguageSetting languageSetting = null;
        for (ICLanguageSetting l : languageSettings)
            if (l.getName().contains("C++"))  //XXX a bit dodgy, but languageId is usually "org.eclipse.cdt.core.g++" which is not good for MSVC 
                languageSetting = l;
        return languageSetting;
    }

}
