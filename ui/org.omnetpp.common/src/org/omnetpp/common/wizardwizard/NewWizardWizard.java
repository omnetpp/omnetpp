package org.omnetpp.common.wizardwizard;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedWizard;


/**
 * "New Wizard" wizard
 *
 * @author Andras
 */
public class NewWizardWizard extends TemplateBasedWizard implements INewWizard {
    private NewWizardProjectSelectionPage firstPage;
    private IStructuredSelection selection;

    @Override
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        super.init(workbench, selection);
        this.selection = selection;
    }

    @Override
    public void addPages() {
        setWindowTitle(isImporting() ? "Import a Wizard" : "New Wizard");

        addPage(firstPage = new NewWizardProjectSelectionPage("first page", selection));
        firstPage.setTitle(isImporting() ? "Import a Wizard" : "New Wizard");
        firstPage.setDescription((isImporting() ? "Import" : "Contribute") + " a wizard into one of the New... dialogs");

        super.addPages();
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);
        context.setVariable("newWizardName", firstPage.getWizardName());
        context.setVariable("newWizardProject", firstPage.getProject());
        return context;
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        return firstPage.getWizardFolder();
    }

    public String getWizardType() {
        return "wizard";
    }

}
