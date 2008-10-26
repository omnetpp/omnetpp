package org.omnetpp.cdt.makefile;

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
 * NOTE: we only support excluding whole folders. In CDT one can exclude individual files too, 
 * but currently our generated makefiles ignore that.
 *
 * @author Andras
 */
public class MetaMakemake {
    /**
     * Generates Makefile in the given folder.
     */
    public static void generateMakefile(IContainer makefileFolder, MakemakeOptions options, List<IContainer> makeFolders) throws CoreException, MakemakeException {
        MakemakeOptions translatedOptions = translateOptions(makefileFolder, options, makeFolders);
        IProject project = makefileFolder.getProject();

        // Activator.getDependencyCache().dumpPerFileDependencies(project);
        
        Map<IContainer, Map<IFile, Set<IFile>>> perFileDependencies = Activator.getDependencyCache().getPerFileDependencies(project);
        new Makemake().generateMakefile(makefileFolder, translatedOptions, perFileDependencies);
    }

    /** 
     * Translates makemake options
     */
    public static MakemakeOptions translateOptions(IContainer makefileFolder, MakemakeOptions options, List<IContainer> makeFolders) throws CoreException {
        MakemakeOptions translatedOptions = options.clone();

        IProject project = makefileFolder.getProject();
        Map<IContainer, Set<IContainer>> folderDeps = Activator.getDependencyCache().getFolderDependencies(project);

        // add -f, and potentially --nmake 
        translatedOptions.force = true;
        translatedOptions.isNMake = CDTUtils.isMsvcToolchainActive(project);

        // add -X option for each excluded folder in CDT, and for each sub-makefile
        translatedOptions.exceptSubdirs.addAll(getExcludedSubpathsWithinFolder(makefileFolder, makeFolders));

        // add -I, -L and -D options configured in CDT
        translatedOptions.includeDirs.addAll(getIncludePathsFor(makefileFolder));
        translatedOptions.libDirs.addAll(getLibraryPathsFor(makefileFolder));
        translatedOptions.defines.addAll(getMacrosFor(makefileFolder));

        // add symbols for locations of referenced projects (they will be used by Makemake.abs2rel())
        IProject[] referencedProjects = ProjectUtils.getAllReferencedProjects(project);
        for (IProject referencedProject : referencedProjects) {
            String name = Makemake.makeSymbolicProjectName(referencedProject);
            String path = makeRelativePath(referencedProject, makefileFolder);
            translatedOptions.makefileVariables.add(name + "=" + path);
        }

        // add sub-make folders
        if (options.metaRecurse) {
            // collect folders in our subtree, and convert to relative path
            List<IPath> submakePaths = new ArrayList<IPath>();
            for (IContainer folder : makeFolders)
                if (!folder.equals(makefileFolder) && makefileFolder.getFullPath().isPrefixOf(folder.getFullPath()))
                    submakePaths.add(folder.getFullPath().removeFirstSegments(makefileFolder.getFullPath().segmentCount()));
            // filter: throw out the ones whose prefix is also in the list 
            // (i.e. we won't recurse into "foo/bar" if "foo" is already on our list)
            for (IPath candidate : submakePaths) {
                boolean ok = true;
                for (IPath otherPath : submakePaths)
                    if (!candidate.equals(otherPath) && otherPath.isPrefixOf(candidate)) {
                        ok = false; break;}
                if (ok)
                    translatedOptions.submakeDirs.add(candidate.toString());
            }
            
            // clear processed setting
            translatedOptions.metaRecurse = false;
        }
        
        // add extra include folders (this code assumes that exceptSubdirs are already filled in at this point)
        if (options.metaAutoIncludePath && folderDeps != null) {
            for (IContainer srcFolder : folderDeps.keySet())
                if (MakefileTools.makefileCovers(makefileFolder, srcFolder, options.isDeep, options.exceptSubdirs))  
                    for (IContainer dep : folderDeps.get(srcFolder))
                        if (!MakefileTools.makefileCovers(makefileFolder, dep, options.isDeep, options.exceptSubdirs)) // only add if "dep" is outside "folder"!  
                            if (!translatedOptions.includeDirs.contains(dep.getLocation().toString()))  // if not added yet
                                translatedOptions.includeDirs.add(dep.getLocation().toString());

            // clear processed setting
            translatedOptions.metaAutoIncludePath = false;
        }

        // add libraries from other projects, if requested
        if (options.metaUseExportedLibs) {
            for (IProject referencedProject : referencedProjects) {
                BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(referencedProject);
                if (buildSpec != null) {
                    for (IContainer f : buildSpec.getMakemakeFolders()) {
                        MakemakeOptions opt = buildSpec.getMakemakeOptions(f);
                        if (opt!=null && (opt.type==Type.SHAREDLIB || opt.type==Type.STATICLIB) && opt.metaExportLibrary) {
                            String libname = StringUtils.isEmpty(opt.target) ? f.getProject().getName() : opt.target;
                            String outdir = StringUtils.isEmpty(opt.outRoot) ? "out" : opt.outRoot; //FIXME hardcoded default!!!
                            String libdir = makeRelativePath(f.getProject(), makefileFolder) + "/" + new Path(outdir).append("$(CONFIGNAME)").append(f.getProjectRelativePath()).toString();
                            translatedOptions.libs.add(libname);
                            translatedOptions.libDirs.add(libdir);
                            if (opt.type==Type.SHAREDLIB && !StringUtils.isEmpty(opt.dllSymbol))
                                translatedOptions.defines.add(opt.dllSymbol + "_IMPORT");
                        }
                    }
                }
            }
            translatedOptions.metaUseExportedLibs = false;
        }

        if (translatedOptions.metaExportLibrary) {
            // no processing required
            translatedOptions.metaExportLibrary = false;
        }
        
        // substitute all ${eclipse_variables}
        translatedOptions.substituteVariables(makefileFolder.getProject());
        
        // System.out.println("Translated makemake options for " + makefileFolder + ": " + translatedOptions.toString());
        return translatedOptions;
    }

    /**
     * Returns the (folder-relative) paths of directories excluded from build 
     * in the active CDT configuration under the given source folder.
     * Excluded files are not considered. The subtrees covered by sub-makes
     * also gets excluded. 
     */
    protected static List<String> getExcludedSubpathsWithinFolder(IContainer makemakeFolder, List<IContainer> makeFolders) throws CoreException {
        List<String> result = new ArrayList<String>();
        collectExcludedSubpaths(makemakeFolder, makemakeFolder, CDTUtils.getSourceEntries(makemakeFolder.getProject()), makeFolders, result);
        return result;
    }

    private static void collectExcludedSubpaths(IContainer folder, IContainer makemakeFolder, ICSourceEntry sourceEntries[], List<IContainer> makeFolders, List<String> result) throws CoreException {
        if (MakefileTools.isGoodFolder(folder)) {
            if (CDTUtils.isExcluded(folder, sourceEntries) || (!folder.equals(makemakeFolder) && makeFolders.contains(folder))) {
                // exclude and don't go into it
                IPath sourceFolderRelativePath = folder.getFullPath().removeFirstSegments(makemakeFolder.getFullPath().segmentCount());
                result.add(sourceFolderRelativePath.isEmpty() ? "." : sourceFolderRelativePath.toString());
            }
            else {
                // recurse into subdirs
                for (IResource member : folder.members())
                    if (member instanceof IContainer)
                        collectExcludedSubpaths((IContainer)member, makemakeFolder, sourceEntries, makeFolders, result);
            }
        }
    }

    /**
     * Returns the include paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getIncludePathsFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, false);
        if (languageSetting == null)
            return new ArrayList<String>();
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.INCLUDE_PATH));
    }

    /**
     * Returns the library paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getLibraryPathsFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, true);
        if (languageSetting == null)
            return new ArrayList<String>();
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.LIBRARY_PATH));
    }

    /**
     * Returns the defined macros (in NAME or NAME=VALUE form) for the given folder,
     * in the active configuration and for the C++ language. Built-in ones are skipped.
     */
    protected static List<String> getMacrosFor(IContainer folder) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, false);
        if (languageSetting == null)
            return new ArrayList<String>();
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
                    // convert to file-system path
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

//      XXX debug code        
//      String buf = "Languages:";
//      for (ICLanguageSetting l : languageSettings)
//      buf += l.getId() + "=" + l.getName() + ",  ";
//      System.out.println(buf);

        ICLanguageSetting languageSetting = null;
        for (ICLanguageSetting l : languageSettings) 
            if (l.getName().contains(forLinker ? "Object" : "C++"))  //FIXME ***must*** use languageId!!!! (usually "org.eclipse.cdt.core.g++" or something) 
                languageSetting = l;
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
