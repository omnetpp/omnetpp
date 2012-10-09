package org.omnetpp.common.importexportwizards;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.common.wizard.TemplateBasedWizard;

public class ExportWizard extends TemplateBasedWizard implements IExportWizard {
    private IWorkbench workbench;
    private IStructuredSelection selection;

    public ExportWizard() {
        setWizardType("export");
    }

    @Override
    public void init(IWorkbench workbench, IStructuredSelection currentSelection) {
        super.init(workbench, selection);
        this.workbench = workbench;
        this.selection = currentSelection;
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    public void addPages() {
        super.addPages();
    }

    @Override
    protected IContainer getFolder() {
        return null;
    }
}
