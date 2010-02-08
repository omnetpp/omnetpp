package org.omnetpp.inifile.editor.wizards;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedNewFileWizard;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.ned.core.NedResourcesPlugin;

/**
 * "New Inifile" wizard
 *
 * @author Andras
 */
public class NewInifileWizard extends TemplateBasedNewFileWizard {

    public NewInifileWizard() {
        setWizardType("inifile");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle(isImporting() ? "Import Ini File" : "New Ini File");

        WizardNewFileCreationPage firstPage = getFirstPage();
        firstPage.setTitle(isImporting() ? "Import Ini File" : "New Ini File");
        firstPage.setDescription("This wizard allows you to " + (isImporting() ? "import" : "create") + " a new OMNeT++ simulation configuration file");
        firstPage.setImageDescriptor(InifileEditorPlugin.getImageDescriptor("icons/full/wizban/newinifile.png"));
        firstPage.setFileExtension("ini");
        firstPage.setFileName("omnetpp.ini");
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);

        IPath filePath = getFirstPage().getContainerFullPath().append(getFirstPage().getFileName());
        IFile newFile = ResourcesPlugin.getWorkspace().getRoot().getFile(filePath);
        String packageName = NedResourcesPlugin.getNedResources().getExpectedPackageFor(newFile);
        context.getVariables().put("nedPackageName", StringUtils.defaultString(packageName,""));

        return context;
    }

}
