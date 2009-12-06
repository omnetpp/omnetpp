package org.omnetpp.msg.editor.wizard;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedNewFileWizard;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

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
        setWindowTitle("New Msg File");

        WizardNewFileCreationPage firstPage = getFirstPage();

        firstPage.setTitle("Create a msg file");
        firstPage.setDescription("This wizard allows you to create a new OMNeT++ message definition file");
        firstPage.setImageDescriptor(ImageDescriptor.createFromFile(getClass(),"/icons/newmsgfile_wiz.png"));

        firstPage.setFileExtension("msg");
        firstPage.setFileName("untitled.msg");
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        // msgname: make a valid identifier
        String name = getFirstPage().getFileName();
        name = name.substring(0, name.lastIndexOf('.'));
        name = StringUtils.capitalize(StringUtils.makeValidIdentifier(name));
        context.getVariables().put("msgTypeName", name);

        // namespace
        String namespaceName = NEDResourcesPlugin.getNEDResources().getSimplePropertyFor(folder, INEDTypeResolver.NAMESPACE_PROPERTY);
        context.getVariables().put("namespaceName", StringUtils.defaultString(namespaceName,""));

        return context;
    }
}
