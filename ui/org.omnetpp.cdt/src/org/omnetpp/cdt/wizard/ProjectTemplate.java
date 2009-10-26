/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Calendar;
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
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.StringUtils;

import freemarker.cache.StringTemplateLoader;
import freemarker.ext.beans.BeansWrapper;
import freemarker.template.Configuration;
import freemarker.template.Template;
import freemarker.template.TemplateException;

/**
 * The default implementation of IProjectTemplate
 * @author Andras
 */
public abstract class ProjectTemplate implements IProjectTemplate {
    public static final Image DEFAULT_ICON = Activator.getCachedImage("icons/full/obj16/template.png");

    private String name;
    private String description;
    private String category;
    private Image image = DEFAULT_ICON;
    
    public ProjectTemplate() {
    }

    public ProjectTemplate(String name, String category, String description) {
        super();
        this.name = name;
        this.category = category;
        this.description = description;
    }

    public ProjectTemplate(String name, String category, String description, Image image) {
    	this(name, category, description);
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
    
    public CreationContext createContext(IProject project) {
    	CreationContext context = new CreationContext(project);
    	
    	// pre-register some potentially useful template variables
    	Map<String, Object> variables = context.getVariables();
        variables.put("templateName", name);
        variables.put("templateDescription", description);
        variables.put("templateCategory", category);
		variables.put("rawProjectName", project);
		variables.put("ProjectName", StringUtils.capitalize(StringUtils.makeValidIdentifier(project.getName())));
		variables.put("projectname", StringUtils.lowerCase(StringUtils.makeValidIdentifier(project.getName())));
		variables.put("PROJECTNAME", StringUtils.upperCase(StringUtils.makeValidIdentifier(project.getName())));
        Calendar cal = Calendar.getInstance();
        variables.put("date", cal.get(Calendar.YEAR)+"-"+cal.get(Calendar.MONTH)+"-"+cal.get(Calendar.DAY_OF_MONTH));
        variables.put("year", ""+cal.get(Calendar.YEAR));
        variables.put("author", System.getProperty("user.name"));
        String licenseCode = LicenseUtils.getDefaultLicense();
        String licenseProperty = licenseCode.equals(LicenseUtils.NONE) ? "" : licenseCode; // @license(custom) is needed, so that wizards knows whether to create banners
        variables.put("licenseCode", licenseProperty); // for @license in package.ned
        variables.put("licenseText", LicenseUtils.getLicenseNotice(licenseCode));
        variables.put("bannerComment", LicenseUtils.getBannerComment(licenseCode, "//"));

        return context;
    }
    
    public void configureProject(CreationContext context) throws CoreException {
    	// The following is needed to make classes from this plugin and dependencies 
    	// (e.g. ned.core) available to templates via BeanWrapper. If not done,
    	// templates only have access to classes from org.omnetpp.core and its
    	// dependencies, because freemarker.jar is loaded from that plugin.
    	// See freemarker.template.utility.ClassUtil.forName(String)
    	Thread.currentThread().setContextClassLoader(getClass().getClassLoader());
    	
        // do it!
        doConfigure(context);
    }

    /**
     * Configure the project.
     */
    protected abstract void doConfigure(CreationContext context) throws CoreException;

	/**
     * Resolves references to other variables. Should be called from doConfigure() before
     * actually processing the templates.
     * 
     * Some variables contain references to other variables (e.g. 
     * "org.example.${projectname}"); substitute them.
     */
	protected void substituteNestedVariables(CreationContext context) throws CoreException {
		Map<String, Object> variables = context.getVariables();
        Configuration cfg = new Configuration();
        cfg.setNumberFormat("computer"); // prevent digit grouping with comma
        cfg.setTemplateLoader(new StringTemplateLoader()); // effectively, no template loading

        try {
        	for (String key : variables.keySet()) {
        		Object value = variables.get(key);
        		if (value instanceof String) {
        			String newValue = doEvaluate((String)value, variables, cfg);
        			variables.put(key, newValue);
        		}
        	}
        } catch (Exception e) {
        	throw Activator.wrapIntoCoreException(e);
        }
	}

	/** 
	 * Performs template processing on the given string, and returns the result.
	 */
	protected String evaluate(String text, CreationContext context) throws TemplateException {
		Map<String, Object> variables = context.getVariables();
        Configuration cfg = new Configuration();
        cfg.setNumberFormat("computer"); // prevent digit grouping with comma
        cfg.setTemplateLoader(new StringTemplateLoader()); // effectively, no template loading
        return doEvaluate(text, variables, cfg);
	}

	private String doEvaluate(String value, Map<String, Object> variables, Configuration cfg) throws TemplateException {
		try {
			int k = 0;
			while (true) {
				Template template = new Template("text" + k++, new StringReader(value), cfg, "utf8");
				StringWriter writer = new StringWriter();
				template.process(variables, writer);
				String newValue = writer.toString();
				if (value.equals(newValue))
					return value;
				value = newValue;
			}
		} 
		catch (IOException e) {
			Activator.logError(e); 
			return ""; // cannot happen, we work with string reader/writer only
		}
		
	}

    /**
     * Utility method for doConfigure. Copies a resource into the project,  
     * performing variable substitutions in it. 
     */
    protected void createFile(IFile file, Configuration templateCfg, String templateName, boolean suppressIfBlank, CreationContext context) throws CoreException {
        // substitute variables
    	String content;    	
        try {
        	// make Math and static methods of other classes available to the template
        	// see chapter "Bean wrapper" in the FreeMarker manual 
        	context.getVariables().put("Math", BeansWrapper.getDefaultInstance().getStaticModels().get(Math.class.getName()));
        	context.getVariables().put("classes", BeansWrapper.getDefaultInstance().getStaticModels());
            templateCfg.setNumberFormat("computer"); // prevent digit grouping with comma
        	
        	// perform template substitution
			Template template = templateCfg.getTemplate(templateName, "utf8");
			StringWriter writer = new StringWriter();
			template.process(context.getVariables(), writer);
			content = writer.toString();
		} catch (Exception e) {
			throw Activator.wrapIntoCoreException(e);
		}

		// normalize line endings, remove multiple blank lines, etc
		content = content.replace("\r\n", "\n");
		content = content.replaceAll("\n\n\n+", "\n\n");
		content = content.trim() + "\n";

		// save file (unless it's blank)
		if (!suppressIfBlank || !StringUtils.isBlank(content)) {
            byte[] bytes = content.getBytes(); 
            if (!file.exists())
                file.create(new ByteArrayInputStream(bytes), true, context.getProgressMonitor());
            else
                file.setContents(new ByteArrayInputStream(bytes), true, false, context.getProgressMonitor());
        }
    }

    /**
     * Creates a folder. If the parent folder(s) do not exist, they are created.
     */
    protected void createFolder(String projectRelativePath, CreationContext context) throws CoreException {
        IFolder folder = context.getProject().getFolder(new Path(projectRelativePath));
        if (!folder.getParent().exists())
            createFolder(folder.getParent().getProjectRelativePath().toString(), context);
        if (!folder.exists())
            folder.create(true, true, context.getProgressMonitor());
    }
    
    /**
     * Creates the given folders, and a folder. If the parent folder(s) do not exist, they are created.
     */
    protected void createAndSetNedSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path, context);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = context.getProject().getFolder(new Path(projectRelativePaths[i]));
        ProjectUtils.saveNedFoldersFile(context.getProject(), folders);
    }

    /**
     * Sets C++ source folders to the given list.
     */
    protected void createAndSetSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path, context);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = context.getProject().getFolder(new Path(projectRelativePaths[i]));
        setSourceLocations(folders, context);
    }
    
    /**
     * Sets the project's source locations list to the given list of folders, in all configurations. 
     * (Previous source entries get overwritten.)  
     */
    protected static void setSourceLocations(IContainer[] folders, CreationContext context) throws CoreException {
        if (System.getProperty("org.omnetpp.test.unit.running") != null)
            return; // in the test case we don't create a full CDT project, so the code below would throw NPE

        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(context.getProject(), true);
        int n = folders.length;
        for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
            ICSourceEntry[] entries = new CSourceEntry[n];
            for (int i=0; i<n; i++) {
                Assert.isTrue(folders[i].getProject().equals(context));
                entries[i] = new CSourceEntry(folders[i].getProjectRelativePath(), new IPath[0], 0);
            }
            try {
                configuration.setSourceEntries(entries);
            }
            catch (WriteAccessException e) {
                Activator.logError(e); // should not happen, as we called getProjectDescription() with write=true
            }
        }
        CoreModel.getDefault().setProjectDescription(context.getProject(), projectDescription);
    }

    /**
     * Creates a default build spec (project root being makemake folder)
     */
    protected void createDefaultBuildSpec(CreationContext context) throws CoreException {
        BuildSpecification.createInitial(context.getProject()).save();
    }

    /**
     * Sets the makemake options on the given project. Array must contain folderPath1, options1, 
     * folderPath2, options2, etc.
     */
    protected void createBuildSpec(String[] pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        Assert.isTrue(pathsAndMakemakeOptions.length%2 == 0);
        IProject project = context.getProject();
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
