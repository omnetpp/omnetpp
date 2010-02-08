package org.omnetpp.ned.editor.wizards;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Path;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedNewFileWizard;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * "New NED file" wizard
 *
 * @author Andras
 */
public abstract class AbstractNedFileWizard extends TemplateBasedNewFileWizard {
    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New NED File");

        WizardNewFileCreationPage firstPage = getFirstPage();
        firstPage.setTitle(isImporting() ? "Import NED File" : "New NED File");
        firstPage.setDescription(isImporting() ? "Choose import target NED file" : "Choose NED file");
        firstPage.setFileExtension("ned");
        firstPage.setFileName("untitled.ned");
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        IFile newFile = folder.getFile(new Path(getFirstPage().getFileName()));
        String packageName = NedResourcesPlugin.getNedResources().getExpectedPackageFor(newFile);
        context.getVariables().put("nedPackageName", StringUtils.defaultString(packageName,""));

        // namespace
        String namespaceName = NedResourcesPlugin.getNedResources().getSimplePropertyFor(folder, INedTypeResolver.NAMESPACE_PROPERTY);
        context.getVariables().put("namespaceName", StringUtils.defaultString(namespaceName,""));

        return context;
    }

}
