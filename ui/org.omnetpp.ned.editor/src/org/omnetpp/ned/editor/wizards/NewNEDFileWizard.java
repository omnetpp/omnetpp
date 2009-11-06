package org.omnetpp.ned.editor.wizards;

import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedWizard;

/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NewNEDFileWizard extends TemplateBasedWizard {
    private IStructuredSelection selection;
    private WizardNewFileCreationPage firstPage;

    public NewNEDFileWizard() {
    }
    
    public void init(IWorkbench workbench, IStructuredSelection currentSelection) {
        this.selection = currentSelection;
        setWindowTitle("New NED File");
    }

    @Override
    public void addPages() {
        firstPage = new WizardNewFileCreationPage("file selection page", selection);
        firstPage.setAllowExistingResources(false);
        firstPage.setTitle("New NED File");
        firstPage.setDescription("Choose NED file");
        firstPage.setFileExtension("ned");
        firstPage.setFileName("untitled.ned");
        addPage(firstPage);
        super.addPages();
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);
        context.getVariables().put("fileName", firstPage.getFileName());
        return context;
    }
    
    @Override
    public boolean performFinish() {
        //XXX who will create the folder?
        return super.performFinish();
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        IPath path = firstPage.getContainerFullPath();
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return path.segmentCount()==1 ? root.getProject(path.toString()) : root.getFolder(path);
    }

    @Override
    protected List<IContentTemplate> getTemplates() {
        return loadTemplatesFromWorkspace();  //XXX filtering etc.
    }

}
