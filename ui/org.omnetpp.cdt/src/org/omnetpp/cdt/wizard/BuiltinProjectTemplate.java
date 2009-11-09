package org.omnetpp.cdt.wizard;

import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.wizard.ContentTemplate;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.ICustomWizardPage;

import freemarker.cache.ClassTemplateLoader;
import freemarker.template.Configuration;

/**
 * Base class for templates defined within this plug-in.
 * Subclasses must provide a doConfigure() method.
 * 
 * @author Andras
 */
public abstract class BuiltinProjectTemplate extends ContentTemplate {

	public BuiltinProjectTemplate(String name, String category, String description) {
		super(name, category, description);
	}
	
	public BuiltinProjectTemplate(String name, String category, String description, Image image) {
		super(name, category, description, image);
	}

	public ICustomWizardPage[] createCustomPages() {
		return new ICustomWizardPage[0];
	}

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, CreationContext context) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/template"));
        IFile file = context.getFolder().getFile(new Path(projectRelativePath));
        createTemplateFile(file, cfg, templateName, context);
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
