package org.omnetpp.launch;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.common.project.ProjectUtils;

/**
 * Assembles the value of the NEDPATH environment variable. Argument
 * is the omnetpp.ini file, or any resource in its project.
 * @author Andras
 */
public class NedPathResolver implements IDynamicVariableResolver {

	public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
		if (argument == null)
			throw new IllegalArgumentException("nedpath resolver requires an argument");
		
		IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
		if (resource == null)
			throw new IllegalArgumentException("argument to nedpath resolver needs to be an existing file, folder, or project");

		IProject project = resource.getProject();
		String result = project.getLocation().toOSString();
		for (IProject p : ProjectUtils.getAllReferencedOmnetppProjects(project))
			result += ";" + p.getLocation().toOSString();
		return result;
	}

}
