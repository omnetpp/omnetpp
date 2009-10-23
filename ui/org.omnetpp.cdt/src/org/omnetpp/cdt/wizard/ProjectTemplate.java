/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.io.ByteArrayInputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.StringUtils;

import freemarker.cache.ClassTemplateLoader;
import freemarker.template.Configuration;
import freemarker.template.Template;

/**
 * The default implementation of IProjectTemplate
 * @author Andras
 */
public abstract class ProjectTemplate implements IProjectTemplate {
    private String name;
    private String description;
    private String category;
    private Image image;
    
    // fields set by and used during configure():   
    private IProject project; // the project being configured
    private IProgressMonitor monitor;
    private Map<String,Object> vars = new HashMap<String, Object>(); // variables to be substituted into generated text files

    public ProjectTemplate(String name, String category, String description, Image image) {
        super();
        this.name = name;
        this.category = category;
        this.description = description;
        this.image = image;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
    	this.name = name;
    }
    
    public String getCategory() {
    	return category;
    }
    
    public void setCategory(String category) {
    	this.category = category;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
    	this.description = description;
    }
    
    public Image getImage() {
        return image;
    }

    public void setImage(Image image) {
    	this.image = image;
    }
    
	/**
     * Valid only during configuring a project.
     */
    public IProject getProject() {
        return project;
    }
    
    /**
     * Valid only during configuring a project.
     */
    public IProgressMonitor getProgressMonitor() {
        return monitor;
    }

    public void configure(IProject project, Map<String, String> variables, IProgressMonitor monitor) throws CoreException {
        this.project = project;
        this.monitor = monitor;
        vars.clear();

        // pre-register some potentially useful template variables
        setVariable("templateName", name);
        setVariable("templateDescription", description);
        setVariable("templateCategory", category);
        setVariable("rawProjectName", project.getName());
        setVariable("ProjectName", StringUtils.capitalize(StringUtils.makeValidIdentifier(project.getName())));
        setVariable("projectname", StringUtils.lowerCase(StringUtils.makeValidIdentifier(project.getName())));
        setVariable("PROJECTNAME", StringUtils.upperCase(StringUtils.makeValidIdentifier(project.getName())));
        Calendar cal = Calendar.getInstance();
        setVariable("date", cal.get(Calendar.YEAR)+"-"+cal.get(Calendar.MONTH)+"-"+cal.get(Calendar.DAY_OF_MONTH));
        setVariable("year", ""+cal.get(Calendar.YEAR));
        setVariable("author", System.getProperty("user.name"));
        String license = LicenseUtils.getDefaultLicense();
        String licenseProperty = license.equals(LicenseUtils.NONE) ? "" : license; // @license(custom) is needed, so that wizards know when to create banners
        setVariable("license", licenseProperty); // for @license in package.ned
        setVariable("bannercomment", LicenseUtils.getBannerComment(license, "//"));
        
        // add user-defined variables (they may overwrite the defaults above)
        if (variables != null)
            for (String var : variables.keySet())
                setVariable(var, variables.get(var));
        
        // do it!
        doConfigure();

        project = null;
        monitor = null;
        vars.clear();
    }

    /**
     * Resolves references to other variables. Should be called from doConfigure() before
     * actually processing the templates.
     * 
     * Some variables contain references to other variables (e.g. 
     * "org.example.${projectname}"); substitute them.
     */
	protected void substituteNestedVariables() throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/org/omnetpp/cdt/wizard"));
        try {
        	int k = 0;
        	boolean again;
        	do {
        		again = false;
        		for (String key : vars.keySet()) {
        			Object value = vars.get(key);
        			if (value instanceof String) {
        				// substitute variables into value
        				Template template = new Template("tmp" + k++, new StringReader((String)value), cfg, "utf8");
        				StringWriter writer = new StringWriter();
        				template.process(vars, writer);
        				String newValue = writer.toString();
        				
        				// write back new value
        				if (!value.equals(newValue)) {
        					vars.put(key, newValue);
        					again = true;
        				}
        			}
        		}
        	} while (again);
        	
        } catch (Exception e) {
        	throw Activator.wrapIntoCoreException(e);
        }
	}
    
    /**
     * Configure the project.
     */
    protected abstract void doConfigure() throws CoreException;
    
    /**
     * Sets (creates or overwrites) a variable to be substituted into files 
     * created with createFile(). Pass value==null to remove an existing variable.
     */
    protected void setVariable(String name, Object value) {
        if (value == null)
            vars.remove(name);
        else 
            vars.put(name, value);
    }

    protected void createFileFromPluginResource(String projectRelativePath, String templateName) throws CoreException {
        createFileFromPluginResource(projectRelativePath, templateName, true);
    }

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, boolean suppressIfBlank) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/org/omnetpp/cdt/wizard"));
        IFile file = project.getFile(new Path(projectRelativePath));
        createFile(file, cfg, templateName, suppressIfBlank);
    }

    /**
     * Utility method for doConfigure. Copies a resource into the project,  
     * performing variable substitutions in it. 
     */
    protected void createFile(IFile file, Configuration templateCfg, String templateName, boolean suppressIfBlank) throws CoreException {
        // substitute variables
    	String content;    	
        try {
			Template template = templateCfg.getTemplate(templateName, "utf8");
			StringWriter writer = new StringWriter();
			template.process(vars, writer);
			content = writer.toString();
		} catch (Exception e) {
			throw Activator.wrapIntoCoreException(e);
		}

		// normalize line endings, remove multiple blank lines, etc
		content = content.replace("\r\n", "\n");
		content = content.replaceAll("\n\n\n+", "\n\n");
		content = content.trim() + "\n";

		if (!suppressIfBlank || !StringUtils.isBlank(content)) {
            // save it
            byte[] bytes = content.getBytes(); 
            if (!file.exists())
                file.create(new ByteArrayInputStream(bytes), true, monitor);
            else
                file.setContents(new ByteArrayInputStream(bytes), true, false, monitor);
        }
    }

    /**
     * Creates a folder. If the parent folder(s) do not exist, they are created.
     */
    protected void createFolder(String projectRelativePath) throws CoreException {
        IFolder folder = getProject().getFolder(new Path(projectRelativePath));
        if (!folder.getParent().exists())
            createFolder(folder.getParent().getProjectRelativePath().toString());
        if (!folder.exists())
            folder.create(true, true, monitor);
    }
    
    /**
     * Creates the given folders, and a folder. If the parent folder(s) do not exist, they are created.
     */
    protected void createAndSetNedSourceFolders(String[] projectRelativePaths) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = getProject().getFolder(new Path(projectRelativePaths[i]));
        ProjectUtils.saveNedFoldersFile(getProject(), folders);
    }

    /**
     * Sets C++ source folders to the given list.
     */
    protected void createAndSetSourceFolders(String[] projectRelativePaths) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = getProject().getFolder(new Path(projectRelativePaths[i]));
        setSourceLocations(project, folders);
    }
    
    /**
     * Sets the project's source locations list to the given list of folders, in all configurations. 
     * (Previous source entries get overwritten.)  
     */
    protected static void setSourceLocations(IProject project, IContainer[] folders) throws CoreException {
        if (System.getProperty("org.omnetpp.test.unit.running") != null)
            return; // in the test case we don't create a full CDT project, so the code below would throw NPE

        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project, true);
        int n = folders.length;
        for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
            ICSourceEntry[] entries = new CSourceEntry[n];
            for (int i=0; i<n; i++) {
                Assert.isTrue(folders[i].getProject().equals(project));
                entries[i] = new CSourceEntry(folders[i].getProjectRelativePath(), new IPath[0], 0);
            }
            try {
                configuration.setSourceEntries(entries);
            }
            catch (WriteAccessException e) {
                Activator.logError(e); // should not happen, as we called getProjectDescription() with write=true
            }
        }
        CoreModel.getDefault().setProjectDescription(project, projectDescription);
    }

    /**
     * Creates a default build spec (project root being makemake folder)
     */
    protected void createDefaultBuildSpec() throws CoreException {
        BuildSpecification.createInitial(getProject()).save();
    }

    /**
     * Sets the makemake options on the given project. Array must contain folderPath1, options1, 
     * folderPath2, options2, etc.
     */
    protected void createBuildSpec(String[] pathsAndMakemakeOptions) throws CoreException {
        Assert.isTrue(pathsAndMakemakeOptions.length%2 == 0);
        IProject project = getProject();
        BuildSpecification buildSpec = BuildSpecification.createBlank(project);
        for (int i=0; i<pathsAndMakemakeOptions.length; i+=2) {
            String folderPath = pathsAndMakemakeOptions[i];
            String args = pathsAndMakemakeOptions[i+1];
            
            IContainer folder = folderPath.equals(".") ? project : project.getFolder(new Path(folderPath));
            buildSpec.setFolderMakeType(folder, BuildSpecification.MAKEMAKE);
            MakemakeOptions options = new MakemakeOptions(args);
            buildSpec.setMakemakeOptions(folder, options);
        }
        buildSpec.save();
    }

}
