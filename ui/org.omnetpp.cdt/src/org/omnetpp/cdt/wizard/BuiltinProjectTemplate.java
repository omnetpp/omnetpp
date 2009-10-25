package org.omnetpp.cdt.wizard;

import java.util.HashMap;
import java.util.Map;

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

	public IWizardPage[] createCustomPages() {
		return new IWizardPage[0];
	}

	public Map<String, Object> extractVariablesFromPages(IWizardPage[] customPages) {
		return new HashMap<String, Object>();
	}

    protected void createFileFromPluginResource(String projectRelativePath, String templateName) throws CoreException {
        createFileFromPluginResource(projectRelativePath, templateName, true);
    }

    protected void createFileFromPluginResource(String projectRelativePath, String templateName, boolean suppressIfBlank) throws CoreException {
        Configuration cfg = new Configuration();
        cfg.setTemplateLoader(new ClassTemplateLoader(getClass(), "/org/omnetpp/cdt/wizard"));
        IFile file = getProject().getFile(new Path(projectRelativePath));
        createFile(file, cfg, templateName, suppressIfBlank);
    }

}
