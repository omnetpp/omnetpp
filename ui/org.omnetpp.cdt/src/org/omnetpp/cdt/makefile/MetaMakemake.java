package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
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
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.project.ProjectUtils;
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
    public static boolean generateMakefile(IContainer folder, MakemakeOptions options) throws IOException, CoreException {
        MakemakeOptions translatedOptions = translateOptions(folder, options);
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies = Activator.getDependencyCache().getPerFileDependencies();
        return new Makemake().generateMakefile(folder, translatedOptions, perFileDependencies);
    }

    /** 
     * Translates makemake options
     */
    public static MakemakeOptions translateOptions(IContainer folder, MakemakeOptions options) throws CoreException {
        MakemakeOptions translatedOptions = options.clone();

        IProject project = folder.getProject();
        Map<IContainer, Set<IContainer>> folderDeps = Activator.getDependencyCache().getFolderDependencies();

        // add -f, and potentially --nmake 
        translatedOptions.force = true;
        translatedOptions.isNMake = CDTUtils.isMsvcToolchainActive(project);

        // add -X option for each excluded folder in CDT
        translatedOptions.exceptSubdirs.addAll(getExcludedPathsWithinFolder(folder));

        // add -I, -L and -D options configured in CDT
        translatedOptions.includeDirs.addAll(getIncludePathsFor(folder));
        translatedOptions.libDirs.addAll(getLibraryPathsFor(folder));
        translatedOptions.defines.addAll(getMacrosFor(folder));

        // add symbols for locations of referenced projects (they will be used by Makemake.abs2rel())
        IProject[] referencedProjects = ProjectUtils.getAllReferencedProjects(project);
        for (IProject referencedProject : referencedProjects) {
            String name = Makemake.makeSymbolicProjectName(referencedProject);
            String path = makeRelativePath(referencedProject, project);
            translatedOptions.makefileDefines.add(name + "=" + path);
        }

        // add extra include folders
        if (options.metaAutoIncludePath && folderDeps != null) {
            // we're looking for include path of the source folders covered by this makefile
            List<IContainer> srcFolders;
            if (options.isDeep)
                srcFolders = collectSourceFolders(folder); // every folder under this makemake folder
            else
                srcFolders = Arrays.asList(new IContainer[]{ folder });

            // find the referenced folders for each srcFolder, and add to include path if it's outside this deep makefile
            for (IContainer srcFolder : srcFolders)
                if (folderDeps.containsKey(srcFolder))  
                    for (IContainer dep : folderDeps.get(srcFolder))
                        if (!srcFolder.getFullPath().isPrefixOf(dep.getFullPath()))  // only add if "dep" is outside "folder"!
                            if (!translatedOptions.includeDirs.contains(dep.getLocation().toString()))  // if not already in there
                                translatedOptions.includeDirs.add(dep.getLocation().toString());

            // clear processed setting
            translatedOptions.metaAutoIncludePath = false;
        }

        // add libraries from other projects, if requested
        if (options.metaUseExportedLibs) {
            for (IProject referencedProject : referencedProjects) {
                BuildSpecification buildSpec = null;
                try {
                    buildSpec = BuildSpecUtils.readBuildSpecFile(referencedProject);
                }
                catch (IOException e) { 
                    //XXX ?
                } 
                if (buildSpec != null) {
                    for (IContainer f : buildSpec.getMakemakeFolders()) {
                        MakemakeOptions opt = buildSpec.getMakemakeOptions(f);
                        if ((opt.type==Type.SHAREDLIB || opt.type==Type.STATICLIB) && opt.metaExportLibrary) {
                            String libname = StringUtils.isEmpty(opt.target) ? f.getName() : opt.target;
                            String outdir = StringUtils.isEmpty(opt.outRoot) ? "out" : opt.outRoot; //FIXME hardcoded default!!!
                            String libdir = makeRelativePath(f.getProject(), folder) + "/" + new Path(outdir).append("$(CONFIGNAME)").append(f.getProjectRelativePath()).toString();
                            translatedOptions.libs.add(libname);
                            translatedOptions.libDirs.add(libdir);
                        }
                    }
                }
            }
            translatedOptions.metaUseExportedLibs = false;
        }

        System.out.println("Translated makemake options for " + folder + ": " + translatedOptions.toString());
        return translatedOptions;
    }

    protected static List<IContainer> collectSourceFolders(IContainer folder) throws CoreException {
        List<IContainer> result = new ArrayList<IContainer>();
        ICSourceEntry[] sourceEntries = ManagedBuildManager.getBuildInfo(folder.getProject()).getDefaultConfiguration().getSourceEntries();//XXX can something be null here?
        collectSourceFolders(folder, result, sourceEntries);
        return result;
    }    

    protected static void collectSourceFolders(IContainer folder, List<IContainer> result, ICSourceEntry[] sourceEntries) throws CoreException {
        if (!CDataUtil.isExcluded(folder.getProjectRelativePath(), sourceEntries)) {
            result.add(folder);
            for (IResource member : folder.members())
                if (MakefileTools.isGoodFolder(member))
                    collectSourceFolders((IContainer)member, result, sourceEntries);
        }
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
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, false);
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.INCLUDE_PATH));
    }

    /**
     * Returns the library paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getLibraryPathsFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, true);
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.LIBRARY_PATH));
    }

    /**
     * Returns the defined macros (in NAME or NAME=VALUE form) for the given folder,
     * in the active configuration and for the C++ language. Built-in ones are skipped.
     */
    protected static List<String> getMacrosFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, false);
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
     * Returns the language settings for the given folder in the active configuration, or null if not found.
     */
    protected static ICLanguageSetting findCCLanguageSettingFor(IContainer folder, boolean forLinker) {
        // find folder description
        IProject project = folder.getProject();
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
        ICConfigurationDescription activeConfiguration = projectDescription.getActiveConfiguration();
        ICFolderDescription folderDescription = (ICFolderDescription)activeConfiguration.getResourceDescription(folder.getProjectRelativePath(), false);

        // find C++ language settings for this folder
        ICLanguageSetting[] languageSettings = folderDescription.getLanguageSettings();
        ICLanguageSetting languageSetting = null;
        for (ICLanguageSetting l : languageSettings) {
            System.out.println("*** language setting: "+l.getName()+" "+l.getId());
            if (l.getName().contains(forLinker ? "Object" : "C++"))  //XXX a bit dodgy, but languageId is usually "org.eclipse.cdt.core.g++" which is not good for MSVC 
                languageSetting = l;
        }
        return languageSetting;
    }

    protected static String makeRelativePath(IContainer input, IContainer reference) {
        String path;
        // use relative path if the two projects are in the same OS directory, otherwise absolute path
        if (input.getProject().getLocation().removeLastSegments(1).equals(reference.getProject().getLocation().removeLastSegments(1)))
            path = MakefileTools.makeRelativePath(input.getLocation(), reference.getLocation()).toString();
        else
            path = input.getLocation().toString();
        return path;
    }

}
