package org.omnetpp.cdt.wizard;

import org.eclipse.core.resources.IProject;
import org.eclipse.swt.graphics.Image;

/**
 * The default implementation of IProjectTemplate
 * @author Andras
 */
public class ProjectTemplate implements IProjectTemplate {
    private String name;
    private String description;
    private Image image;
    
    public ProjectTemplate(String name, String description, Image image) {
        super();
        this.name = name;
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

    public void configure(IProject project) {
        System.out.println("Applying template '"+name+"' to project "+project.getName());
        // TODO Auto-generated method stub
    }
}
