package org.omnetpp.common.wizard;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;

import freemarker.cache.ClassTemplateLoader;
import freemarker.template.Configuration;

/**
 * Base class for templates defined within this plug-in.
 * Subclasses must provide a doPerformFinish() method.
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
        createFileFromPluginResource(projectRelativePath, templateName, true, context);
    }

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, boolean suppressIfBlank, CreationContext context) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/template"));
        IFile file = context.getFolder().getFile(new Path(projectRelativePath));
        createFile(file, cfg, templateName, suppressIfBlank, context);
    }

}
