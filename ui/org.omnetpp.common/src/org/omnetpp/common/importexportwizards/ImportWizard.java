package org.omnetpp.common.importexportwizards;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IImportWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateSelectionPage;

public class ImportWizard extends Wizard implements IImportWizard {
    private TemplateSelectionPage templateSelectionPage;

    public ImportWizard() {
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    @Override
    public void addPages() {
        addPage(templateSelectionPage = new TemplateSelectionPage(null, true));
    }

    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        if (page == templateSelectionPage) {
            IContentTemplate template = templateSelectionPage.getSelectedTemplate();
            //TODO create the corresponding wizard... and return its first page
            return null;
        }
        return super.getNextPage(page);
    }
    
    @Override
    public boolean performFinish() {
        return false;
    }
}
