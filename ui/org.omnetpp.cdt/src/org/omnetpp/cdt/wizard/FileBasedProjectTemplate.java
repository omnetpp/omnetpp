package org.omnetpp.cdt.wizard;

import java.net.URL;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Plugin;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.FileBasedContentTemplate;
import org.omnetpp.common.wizard.SWTDataUtil;

/**
 * Content template specialized for OMNeT++ projects
 * 
 * @author Andras
 */
public class FileBasedProjectTemplate extends FileBasedContentTemplate {
    public static final String PROP_ADDPROJECTREFERENCE = "addProjectReference"; // boolean: if true, make project as dependent on this one
    public static final String PROP_SOURCEFOLDERS = "sourceFolders"; // source folders to be created and configured
    public static final String PROP_NEDSOURCEFOLDERS = "nedSourceFolders"; // NED source folders to be created and configured
    public static final String PROP_MAKEMAKEOPTIONS = "makemakeOptions"; // makemake options, as "folder1:options1,folder2:options2,..."
    

    public FileBasedProjectTemplate(IFolder folder) throws CoreException {
        super(folder);
    }

    public FileBasedProjectTemplate(Plugin plugin, String templateFolder) throws CoreException {
        super(plugin, templateFolder);
    }

    public FileBasedProjectTemplate(URL templateUrl) throws CoreException {
        super(templateUrl);
    }

    @Override
    public CreationContext createContext(IContainer folder) {
        CreationContext context = super.createContext(folder);

        // default values of recognized options (will be overwritten from property file)
        context.getVariables().put(PROP_ADDPROJECTREFERENCE, "true");
        context.getVariables().put(PROP_SOURCEFOLDERS, "");
        context.getVariables().put(PROP_NEDSOURCEFOLDERS, "");
        context.getVariables().put(PROP_MAKEMAKEOPTIONS, "");

        return context;
    }

    //FIXME TODO override: protected ClassLoader createClassLoader() {
    //FIXME register newer IDEUtils
    
	
	public void performFinish(CreationContext context) throws CoreException {
	    super.performFinish(context);

	    if (getTemplateFolder()!=null && SWTDataUtil.toBoolean(context.getVariables().get(PROP_ADDPROJECTREFERENCE)))
			ProjectUtils.addReferencedProject((IProject)context.getFolder(), getTemplateFolder().getProject(), context.getProgressMonitor());
		
		String[] srcFolders = SWTDataUtil.toStringArray(context.getVariables().get(PROP_SOURCEFOLDERS), " *, *");
		if (srcFolders.length > 0)
			createAndSetSourceFolders(srcFolders, context);

		String[] nedSrcFolders = SWTDataUtil.toStringArray(context.getVariables().get(PROP_NEDSOURCEFOLDERS), " *, *");
		if (nedSrcFolders.length > 0)
			createAndSetNedSourceFolders(nedSrcFolders, context);

		Map<String,String> makemakeOptions = SWTDataUtil.toStringMap(context.getVariables().get(PROP_MAKEMAKEOPTIONS));
		if (!makemakeOptions.isEmpty())
			createBuildSpec(makemakeOptions, context);
	}
	
    protected void createFolder(String projectRelativePath, CreationContext context) throws CoreException {
        ProjectWizardUtils.createFolder(projectRelativePath, context);
    }
    
    /**
     * Creates the given folders, and a folder. If the parent folder(s) do not exist, they are created.
     */
    protected void createAndSetNedSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        ProjectWizardUtils.createAndSetNedSourceFolders(projectRelativePaths, context);
    }

    /**
     * Sets C++ source folders to the given list.
     */
    protected void createAndSetSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        ProjectWizardUtils.createAndSetSourceFolders(projectRelativePaths, context);
    }
    
    /**
     * Sets the project's source locations list to the given list of folders, in all configurations. 
     * (Previous source entries get overwritten.)  
     */
    protected void setSourceLocations(IContainer[] folders, CreationContext context) throws CoreException {
        ProjectWizardUtils.setSourceLocations(folders, context);
    }

    /**
     * Creates a default build spec (project root being makemake folder)
     */
    protected void createDefaultBuildSpec(CreationContext context) throws CoreException {
        BuildSpecification.createInitial((IProject)context.getFolder()).save();
    }

    /**
     * Sets the makemake options on the given project. Array must contain folderPaths 
     * as keys, and options as values.
     */
        
    protected void createBuildSpec(String[] pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        ProjectWizardUtils.createBuildSpec(pathsAndMakemakeOptions, context);
    }
    
    /**
     * Sets the makemake options on the given project. Array must contain folderPath1, options1, 
     * folderPath2, options2, etc.
     */
    protected void createBuildSpec(Map<String,String> pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        ProjectWizardUtils.createBuildSpec(pathsAndMakemakeOptions, context);
    }
}
