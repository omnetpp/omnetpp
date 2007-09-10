package org.omnetpp.common.project;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Utilities to manage OMNeT++ projects.
 * 
 * @author Andras
 */
public class ProjectUtils {
	public static final String OMNETPP_NATURE = "org.omnetpp.main.omnetppnature";
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
			return project.isAccessible() && project.isNatureEnabled(OMNETPP_NATURE);
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
        	if (ProjectUtils.isOpenOmnetppProject(project))
        		omnetppProjects.add(project);
        return omnetppProjects.toArray(new IProject[]{});
	}

	
	/**
	 * Returns the transitive closure of OMNeT++ projects referenced from the given project,
	 * excluding the project itself.
	 * 
	 * Potential CoreExceptions are re-thrown as RuntimeException.
	 */
	public static IProject[] getAllReferencedOmnetppProjects(IProject project) {
		try {
			Set<IProject> result = new HashSet<IProject>();
			collectAllReferencedOmnetppProjects(project, result);
			return result.toArray(new IProject[]{});
		} catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}

	// helper for getAllReferencedOmnetppProjects()
	private static void collectAllReferencedOmnetppProjects(IProject project, Set<IProject> result) throws CoreException {
		for (IProject dependency : project.getReferencedProjects()) {
			if (isOpenOmnetppProject(dependency)) {
				result.add(dependency);
				collectAllReferencedOmnetppProjects(dependency, result);
			}
		}
	}

	/**
	 * Reads the ".nedfolders" file from the given OMNeT++ project.
	 */
	public static IContainer[] readNedFoldersFile(IProject project) throws IOException, CoreException {
		List<IContainer> result = new ArrayList<IContainer>();
		IFile nedFoldersFile = project.getFile(ProjectUtils.NEDFOLDERS_FILENAME);
		if (nedFoldersFile.exists()) {
			String contents = FileUtils.readTextFile(nedFoldersFile.getContents());
			for (String line : StringUtils.splitToLines(contents))
				if (!StringUtils.isBlank(line))
					result.add(project.getFolder(line.trim())); //FIXME what notation is the project itself?
		}
		System.out.println("Project "+ project.getName() + " NED source folders: " + StringUtils.join(result, ", "));
		return result.toArray(new IContainer[]{});
	}
    
	/**
	 * Saves the ".nedfolders" file in the given OMNeT++ project.
	 */
    public static void saveNedFoldersFile(IProject project, IContainer[] folders) throws CoreException {
    	// assemble file content to save
    	String content = "";
    	for (IContainer element : folders)
    		content += getProjectRelativePathOf(project, element) + "\n"; //FIXME what notation is the project itself?

    	// save it
    	IFile nedpathFile = project.getFile(NEDFOLDERS_FILENAME);
    	if (!nedpathFile.exists())
    		nedpathFile.create(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
    	else
    		nedpathFile.setContents(new ByteArrayInputStream(content.getBytes()), IFile.FORCE, null);
	} 
    
	private static String getProjectRelativePathOf(IProject project, IContainer container) {
		Assert.isTrue(container != null);
		if (container == project)
			return "";
		else
			return StringUtils.removeStart(container.getFullPath().toString(), project.getFullPath().toString()+"/");
	}

}
