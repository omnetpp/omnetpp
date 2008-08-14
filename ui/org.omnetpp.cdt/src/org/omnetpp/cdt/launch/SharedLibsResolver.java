package org.omnetpp.cdt.launch;

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
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;

/**
 * resolves the variable to the library command line like: -l lib1 -l lib2
 * note that the -l option is included before each library
 */
public class SharedLibsResolver implements IDynamicVariableResolver {

	public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
		if (argument == null)
			abort("${opp_shared_libs:arg} requires an argument", null);

		IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
		if (resource == null)
			abort("argument to ${opp_shared_libs:arg} needs to be an existing file, folder, or project", null);

		IProject project = resource.getProject();
		// resolve the ned path files
		String result = "";
		try {
			result = getExportedSharedLib(project)+" ";
			
			// do the same for the referenced projects
			for (IProject p : ProjectUtils.getAllReferencedOmnetppProjects(project)) 
				result += getExportedSharedLib(p);
		}
		catch (Exception e) {
			Activator.logError(e);
		}
		return result;
	}

	protected String getExportedSharedLib(IProject project) {
		try {
			String libraries = "";
			BuildSpecification spec = BuildSpecUtils.readBuildSpecFile(project);
			if (spec == null)
				return "";
			for(IContainer container: spec.getMakemakeFolders()) {
				MakemakeOptions options = spec.getMakemakeOptions(container);
				if (options == null || options.type != MakemakeOptions.Type.SHAREDLIB) 
					continue;
				
				String target = options.target != null ? options.target : project.getName();
				String lib = container.getLocation().append(target).toString();
				libraries += " " + lib;
			}
			return libraries;
		} catch (CoreException e) {
			Activator.logError(e);
		}
		return "";
	}
	
    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 1, message, exception));
    }
}
