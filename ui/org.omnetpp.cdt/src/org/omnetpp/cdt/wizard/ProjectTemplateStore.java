package org.omnetpp.cdt.wizard;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
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
        createTemplates();
    }

    public List<IProjectTemplate> getCppTemplates() {
        return cppTemplates;
    }
    
    public List<IProjectTemplate> getNoncppTemplates() {
        return noncppTemplates;
    }

    protected void createTemplates() {
        // non-C++ projects
        final String SINGLE_DIR0 = "Single-directory project";
        noncppTemplates.add(new ProjectTemplate("Empty project", SINGLE_DIR0, null, ICON_TEMPLATE));
        
        // C++ projects
        final String SINGLE_DIR = "Single-directory project, for small simulations";
        cppTemplates.add(new ProjectTemplate("Empty project", SINGLE_DIR, null, ICON_TEMPLATE));
        cppTemplates.add(new ProjectTemplate("Tictoc example", SINGLE_DIR, null, ICON_TEMPLATE) {
            @Override
            public void doConfigure() throws CoreException {
                createFileFromResource("Source.cc", "templates/Source.cc");
                createFileFromResource("Sink.cc", "templates/Sink.cc");
            }
        }) ;

        final String SRC_AND_SMULATIONS = "Project with \"src\" and \"simulations\" folders";
        cppTemplates.add(new ProjectTemplate("Empty project", SRC_AND_SMULATIONS, null, ICON_TEMPLATE));
        cppTemplates.add(new ProjectTemplate("Tictoc example", SRC_AND_SMULATIONS, null, ICON_TEMPLATE));

        final String SRC_AND_EXAMPLES = "Project with \"src\" and \"examples\" folders";
        cppTemplates.add(new ProjectTemplate("Empty project", SRC_AND_EXAMPLES, null, ICON_TEMPLATE));
        cppTemplates.add(new ProjectTemplate("Tictoc example", SRC_AND_EXAMPLES, null, ICON_TEMPLATE));
    }
    
}
