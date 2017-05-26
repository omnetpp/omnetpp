/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICFolderDescription;
import org.eclipse.cdt.core.settings.model.ICLanguageSetting;
import org.eclipse.cdt.core.settings.model.ICLanguageSettingEntry;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.core.resources.IContainer;
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
public class MetaMakemake {
    /**
     * Generates Makefile in the given folder. May be a long-running operation, mainly because of
     * dependency generation.
     */
    public static void generateMakefile(IContainer makefileFolder, BuildSpecification buildSpec, ICConfigurationDescription configuration, IProgressMonitor monitor) throws CoreException, MakemakeException {
        MakemakeOptions translatedOptions = translateOptions(makefileFolder, buildSpec, configuration, monitor);
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

        ProjectFeaturesManager projectFeatures = new ProjectFeaturesManager(project);
        projectFeatures.loadFeaturesFile();

        MakemakeOptions translatedOptions = options.clone();

        // add -f, and potentially --nmake
        translatedOptions.force = true;

        // add -X option for each excluded folder in CDT, and for each sub-makefile
        translatedOptions.exceptSubdirs.addAll(getExcludedSubpathsWithinFolder(makefileFolder, configuration));

        // add symbols for locations of referenced projects (they will be used by toRelative())
        for (IProject referencedProject : ProjectUtils.getAllReferencedOmnetppProjects(project)) {
            String name = makeSymbolicProjectName(referencedProject);
            String path = makeFriendlyPath(referencedProject, makefileFolder);
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

        // add include folders from this project and exported from referenced projects
        translatedOptions.includeDirs = getIncludePath(makefileFolder, options, configuration, projectFeatures, monitor);
        translatedOptions.metaFeatureCFlags = false;  // they may only contain -D and -I, and -D's are processed elsewhere
        translatedOptions.metaExportIncludePath = false;
        translatedOptions.metaUseExportedIncludePaths = false;

        // add -L and -D options configured in CDT
        translatedOptions.libDirs.addAll(getLibraryPathsFor(makefileFolder, configuration));
        translatedOptions.defines.addAll(getMacrosFor(makefileFolder, configuration));

        // add libraries from directly referenced projects, if requested (but all their -L options)
        for (IProject referencedProject : ProjectUtils.getReferencedOmnetppProjects(project)) {
            BuildSpecification refBuildSpec = BuildSpecification.readBuildSpecFile(referencedProject);
            if (refBuildSpec != null) {
                for (IContainer refMakemakeFolder : refBuildSpec.getMakemakeFolders()) {
                    MakemakeOptions refOptions = refBuildSpec.getMakemakeOptions(refMakemakeFolder);
                    if (refOptions!=null && (refOptions.type==Type.SHAREDLIB || refOptions.type==Type.STATICLIB) && refOptions.metaExportLibrary) {
                        String libname = StringUtils.defaultIfEmpty(refOptions.target, refMakemakeFolder.getProject().getName());
                        String libdir = refMakemakeFolder.getProject().getLocation().append(refMakemakeFolder.getProjectRelativePath()).toString();
                        translatedOptions.libDirs.add(libdir);
                        if (options.metaUseExportedLibs) {
                            translatedOptions.libs.add(libname+"$(D)");
                            if (refOptions.type==Type.SHAREDLIB && !StringUtils.isEmpty(refOptions.dllSymbol))
                                translatedOptions.defines.add(refOptions.dllSymbol + "_IMPORT");
                        }
                    }
                }
            }
        }
        translatedOptions.metaUseExportedLibs = false;

        // add paths to other libraries in this project to the library path
        for (IContainer f : buildSpec.getMakemakeFolders()) {
            if (!f.equals(makefileFolder)) {
                MakemakeOptions otherOptions = buildSpec.getMakemakeOptions(f);
                if (otherOptions!=null && (otherOptions.type==Type.SHAREDLIB || otherOptions.type==Type.STATICLIB)) {
                    String outdir = StringUtils.isEmpty(otherOptions.outRoot) ? "out" : otherOptions.outRoot; //FIXME hardcoded default!!!
                    String libdir = f.getProject().getLocation().append(outdir).append("$(CONFIGNAME)").append(f.getProjectRelativePath()).toString();
                    translatedOptions.libDirs.add(libdir);
                    if (otherOptions.type==Type.SHAREDLIB && !StringUtils.isEmpty(otherOptions.dllSymbol))
                        translatedOptions.defines.add(otherOptions.dllSymbol + "_IMPORT");
                }
            }
        }

        // add LDFLAGS contributed by enabled project features
        if (options.metaFeatureLDFlags) {
            List<ProjectFeature> enabledFeatures = projectFeatures.getEnabledFeatures();
            List<String> ldflags = projectFeatures.getFeatureLDFlags(enabledFeatures);
            for (String ldflag : ldflags) {
                // we only need to handle -l and -L here, and can simply ignore the rest:
                // validateFeatures() already reports them as errors. We can also reject "-L <path>"
                // (i.e. with a space), because validateFeatures() also complains about it.
                if (ldflag.startsWith("-l") && ldflag.length()>2)
                    translatedOptions.libs.add(ldflag.substring(2));
                else if (ldflag.startsWith("-L")  && ldflag.length()>2)
                    translatedOptions.libDirs.add(ldflag.substring(2));
            }

            // clear processed setting
            translatedOptions.metaFeatureLDFlags = false;
        }

        if (translatedOptions.metaExportLibrary) {
            // no processing required
            translatedOptions.metaExportLibrary = false;
        }

        // substitute all ${eclipse_variables}
        translatedOptions.substituteVariables(makefileFolder.getProject());

        // convert paths to relative where convenient
        translatedOptions.includeDirs = toRelative(translatedOptions.includeDirs, makefileFolder);
        translatedOptions.libDirs = toRelative(translatedOptions.libDirs, makefileFolder);

        // Debug.println("Translated makemake options for " + makefileFolder + ": " + translatedOptions.toString());
        return translatedOptions;
    }

    protected static List<String> getIncludePath(IContainer makefileFolder, MakemakeOptions options, ICConfigurationDescription configuration, ProjectFeaturesManager projectFeatures, IProgressMonitor monitor) throws CoreException {
        List<String> result = getLocalIncludePath(makefileFolder, options, configuration, projectFeatures, monitor);

        // add include folders exported from referenced projects
        if (options.metaUseExportedIncludePaths) {
            IProject project = makefileFolder.getProject();
            for (IProject referencedProject : ProjectUtils.getReferencedOmnetppProjects(project)) {
                ICProjectDescription refProjDesc = CoreModel.getDefault().getProjectDescription(referencedProject);
                ICConfigurationDescription refConfiguration = refProjDesc==null ? null : refProjDesc.getActiveConfiguration();
                BuildSpecification refBuildSpec = BuildSpecification.readBuildSpecFile(referencedProject);
                ProjectFeaturesManager refProjectFeatures = new ProjectFeaturesManager(referencedProject);
                refProjectFeatures.loadFeaturesFile();
                if (refBuildSpec != null && refProjDesc != null) {
                    for (IContainer refMakemakeFolder : refBuildSpec.getMakemakeFolders()) {
                        MakemakeOptions refOptions = refBuildSpec.getMakemakeOptions(refMakemakeFolder);
                        if (refOptions != null && refOptions.metaExportIncludePath) {
                            List<String> refIncludePath = getIncludePath(refMakemakeFolder, refOptions, refConfiguration, refProjectFeatures, monitor);
                            addAllNewOnes(result, refIncludePath);
                        }
                    }
                }
            }
        }
        return result;
    }

    protected static List<String> getLocalIncludePath(IContainer makefileFolder, MakemakeOptions options, ICConfigurationDescription configuration, ProjectFeaturesManager projectFeatures, IProgressMonitor monitor) throws CoreException {
        List<String> result = new ArrayList<String>();

        // add -I options in local makemake options
        result.addAll(toAbsolute(options.includeDirs, makefileFolder));

        // add include folders configured in CDT
        result.addAll(toAbsolute(getIncludePathsFor(makefileFolder, configuration), makefileFolder));

        // add feature CFLAGs if enabled
        if (options.metaFeatureCFlags) {
            List<ProjectFeature> enabledFeatures = projectFeatures.getEnabledFeatures();
            List<String> cflags = projectFeatures.getFeatureCFlags(enabledFeatures);
            for (String cflag : cflags) {
                // we only need to handle -I here, and can simply ignore the rest: -D's are put into the
                // generated header file, and validateFeatures() reports all other options as errors.
                // We can also reject "-I <path>" (i.e. with a space), because validateFeatures() also complains about it.
                if (cflag.startsWith("-I") && cflag.length()>2)
                    result.add(toAbsolute(cflag.substring(2), makefileFolder));
            }
        }
        return result;
    }

    protected static void addAllNewOnes(List<String> list, List<String> items) {
        for (String item : items)
            if (!list.contains(item))
                list.add(item);
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

    protected static List<String> toAbsolute(List<String> dirs, IContainer baseFolder) {
        List<String> result = new ArrayList<String>();
        for (String dir : dirs)
            result.add(toAbsolute(dir, baseFolder));
        return result;
    }

    protected static String toAbsolute(String dir, IContainer baseFolder) {
        if (dir.charAt(0) == '$')
            return dir; // don't mess with makefile variable references, as they may already expand to absolute paths
        else {
            Path dirPath = new Path(dir);
            return dirPath.isAbsolute() ? dir : baseFolder.getLocation().append(dirPath).toString();
        }
    }

    protected static List<String> toRelative(List<String> dirs, IContainer baseFolder) throws CoreException {
        List<String> result = new ArrayList<String>();
        for (String dir : dirs)
            result.add(toRelative(dir, baseFolder));
        return result;
    }

    protected static String toRelative(String location, IContainer baseFolder) throws CoreException {
        return toRelative(new Path(location), baseFolder).toString();
    }

    protected static IPath toRelative(IPath location, IContainer baseFolder) throws CoreException {
        if (!location.isAbsolute()) {
            // leave relative paths untouched
            return location;
        }
        else if (baseFolder.getProject().getLocation().isPrefixOf(location)) {
            // location is within the same project, make it relative
            return MakefileTools.makeRelativePath(location, baseFolder.getLocation());
        }
        else {
            IProject containingProject = null;
            for (IProject project : ProjectUtils.getAllReferencedOmnetppProjects(baseFolder.getProject())) {
                if (project.getLocation().isPrefixOf(location)) {
                    containingProject = project; break;
                }
            }
            if (containingProject != null) {
                // generate something like $(OTHER_PROJECT_DIR)/some/file
                IPath projectRelativePath = location.removeFirstSegments(containingProject.getLocation().segmentCount());
                String symbolicProjectName = MetaMakemake.makeSymbolicProjectName(containingProject);
                return new Path("$(" + symbolicProjectName + ")").append(projectRelativePath);
            }
            else {
                // points outside the project -- leave it as it is
                return location;
            }
        }
    }

    public static String makeSymbolicProjectName(IProject project) {
        return StringUtils.makeValidIdentifier(project.getName()).toUpperCase()+"_PROJ";
    }

    /**
     * Return relative path if the two projects containing the folders are "close" on the disk, otherwise absolute path.
     */
    protected static String makeFriendlyPath(IContainer input, IContainer reference) {
        IPath wsLocation = ResourcesPlugin.getWorkspace().getRoot().getLocation();
        boolean bothInWorkspace = wsLocation.isPrefixOf(input.getLocation()) && wsLocation.isPrefixOf(reference.getLocation());
        boolean inNeighboringProjects = input.getProject().getLocation().removeLastSegments(1).equals(reference.getProject().getLocation().removeLastSegments(1));
        if (bothInWorkspace || inNeighboringProjects)
            return MakefileTools.makeRelativePath(input.getLocation(), reference.getLocation()).toString();
        else
            return input.getLocation().toString();
    }

}
