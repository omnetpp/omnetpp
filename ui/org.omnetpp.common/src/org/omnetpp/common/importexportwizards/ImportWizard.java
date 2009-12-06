package org.omnetpp.common.importexportwizards;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IImportWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateSelectionPage;

/**
 * Import OMNeT++ File wizard. Brings up the IContentTemplates that support the
 * wizard type "import", lets the user pick one, then delegates to a wizard
 * that can run the given template.
 *  
 * @author Andras
 */
public class ImportWizard extends Wizard implements IImportWizard {
    private TemplateSelectionPage templateSelectionPage;

    public ImportWizard() {
        // code copied from PreferencesExportWizard
        IDialogSettings workbenchSettings = CommonPlugin.getDefault().getDialogSettings();
        IDialogSettings section = workbenchSettings.getSection("ImportWizard");
        if (section == null)
            section = workbenchSettings.addNewSection("ImportWizard");
        setDialogSettings(section);
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    @Override
    public void addPages() {
        addPage(templateSelectionPage = new TemplateSelectionPage("import", true)); 
        templateSelectionPage.setTitle("Select Importer");
    }

    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        if (page == templateSelectionPage) {
            IContentTemplate template = templateSelectionPage.getSelectedTemplate();
            String wizardType = chooseWizardTypeFor(template);
            //TODO create the corresponding wizard... and return its first page
            return null;
        }
        return super.getNextPage(page);
    }
    
    protected String chooseWizardTypeFor(IContentTemplate template) {
        // by default we'd like to invoke the "New Network" wizard; if not supported, 
        // here's a decreasing preference order of wizards
        String[] preferredOrder = "network nedfile compoundmodule simplemodule msgfile inifile simulation project".split(" ");
        for (String type : preferredOrder)
            if (template.getSupportedWizardTypes().contains(type))
                return type;
        
        // ok, return what it supports
        for (String type : template.getSupportedWizardTypes())
            if (!type.equals("import"))
                return type;
        
        // looks like it does not support anything
        return null;
    }

    @Override
    public boolean performFinish() {
        return false;
    }
}
