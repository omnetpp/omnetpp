package org.omnetpp.cdt.wizard;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

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

    public String getDescription() {
        return description;
    }

    public Image getImage() {
        return image;
    }

    public String getCategory() {
        return category;
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

    public void configure(IProject project, IProgressMonitor monitor) throws CoreException {
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
        setVariable("copyright", readResource("templates/defaultCopyright.txt")); // some default; may be overwritten
        
        // do it!
        doConfigure();
        
        project = null;
        monitor = null;
        vars.clear();
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

    /**
     * Reads a resource to a string.
     */
    protected String readResource(String resourcePath) throws CoreException {
        try {
            return FileUtils.readTextFile(getClass().getResourceAsStream(resourcePath));
        } 
        catch (IOException e) {
            throw Activator.wrapIntoCoreException(e);
        }
    }

    /**
     * Utility method for doConfigure. Copies a resource into the project, 
     * performing variable substitutions in it.
     */
    protected void createFileFromResource(String projectRelativePath, String resourcePath) throws CoreException {
        String content = readResource(resourcePath);
        content = content.replace("\r\n", "\n");
        createFile(projectRelativePath, content);
    }

    /**
     * Utility method for doConfigure. Saves the given text into the project as a file, 
     * performing variable substitutions in it.
     */
    protected void createFile(String projectRelativePath, String content) throws CoreException {
        // substitute variables (recursively)
        String oldContent = "";
        while (!content.equals(oldContent)) {
            oldContent = content;
            content = StringUtils.substituteIntoTemplate(content, vars, "{{", "}}");
        }
        
        // save it
        byte[] bytes = content.getBytes(); 
        IFile file = project.getFile(new Path(projectRelativePath));
        if (!file.exists())
            file.create(new ByteArrayInputStream(bytes), true, monitor);
        else
            file.setContents(new ByteArrayInputStream(bytes), true, false, monitor);
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
        CDTUtils.setSourceLocations(project, folders);
    }
}
