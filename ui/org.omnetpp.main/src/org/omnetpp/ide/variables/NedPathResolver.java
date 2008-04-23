package org.omnetpp.ide.variables;

import org.eclipse.core.internal.runtime.IRuntimeConstants;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * Assembles the value of the NEDPATH environment variable. Argument
 * is the omnetpp.ini file, or any resource in its project.
 * @author Andras
 */
public class NedPathResolver implements IDynamicVariableResolver {

	public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
		if (argument == null)
			abort("${ned_path:arg} requires an argument", null);

		IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
		if (resource == null)
			abort("argument to ${ned_path:arg} needs to be an existing file, folder, or project", null);

		IProject project = resource.getProject();
		String result = project.getLocation().toOSString();
		// resolve the ned path files
		try {
			result = "";
			String pathSep = System.getProperty("path.separator"); 
			// read the actual projects nedfolders file
			for(IContainer folder : ProjectUtils.readNedFoldersFile(project))
				result += pathSep + folder.getLocation().toOSString();

			// do the same for the referenced projects
			for (IProject p : ProjectUtils.getAllReferencedOmnetppProjects(project)) 
				for(IContainer folder : ProjectUtils.readNedFoldersFile(p))
					result += pathSep + folder.getLocation().toOSString();
		}
		catch (Exception e) {
			OmnetppMainPlugin.logError(e);
		}
		return result;
	}

    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, OmnetppMainPlugin.PLUGIN_ID, 1, message, exception));
    }
}
