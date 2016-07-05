/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.ArrayList;
import java.util.List;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICFolderDescription;
import org.eclipse.cdt.core.settings.model.ICLanguageSetting;
import org.eclipse.cdt.core.settings.model.ICLanguageSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.MakemakeOptions.Type;
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
//FIXME automatic includes from sub-make folders don't work!
public class MetaMakemake {
    /**
     * Generates Makefile in the given folder. May be a long-running operation, mainly because of
     * dependency generation.
     */
    public static void generateMakefile(IContainer makefileFolder, BuildSpecification buildSpec, ICConfigurationDescription configuration, IProgressMonitor monitor) throws CoreException, MakemakeException {
        MakemakeOptions translatedOptions = translateOptions(makefileFolder, buildSpec, configuration, monitor);
        IProject project = makefileFolder.getProject();

        new Makemake().generateMakefile(makefileFolder, translatedOptions);
    }

    /**
     * Translates makemake options. Currently may be a long-running operation due to the getFolderDependencies() call,
     * but that does not seem to be strictly necessary.
     */
    public static MakemakeOptions translateOptions(IContainer makefileFolder, BuildSpecification buildSpec, ICConfigurationDescription configuration, IProgressMonitor monitor) throws CoreException {
        Assert.isTrue(buildSpec.getFolderMakeType(makefileFolder)==BuildSpecification.MAKEMAKE);

        MakemakeOptions options = buildSpec.getMakemakeOptions(makefileFolder);
        List<IContainer> makeFolders = buildSpec.getMakeFolders();
        IProject project = makefileFolder.getProject();

        MakemakeOptions translatedOptions = options.clone();

        // add -f, and potentially --nmake
        translatedOptions.force = true;
        translatedOptions.isNMake = CDTUtils.isMsvcConfiguration(configuration);

        // add -X option for each excluded folder in CDT, and for each sub-makefile
        translatedOptions.exceptSubdirs.addAll(getExcludedSubpathsWithinFolder(makefileFolder, configuration));

        // add -I, -L and -D options configured in CDT
        translatedOptions.includeDirs.addAll(getIncludePathsFor(makefileFolder, configuration));
        translatedOptions.libDirs.addAll(getLibraryPathsFor(makefileFolder, configuration));
        translatedOptions.defines.addAll(getMacrosFor(makefileFolder, configuration));

//        // add extra include folders (this code assumes that exceptSubdirs are already filled in at this point)
//        if (options.metaAutoIncludePath && folderDeps != null) {
//            for (IContainer srcFolder : folderDeps.keySet())
//                if (MakefileTools.makefileCovers(makefileFolder, srcFolder, options.isDeep, options.exceptSubdirs, makeFolders))
//                    for (IContainer dep : folderDeps.get(srcFolder))
//                        if (!MakefileTools.makefileCovers(makefileFolder, dep, options.isDeep, options.exceptSubdirs, makeFolders)) // only add if "dep" is outside "folder"!
//                            addLocationToDirList(dep, translatedOptions.includeDirs, makefileFolder);
//
//            // clear processed setting
//            translatedOptions.metaAutoIncludePath = false;
//        }

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

        ProjectFeaturesManager projectFeatures = new ProjectFeaturesManager(project);
        projectFeatures.loadFeaturesFile();

        // add CFLAGS contributed by enabled project features
        if (options.metaFeatureCFlags) {
            projectFeatures.addFeatureCFlagsTo(translatedOptions);
            translatedOptions.metaFeatureCFlags = false;
        }

        // add libraries from other projects, if requested
        if (options.metaUseExportedLibs) {
            for (IProject referencedProject : referencedProjects) {
                BuildSpecification refBuildSpec = BuildSpecification.readBuildSpecFile(referencedProject);
                if (refBuildSpec != null) {
                    for (IContainer f : refBuildSpec.getMakemakeFolders()) {
                        MakemakeOptions opt = refBuildSpec.getMakemakeOptions(f);
                        if (opt!=null && (opt.type==Type.SHAREDLIB || opt.type==Type.STATICLIB) && opt.metaExportLibrary) {
                            String libname = StringUtils.defaultIfEmpty(opt.target, f.getProject().getName());
                            String outdir = StringUtils.defaultIfEmpty(opt.outRoot, "out");
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

        // add paths to other libraries in this project to the library path
        for (IContainer f : buildSpec.getMakemakeFolders()) {
            if (!f.equals(makefileFolder)) {
                MakemakeOptions opt = buildSpec.getMakemakeOptions(f);
                if (opt!=null && (opt.type==Type.SHAREDLIB || opt.type==Type.STATICLIB)) {
                    String outdir = StringUtils.isEmpty(opt.outRoot) ? "out" : opt.outRoot; //FIXME hardcoded default!!!
                    String libdir = makeRelativePath(f.getProject(), makefileFolder) + "/" + new Path(outdir).append("$(CONFIGNAME)").append(f.getProjectRelativePath()).toString(); //FIXME why
                    translatedOptions.libDirs.add(libdir);
                    if (opt.type==Type.SHAREDLIB && !StringUtils.isEmpty(opt.dllSymbol))
                        translatedOptions.defines.add(opt.dllSymbol + "_IMPORT");
                }
            }
        }

        // add LDFLAGS contributed by enabled project features
        if (options.metaFeatureLDFlags) {
            projectFeatures.addFeatureLDFlagsTo(translatedOptions);
            translatedOptions.metaFeatureLDFlags = false;
        }

        if (translatedOptions.metaExportLibrary) {
            // no processing required
            translatedOptions.metaExportLibrary = false;
        }

        // substitute all ${eclipse_variables}
        translatedOptions.substituteVariables(makefileFolder.getProject());

        // Debug.println("Translated makemake options for " + makefileFolder + ": " + translatedOptions.toString());
        return translatedOptions;
    }

    /**
     * Returns the (folder-relative) paths of directories excluded from build
     * in the active CDT configuration under the given source folder.
     * File exclusions are ignored.
     */
    protected static List<String> getExcludedSubpathsWithinFolder(IContainer makemakeFolder, ICConfigurationDescription configuration) throws CoreException {
        List<String> result = new ArrayList<String>();
        collectExcludedSubpaths(makemakeFolder, makemakeFolder, configuration.getSourceEntries(), result);
        return result;
    }

    private static void collectExcludedSubpaths(IContainer folder, IContainer makemakeFolder, ICSourceEntry sourceEntries[], List<String> result) throws CoreException {
        if (MakefileTools.isGoodFolder(folder)) {
            if (CDTUtils.isExcluded(folder, sourceEntries)) {
                // exclude and don't go into it
                IPath sourceFolderRelativePath = folder.getFullPath().removeFirstSegments(makemakeFolder.getFullPath().segmentCount());
                result.add(sourceFolderRelativePath.isEmpty() ? "." : sourceFolderRelativePath.toString());
            }
            else {
                // recurse into subdirs
                for (IResource member : folder.members())
                    if (member instanceof IContainer)
                        collectExcludedSubpaths((IContainer)member, makemakeFolder, sourceEntries, result);
            }
        }
    }

    /**
     * Returns the include paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getIncludePathsFor(IContainer folder, ICConfigurationDescription configuration) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, configuration, false);
        if (languageSetting == null)
            return new ArrayList<String>();
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.INCLUDE_PATH));
    }

    /**
     * Returns the library paths for the given folder, in the active configuration
     * and for the C++ language. Built-in paths are skipped. The returned strings
     * are filesystem paths.
     */
    protected static List<String> getLibraryPathsFor(IContainer folder, ICConfigurationDescription configuration) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, configuration, true);
        if (languageSetting == null)
            return new ArrayList<String>();
        return getPaths(languageSetting.getSettingEntries(ICSettingEntry.LIBRARY_PATH));
    }

    /**
     * Returns the defined macros (in NAME or NAME=VALUE form) for the given folder,
     * in the active configuration and for the C++ language. Built-in ones are skipped.
     */
    protected static List<String> getMacrosFor(IContainer folder, ICConfigurationDescription configuration) {
        ICLanguageSetting languageSetting = findCCLanguageSettingFor(folder, configuration, false);
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
                    IPath path = new Path(value);
                    IWorkspaceRoot wsRoot = ResourcesPlugin.getWorkspace().getRoot();
                    IContainer folder = (path.segmentCount() == 1) ? wsRoot.getProject(path.segment(0)) : wsRoot.getFolder(path);
                    IPath location = folder.getLocation(); // null if project doesn't exist etc
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
    protected static ICLanguageSetting findCCLanguageSettingFor(IContainer folder, ICConfigurationDescription configuration, boolean forLinker) {
        // find folder description
        ICFolderDescription folderDescription = (ICFolderDescription)configuration.getResourceDescription(folder.getProjectRelativePath(), false);

        // find C++ language settings for this folder
        ICLanguageSetting[] languageSettings = folderDescription.getLanguageSettings();
        return CDTUtils.findCplusplusLanguageSetting(languageSettings, forLinker);
    }

    protected static void addLocationToDirList(IContainer input, List<String> dirList, IContainer reference) {
        String relativePath = makeRelativePath(input, reference);
        if (!dirList.contains(input.getLocation().toString()) && !dirList.contains(relativePath))
            dirList.add(relativePath);
    }

    protected static String makeRelativePath(IContainer input, IContainer reference) {
        // use relative path if the two projects are in the same directory on the disk, otherwise absolute path
        if (input.getProject().getLocation().removeLastSegments(1).equals(reference.getProject().getLocation().removeLastSegments(1)))
            return MakefileTools.makeRelativePath(input.getLocation(), reference.getLocation()).toString();
        else
            return input.getLocation().toString();
    }

}
