package org.omnetpp.msg.editor.wizard;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedNewFileWizard;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * "New Msg File" wizard
 *
 * @author Andras
 */
public class NewMsgFileWizard extends TemplateBasedNewFileWizard {

    public NewMsgFileWizard() {
        setWizardType("msgfile");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle(isImporting() ? "Import Message File" : "New Message File");

        WizardNewFileCreationPage firstPage = getFirstPage();

        firstPage.setTitle(isImporting() ? "Import Message File" : "New Message File");
        firstPage.setDescription("This wizard allows you to " + (isImporting() ? "import" : "create") + " a new OMNeT++ message definition file");
        firstPage.setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newmsgfile_wiz.png"));

        firstPage.setFileExtension("msg");
        firstPage.setFileName("untitled.msg");
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        // namespace
        String namespaceName = NedResourcesPlugin.getNedResources().getSimplePropertyFor(folder, INedTypeResolver.NAMESPACE_PROPERTY);
        context.getVariables().put("namespaceName", StringUtils.defaultString(namespaceName,""));

        return context;
    }
}
