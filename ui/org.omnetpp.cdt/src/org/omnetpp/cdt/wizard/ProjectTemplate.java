package org.omnetpp.cdt.wizard;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.util.FileUtils;

/**
 * The default implementation of IProjectTemplate
 * @author Andras
 */
public class ProjectTemplate implements IProjectTemplate {
    private String name;
    private String description;
    private String category;
    private Image image;
    
    // fields set by and used during configure():   
    private IProject project; // the project being configured
    private IProgressMonitor monitor;
    private Map<String,String> vars = new HashMap<String, String>(); // variables to be substituted into generated text files

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

    public void configure(IProject project, IProgressMonitor monitor) throws CoreException {
        this.project = project;
        this.monitor = monitor;
        vars.clear();

        doConfigure();
        
        project = null;
        monitor = null;
        vars.clear();
    }

    protected void doConfigure() throws CoreException {}  //FIXME make abstract

    protected boolean isCanceled() {
        return monitor == null ? false : monitor.isCanceled();
    }

    /**
     * Sets (creates or overwrites) a variable to be substituted into files 
     * created with createFile(). Pass value==null to remove an existing variable.
     */
    protected void setVariable(String name, String value) {
        if (value == null)
            vars.remove(name);
        else 
            vars.put(name, value);
    }

    /**
     * Utility method for doConfigure. Copies a resource into the project, 
     * performing variable substitutions in it.
     */
    protected void createFileFromResource(String projectRelativePath, String resourcePath) throws CoreException {
        try {
            String content = FileUtils.readTextFile(getClass().getResourceAsStream(resourcePath));
            content = content.replace("\r\n", "\n");
            createFile(projectRelativePath, content);
        } 
        catch (IOException e) {
            throw Activator.wrapIntoCoreException(e);
        }
    }

    /**
     * Utility method for doConfigure. Saves the given text into the project as a file, 
     * performing variable substitutions in it.
     */
    protected void createFile(String projectRelativePath, String content) throws CoreException {
        // substitute variables
        for (String varName : vars.keySet())
            content = content.replace("@"+varName+"@", vars.get(varName));
        
        // save it
        byte[] bytes = content.getBytes(); 
        IFile file = project.getFile(new Path(projectRelativePath));
        if (!file.exists())
            file.create(new ByteArrayInputStream(bytes), true, monitor);
        else
            file.setContents(new ByteArrayInputStream(bytes), true, false, monitor);
    }


}
