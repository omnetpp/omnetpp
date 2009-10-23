package org.omnetpp.cdt.wizard;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.graphics.Image;

import freemarker.cache.ClassTemplateLoader;
import freemarker.template.Configuration;

public abstract class BuiltinProjectTemplate extends ProjectTemplate {

	public BuiltinProjectTemplate(String name, String category, String description, Image image) {
		super(name, category, description, image);
	}

	@Override
	public IWizardPage[] createCustomPages() {
		return new IWizardPage[0];
	}

	@Override
	protected void extractVariablesFromPages(IWizardPage[] customPages) {
		// nothing
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
