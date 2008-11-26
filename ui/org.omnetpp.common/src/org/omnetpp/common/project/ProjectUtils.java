package org.omnetpp.common.project;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Utilities to manage OMNeT++ projects.
 *
 * @author Andras
 */
//FIXME this "potential CoreExceptions are re-thrown as RuntimeException" looks like a dubious practice...
public class ProjectUtils {
	public static final String NEDFOLDERS_FILENAME = ".nedfolders";

	/**
	 * Checks whether the the provided project is open, has the OMNeT++ nature,
	 * and it is enabled.
	 *
	 * Potential CoreExceptions are re-thrown as RuntimeException.
	 */
	public static boolean isOpenOmnetppProject(IProject project) {
		try {
			// project is open, nature is set and also enabled
			return project.isAccessible() && project.isNatureEnabled(IConstants.OMNETPP_NATURE_ID);
		}
		catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}

	/**
	 * Returns all open projects with the OMNeT++ nature. Based on isOpenOmnetppProject().
	 */
	public static IProject[] getOmnetppProjects() {
		List<IProject> omnetppProjects = new ArrayList<IProject>();
        for (IProject project : ResourcesPlugin.getWorkspace().getRoot().getProjects())
        	if (isOpenOmnetppProject(project))
        		omnetppProjects.add(project);
        return omnetppProjects.toArray(new IProject[]{});
	}


	/**
	 * Returns the transitive closure of OMNeT++ projects referenced from the given project,
	 * excluding the project itself. Nonexistent and closed projects are ignored.
	 *
	 * Potential CoreExceptions are re-thrown as RuntimeException.
	 */
	public static IProject[] getAllReferencedOmnetppProjects(IProject project) {
        return getAllReferencedProjects(project, true, false);
	}

	/**
     * Returns the transitive closure of all projects referenced from the given project,
     * excluding the project itself. Nonexistent and closed projects are ignored.
     *
     * Potential CoreExceptions are re-thrown as RuntimeException.
     */
    public static IProject[] getAllReferencedProjects(IProject project) {
        return getAllReferencedProjects(project, false, false);
    }

    public static IProject[] getAllReferencedProjects(IProject project, boolean requireOmnetppNature, boolean includeSelf) {
        try {
            Set<IProject> result = new HashSet<IProject>();
            if (includeSelf && (requireOmnetppNature ? isOpenOmnetppProject(project) : project.isAccessible()))
                result.add(project);
            collectReferencedProjects(project, requireOmnetppNature, result);
            return result.toArray(new IProject[]{});
        } catch (CoreException e) {
            throw new RuntimeException(e);
        }
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

	public static boolean isNedFoldersFile(IResource resource) {
		return (resource instanceof IFile &&
				resource.getParent() instanceof IProject &&
				resource.getName().equals(NEDFOLDERS_FILENAME));
	}

	/**
	 * Reads the ".nedfolders" file from the given OMNeT++ project.
	 */
	public static IContainer[] readNedFoldersFile(IProject project) throws IOException, CoreException {
		List<IContainer> result = new ArrayList<IContainer>();
		IFile nedFoldersFile = project.getFile(NEDFOLDERS_FILENAME);
		if (nedFoldersFile.exists()) {
			String contents = FileUtils.readTextFile(nedFoldersFile.getContents());
			for (String line : StringUtils.splitToLines(contents)) {
				line = line.trim();
				if (line.equals("."))
					result.add(project);
				else if (line.length()>0)
					result.add(project.getFolder(line)); 
			}
		}
		if (result.isEmpty())
			result.add(project); // this is the default
		return result.toArray(new IContainer[]{});
	}

	/**
	 * Saves the ".nedfolders" file in the given OMNeT++ project.
	 */
    public static void saveNedFoldersFile(IProject project, IContainer[] folders) throws CoreException {
    	// assemble file content to save
    	String content = "";
    	for (IContainer element : folders)
    		content += getProjectRelativePathOf(project, element) + "\n";

    	// save it
    	IFile nedpathFile = project.getFile(NEDFOLDERS_FILENAME);
    	if (!nedpathFile.exists())
    		nedpathFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
    	else
    		nedpathFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
	}

	private static String getProjectRelativePathOf(IProject project, IContainer container) {
		return container.equals(project) ? "." : container.getProjectRelativePath().toString();
	}

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
    public static void addOmnetppNature(IProject project, IProgressMonitor monitor) {
        try {
            if (hasOmnetppNature(project))
                return;
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            description.setNatureIds((String[])ArrayUtils.add(natures, IConstants.OMNETPP_NATURE_ID));
            project.setDescription(description, monitor);
            // note: builders are added automatically, by OmnetppNature.configure()
        } 
        catch (CoreException e) {
            CommonPlugin.logError(e);
        }
    }

    /**
     * Removes the omnetpp project nature
     */
    public static void removeOmnetppNature(IProject project) {
        try {
            if (!hasOmnetppNature(project))
                return;
            IProjectDescription description = project.getDescription();
            String[] natures = description.getNatureIds();
            description.setNatureIds((String[])ArrayUtils.removeElement(natures, IConstants.OMNETPP_NATURE_ID));
            project.setDescription(description, null);
            // note: builders are removed automatically, by OmnetppNature.deconfigure()
        } 
        catch (CoreException e) {
            CommonPlugin.logError(e);
        }
    }

    /**
     * Imports all project from the workspace directory, and optionally opens them.
     */
    public static void importAllProjectsFromWorkspaceDirectory(boolean open, IProgressMonitor monitor) throws CoreException {
        // note: code based on WizardProjectsImportPage.updateProjectsList()
        IPath workspaceLocation = ResourcesPlugin.getWorkspace().getRoot().getLocation();
        File directory = workspaceLocation.toFile();
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
                    if (dotProjectFile.isFile()) {
                        importProjectFromWorkspaceDirectory(subdirFile.getName(), open, monitor);
                    }
                }
            }
        }
    }

    /**
     * Imports a project from the workspace directory, and optionally opens it.
     * This is basically a convenience wrapper around IProject.create().
     */
    public static IProject importProjectFromWorkspaceDirectory(String projectName, boolean open, IProgressMonitor monitor) throws CoreException {
        //
        // Note: code based on WizardProjectsImportPage.createExistingProject().
        // Note2: description.setLocation() would only be needed when linking to a project 
        // outside the workspace directory
        //
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        final IProject project = workspace.getRoot().getProject(projectName);
        IProjectDescription description = workspace.newProjectDescription(projectName);
        
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

}
