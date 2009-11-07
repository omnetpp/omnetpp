package org.omnetpp.common.wizard;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.graphics.Image;

import freemarker.cache.ClassTemplateLoader;
import freemarker.cache.MultiTemplateLoader;
import freemarker.cache.TemplateLoader;
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

	public ICustomWizardPage[] createCustomPages() throws CoreException {
		return new ICustomWizardPage[0];
	}

	@Override
	protected Configuration createFreemarkerConfiguration() {
	    // add this plugin's "template" dir to the template loader
	    Configuration cfg = super.createFreemarkerConfiguration();
	    cfg.setTemplateLoader(new MultiTemplateLoader( new TemplateLoader[] { 
	            cfg.getTemplateLoader(), 
	            new ClassTemplateLoader(getClass(), "/template")  
	    }));
        return cfg;
	}
	
    protected void createFileFromPluginResource(String projectRelativePath, String templateName, CreationContext context) throws CoreException {
        IFile file = context.getFolder().getFile(new Path(projectRelativePath));
        createFile(file, getFreemarkerConfiguration(), templateName, context);
    }

}
