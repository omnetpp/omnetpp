package org.omnetpp.cdt.wizard;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.cdt.Activator;

/**
 * Stores project templates
 * @author Andras
 */
public class ProjectTemplateStore {
    private List<IProjectTemplate> cppTemplates = new ArrayList<IProjectTemplate>();
    private List<IProjectTemplate> noncppTemplates = new ArrayList<IProjectTemplate>();

    public static final Image ICON_TEMPLATE = Activator.getCachedImage("icons/full/obj16/template.png");
    
    public ProjectTemplateStore() {
        cppTemplates.add(new ProjectTemplate("alma", null, ICON_TEMPLATE));
        cppTemplates.add(new ProjectTemplate("korte", null, ICON_TEMPLATE));
        cppTemplates.add(new ProjectTemplate("barack", null, ICON_TEMPLATE));
    }
    
    public List<IProjectTemplate> getCppTemplates() {
        return cppTemplates;
    }

    public List<IProjectTemplate> getNoncppTemplates() {
        return noncppTemplates;
    }
}
