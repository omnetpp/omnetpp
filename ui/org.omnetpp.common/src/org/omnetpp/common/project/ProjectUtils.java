/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.project;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Utilities to manage OMNeT++ projects.
 *
 * @author Andras
 */
public class ProjectUtils {
    public static final String NEDFOLDERS_FILENAME = ".nedfolders";
    public static final String NEDEXCLUSIONS_FILENAME = ".nedexclusions";

    /**
     * Checks whether the provided project is open, has the OMNeT++ nature,
     * and it is enabled.
     */
    public static boolean isOpenOmnetppProject(IProject project) throws CoreException {
        // project is open, nature is set and also enabled
        return project.isAccessible() && project.isNatureEnabled(IConstants.OMNETPP_NATURE_ID);
    }

    /**
     * Returns all open projects with the OMNeT++ nature. Based on isOpenOmnetppProject().
     */
    public static IProject[] getOmnetppProjects() throws CoreException {
        List<IProject> omnetppProjects = new ArrayList<IProject>();
        for (IProject project : ResourcesPlugin.getWorkspace().getRoot().getProjects())
            if (isOpenOmnetppProject(project))
                omnetppProjects.add(project);
        return omnetppProjects.toArray(new IProject[]{});
    }

    /**
     * Same as getOmnetppProjects(), but CoreExceptions are caught and displayed
     * in an error dialog and an empty project list is returned.
     */
    public static IProject[] getOmnetppProjectsSafely(Shell parentForErrorDialog) {
        try {
            return getOmnetppProjects();
        }
        catch (CoreException e) {
            ErrorDialog.openError(parentForErrorDialog, "Error", "Could not get list of open OMNeT++ projects", e.getStatus());
            return new IProject[0];
        }
    }

    /**
     * Returns all open projects.
     */
    public static IProject[] getOpenProjects() {
        List<IProject> openProjects = new ArrayList<IProject>();

        for (IProject project : ResourcesPlugin.getWorkspace().getRoot().getProjects())
            if (project.isAccessible())
                openProjects.add(project);

        return openProjects.toArray(new IProject[]{});
    }

    /**
     * Returns the transitive closure of OMNeT++ projects referenced from the given project,
     * excluding the project itself. Nonexistent and closed projects are ignored.
     *
     * Potential CoreExceptions are re-thrown as RuntimeException.
     */
    public static IProject[] getAllReferencedOmnetppProjects(IProject project) throws CoreException {
        return getAllReferencedProjects(project, true, false);
    }

    /**
     * Returns the transitive closure of all projects referenced from the given project,
     * excluding the project itself. Nonexistent and closed projects are ignored.
     */
    public static IProject[] getAllReferencedProjects(IProject project) throws CoreException {
        return getAllReferencedProjects(project, false, false);
    }

    public static IProject[] getAllReferencedProjects(IProject project, boolean requireOmnetppNature, boolean includeSelf) throws CoreException {
        Set<IProject> result = new LinkedHashSet<IProject>();
        if (includeSelf && (requireOmnetppNature ? isOpenOmnetppProject(project) : project.isAccessible()))
            result.add(project);
        collectReferencedProjects(project, requireOmnetppNature, result);
        return result.toArray(new IProject[]{});
    }

    // helper for getAllReferencedOmnetppProjects()
    private static void collectReferencedProjects(IProject project, boolean requireOmnetppNature, Set<IProject> result) throws CoreException {
        for (IProject dependency : project.getReferencedProjects()) {
            if ((requireOmnetppNature ? isOpenOmnetppProject(dependency) : dependency.isAccessible()) && !result.contains(dependency)) {
                result.add(dependency);
                collectReferencedProjects(dependency, requireOmnetppNature, result);
            }
        }
    }

    /**
     * Add the given project as referenced project.
     */
    public static void addReferencedProject(IProject toProject, IProject project, IProgressMonitor monitor) throws CoreException {
        IProjectDescription description = toProject.getDescription();
        IProject[] referencedProjects = description.getReferencedProjects();
        if (!ArrayUtils.contains(referencedProjects, project)) {
            referencedProjects = (IProject[])ArrayUtils.add(referencedProjects, project);
            description.setReferencedProjects(referencedProjects);
            toProject.setDescription(description, monitor);
        }
    }

    /**
     * Return the list of all open projects that directly or indirectly reference the given project.
     * Note: to obtain the list of open projects that directly reference it, use IProject.getReferencingProjects().
     */
    public static IProject[] getAllReferencingProjects(IProject project, boolean includeSelf) throws CoreException {
        Set<IProject> result = new HashSet<IProject>();
        if (includeSelf && project.isAccessible())
            result.add(project);
        collectReferencingProjects(project, result);
        return result.toArray(new IProject[]{});
    }

    // helper for getAllReferencingProjects()
    private static void collectReferencingProjects(IProject project, Set<IProject> result) throws CoreException {
        for (IProject p : project.getReferencingProjects()) {
            if (p.isAccessible() && !result.contains(p)) {
                result.add(p);
                collectReferencingProjects(p, result);
            }
        }
    }

    /**
     * In case of CoreException it simply return false.
     */
    public static boolean hasOmnetppNature(IProject project) {
        try {
            if (project == null)
                return false;
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            return ArrayUtils.contains(natures, IConstants.OMNETPP_NATURE_ID);
        }
        catch (CoreException e) {
            CommonPlugin.logError(e);
            return false;
        }
    }

    /**
     * Add the omnetpp nature to the project (if the project does not have it already)
     */
    public static void addOmnetppNature(IProject project, IProgressMonitor monitor) throws CoreException {
        if (hasOmnetppNature(project))
            return;
        IProjectDescription description = project.getDescription();
        String[] natures = description.getNatureIds();
        description.setNatureIds((String[])ArrayUtils.add(natures, IConstants.OMNETPP_NATURE_ID));
        project.setDescription(description, monitor);
        // note: builders are added automatically, by OmnetppNature.configure()
    }

    /**
     * Removes the omnetpp project nature
     */
    public static void removeOmnetppNature(IProject project) throws CoreException {
        if (!hasOmnetppNature(project))
            return;
        IProjectDescription description = project.getDescription();
        String[] natures = description.getNatureIds();
        description.setNatureIds((String[])ArrayUtils.removeElement(natures, IConstants.OMNETPP_NATURE_ID));
        project.setDescription(description, null);
        // note: builders are removed automatically, by OmnetppNature.deconfigure()
    }

    /**
     * Imports all project from a directory, and optionally opens them.
     */
    public static void importAllProjectsFromDirectory(File directory, boolean open, IProgressMonitor monitor) throws CoreException {
        // note: code based on WizardProjectsImportPage.updateProjectsList()
        if (directory.isDirectory()) {
            // we'll need the names of the currently existing projects
            IProject[] wsProjects = ResourcesPlugin.getWorkspace().getRoot().getProjects();
            Set<String> wsProjectNames = new HashSet<String>();
            for (IProject p : wsProjects)
                wsProjectNames.add(p.getName());

            // iterate through all dirs in the workspace directory and check them
            File[] contents = directory.listFiles();
            final String dotProject = IProjectDescription.DESCRIPTION_FILE_NAME;
            for (int i = 0; i < contents.length; i++) {
                File subdirFile = contents[i];
                if (subdirFile.isDirectory() && !wsProjectNames.contains(subdirFile.getName())) {
                    File dotProjectFile = new File(subdirFile.getPath()+ File.separator + dotProject);
                    if (dotProjectFile.isFile())
                        importProjectFromDirectory(subdirFile, subdirFile.getName(), open, monitor); // we assume directory name is the same as project name in .project
                }
            }
        }
    }

    /**
     * Imports a project from the workspace directory, and optionally opens it.
     * This is basically a convenience wrapper around IProject.create().
     */
    public static IProject importProjectFromDirectory(File directory, String projectName, boolean open, IProgressMonitor monitor) throws CoreException {
        //
        // Note: code based on WizardProjectsImportPage.createExistingProject().
        // Note2: description.setLocation() would only be needed when linking to a project
        // outside the workspace directory
        //
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        final IProject project = workspace.getRoot().getProject(projectName);
        IProjectDescription description = workspace.newProjectDescription(projectName);
        if (!workspace.getRoot().getLocation().isPrefixOf(new Path(directory.getPath())))
            description.setLocation(new Path(directory.toString()));

        try {
            monitor.beginTask("Importing project", 100);
            project.create(description, new SubProgressMonitor(monitor, 30));
            if (open)
                project.open(IResource.NONE, new SubProgressMonitor(monitor, 30));
        }
        finally {
            monitor.done();
        }

        return project;
    }

    /**
     * Utility function to convert a string to a byte array, using the
     * encoding of a resource (IFile). Useful for saving the string into
     * the given file.
     */
    public static byte[] getBytesForFile(String content, IFile file) throws CoreException {
        String charset = file.getCharset();
        try {
            return content.getBytes(charset);
        }
        catch (UnsupportedEncodingException e) {
            throw new CoreException(new Status(IStatus.ERROR, CommonPlugin.PLUGIN_ID, 0, "", e));
        }
    }

    public static boolean isNedFoldersFile(IResource resource) {
        return resource instanceof IFile && resource.getParent() instanceof IProject &&
                (resource.getName().equals(NEDFOLDERS_FILENAME) || resource.getName().equals(NEDEXCLUSIONS_FILENAME));
    }

    /**
     * Reads the ".nedfolders" file from the given OMNeT++ project.
     */
    public static NedSourceFoldersConfiguration readNedFoldersFile(IProject project) throws CoreException {
        List<IContainer> folders = new ArrayList<IContainer>();
        List<String> excludedPackages = new ArrayList<String>();

        // read .nedfolders if exists
        IFile nedFoldersFile = project.getFile(NEDFOLDERS_FILENAME);
        if (!project.getWorkspace().isTreeLocked())
            nedFoldersFile.refreshLocal(IResource.DEPTH_ZERO, null);
        if (nedFoldersFile.exists()) {
            String contents;
            try {
                contents = FileUtils.readTextFile(nedFoldersFile.getContents(), null);
            }
            catch (IOException e) {
                throw CommonPlugin.wrapIntoCoreException("Cannot read " + nedFoldersFile.getFullPath(), e);
            }
            for (String line : StringUtils.splitToLines(contents)) {
                line = line.trim();
                if (line.startsWith("-"))
                    excludedPackages.add(line.substring(1).trim());
                else if (line.equals("."))
                    folders.add(project);
                else if (!line.isEmpty())
                    folders.add(project.getFolder(line));
            }
        }
        if (folders.isEmpty())
            folders.add(project); // this is the default

        // read .nedexclusions if exists
        IFile nedExclusionsFile = project.getFile(NEDEXCLUSIONS_FILENAME);
        if (!project.getWorkspace().isTreeLocked())
            nedExclusionsFile.refreshLocal(IResource.DEPTH_ZERO, null);
        if (nedExclusionsFile.exists()) {
            String contents;
            try {
                contents = FileUtils.readTextFile(nedExclusionsFile.getContents(), null);
            }
            catch (IOException e) {
                throw CommonPlugin.wrapIntoCoreException("Cannot read " + nedExclusionsFile.getFullPath(), e);
            }
            for (String line : StringUtils.splitToLines(contents)) {
                line = line.trim();
                if (!line.isEmpty())
                    excludedPackages.add(line);
            }
        }

        return new NedSourceFoldersConfiguration(folders.toArray(new IContainer[]{}), excludedPackages.toArray(new String[]{}));
    }

    /**
     * Saves the ".nedfolders" file in the given OMNeT++ project.
     */
    public static void saveNedFoldersFile(IProject project, NedSourceFoldersConfiguration config) throws CoreException {
        // save .nedfolders
        String content = "";
        for (IContainer folder : config.getSourceFolders())
            content += getProjectRelativePathOf(project, folder) + "\n";
        IFile nedfoldersFile = project.getFile(NEDFOLDERS_FILENAME);
        if (!nedfoldersFile.exists())
            nedfoldersFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        else
            nedfoldersFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);

        // save .nedexclusions
        content = "";
        for (String packageName : config.getExcludedPackages())
            content += packageName + "\n";
        IFile nedexclusionsFile = project.getFile(NEDEXCLUSIONS_FILENAME);
        if (!nedexclusionsFile.exists())
            nedexclusionsFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
        else
            nedexclusionsFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
    }

    private static String getProjectRelativePathOf(IProject project, IContainer container) {
        return container.equals(project) ? "." : container.getProjectRelativePath().toString();
    }

}
