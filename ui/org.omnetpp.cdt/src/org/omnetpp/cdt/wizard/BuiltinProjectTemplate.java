package org.omnetpp.cdt.wizard;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.graphics.Image;

import freemarker.cache.ClassTemplateLoader;
import freemarker.template.Configuration;

/**
 * Base class for templates defined within this plug-in.
 * Subclasses must provide a doConfigure() method.
 * 
 * @author Andras
 */
public abstract class BuiltinProjectTemplate extends ProjectTemplate {

	public BuiltinProjectTemplate(String name, String category, String description) {
		super(name, category, description);
	}
	
	public BuiltinProjectTemplate(String name, String category, String description, Image image) {
		super(name, category, description, image);
	}

	public IWizardPage[] createCustomPages(CreationContext context) {
		return new IWizardPage[0];
	}

	public void updateVariablesFromCustomPages(IWizardPage[] customPages, CreationContext context) {
		// nothing
	}

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, CreationContext context) throws CoreException {
        createFileFromPluginResource(projectRelativePath, templateName, true, context);
    }

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, boolean suppressIfBlank, CreationContext context) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/org/omnetpp/cdt/wizard"));
        IFile file = context.project.getFile(new Path(projectRelativePath));
        createFile(file, cfg, templateName, suppressIfBlank, context);
    }

}
