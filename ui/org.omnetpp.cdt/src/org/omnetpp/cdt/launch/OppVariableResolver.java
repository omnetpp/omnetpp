package org.omnetpp.cdt.launch;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Resolves the variable containing all executables/shared libs/static libs built 
 * by the given project and the project it depends on. Libs and executables are
 * returned WITHOUT possible prefixes ("lib") and suffixes (".exe", ".so", 
 * ".dll", ".a", ".lib" etc.)
 * 
 * @author rhornig, andras
 */
public class OppVariableResolver implements IDynamicVariableResolver {
	public static final String OPP_STATIC_LIBS = "opp_static_libs";
	public static final String OPP_SHARED_LIBS = "opp_shared_libs";
	public static final String OPP_LD_LIBRARY_PATH = "opp_ld_library_path";
	public static final String OPP_SIMPROGS = "opp_simprogs";
	public static final String LOC_SUFFIX = "_loc";

    public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
	    String varName = variable.getName();
	    
		if (argument == null)
			abort("${" + varName +"} requires an argument", null);

		IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
		if (resource == null)
			abort("argument to ${" + varName + "} needs to be an existing file, folder, or project", null);

		IProject project = resource.getProject();

		String result = "";
		try {
			result = resolveForProject(project, varName);
			
			// do the same for the referenced projects
			for (IProject p : ProjectUtils.getAllReferencedOmnetppProjects(project)) 
				result += resolveForProject(p, varName);
		}
		catch (Exception e) {
			Activator.logError(e);
		}
		return result;
	}

	protected String resolveForProject(IProject project, String varName) {
		try {
			String result = "";
			boolean isLocation = varName.endsWith(LOC_SUFFIX);
			varName = StringUtils.removeEnd(varName, LOC_SUFFIX);
			
			BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project);
			if (buildSpec == null)
				return ""; // no build spec file
			
	        IContainer[] sourceFolders = buildSpec.getMakemakeFolders();
			for (IContainer folder : sourceFolders) {
				MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
				Assert.isTrue(options!=null);
				
				if (varName.equals(OPP_SIMPROGS)) {
				    if (options.type == MakemakeOptions.Type.EXE) { 
				        String target = options.target != null ? options.target : project.getName();
				        IFile file = folder.getFile(new Path(target));
				        String targetPath = (isLocation ? file.getLocation() : file.getFullPath()).toString();
				        result += " " + targetPath;
				    }
				}
				else if (varName.equals(OPP_SHARED_LIBS)) {
                    if (options.type == MakemakeOptions.Type.SHAREDLIB) { 
                        String target = options.target != null ? options.target : project.getName();
				        IFile file = folder.getFile(new Path(target));
				        String targetPath = (isLocation ? file.getLocation() : file.getFullPath()).toString();
                        result += " " + targetPath;
                    }
                }
                else if (varName.equals(OPP_STATIC_LIBS)) {
                    if (options.type == MakemakeOptions.Type.STATICLIB) { 
                        String target = options.target != null ? options.target : project.getName();
				        IFile file = folder.getFile(new Path(target));
				        String targetPath = (isLocation ? file.getLocation() : file.getFullPath()).toString();
                        result += " " + targetPath;
                    }
                }
                else if (varName.equals(OPP_LD_LIBRARY_PATH)) {
                    if (options.type == MakemakeOptions.Type.SHAREDLIB) { 
				        String targetPath = (isLocation ? folder.getLocation() : folder.getFullPath()).toString();
                        result += System.getProperty("path.separator") + targetPath;
                    }
                }
                else {
                    abort("internal error: ${" + varName +"}: unexpected macro name by processing class", null);
                }
			}
			return result;
		} 
		catch (CoreException e) {
			Activator.logError(e);
			return "";
		}
	}
	
    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 1, message, exception));
    }
}
