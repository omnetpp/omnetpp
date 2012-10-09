package org.omnetpp.common.importexportwizards;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IImportWizard;
import org.eclipse.ui.IWorkbench;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ITemplateBasedWizard;
import org.omnetpp.common.wizard.TemplateSelectionPage;

/**
 * Import OMNeT++ File wizard. Brings up the IContentTemplates that support the
 * wizard type "import", lets the user pick one, then delegates to a wizard
 * that can run the given template.
 *
 * @author Andras
 */
public class ImportWizard extends Wizard implements IImportWizard {
    private static final String NEWWIZARDS_EXTENSION_POINT_ID = "org.eclipse.ui.newWizards";
    private TemplateSelectionPage templateSelectionPage;
    private IWorkbench workbench;
    private IStructuredSelection selection;

    private static Map<String,String> wizardTypeToId = new HashMap<String, String>();
    static {
        wizardTypeToId.put("nedfile", IConstants.NEW_NEDFILE_WIZARD_ID);
        wizardTypeToId.put("msgfile", IConstants.NEW_MSGFILE_WIZARD_ID);
        wizardTypeToId.put("inifile", IConstants.NEW_INIFILE_WIZARD_ID);
        wizardTypeToId.put("network", IConstants.NEW_NETWORK_WIZARD_ID);
        wizardTypeToId.put("compoundmodule", IConstants.NEW_COMPOUND_MODULE_WIZARD_ID);
        wizardTypeToId.put("simplemodule", IConstants.NEW_SIMPLE_MODULE_WIZARD_ID);
        wizardTypeToId.put("simulation", IConstants.NEW_SIMULATION_WIZARD_ID);
        wizardTypeToId.put("project", IConstants.NEW_OMNETPP_CC_PROJECT_WIZARD_ID);
    }

    public ImportWizard() {
        // code copied from PreferencesExportWizard
        IDialogSettings workbenchSettings = CommonPlugin.getDefault().getDialogSettings();
        IDialogSettings section = workbenchSettings.getSection("ImportWizard");
        if (section == null)
            section = workbenchSettings.addNewSection("ImportWizard");
        setDialogSettings(section);
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
        this.workbench = workbench;
        this.selection = selection;
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
            if (template != null) {
                String wizardType = chooseWizardTypeFor(template);
                try {
                    ITemplateBasedWizard wizard = createWizard(wizardType);
                    wizard.setContainer(getContainer());
                    wizard.setTemplate(template);
                    wizard.setImporting(true);
                    wizard.init(workbench, selection);
                    wizard.addPages();
                    return wizard.getStartingPage();
                }
                catch (CoreException e) {
                    CommonPlugin.logError("Cannot create wizard for template " + template.getName(), e);
                }
            }
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


    protected ITemplateBasedWizard createWizard(String wizardType) throws CoreException {
        String id = wizardTypeToId.get(wizardType);
        if (id == null)
            throw new IllegalArgumentException("Cannot find wizard for wizardType='" + wizardType + "'");

        IConfigurationElement wizardConfigElement = null;
        IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(NEWWIZARDS_EXTENSION_POINT_ID);
        for (IConfigurationElement e : config)
            if (id.equals(e.getAttribute("id")))
                wizardConfigElement = e;
        if (wizardConfigElement == null)
            throw new IllegalArgumentException("Cannot find wizard for wizardType='" + wizardType + "'");

        return (ITemplateBasedWizard) wizardConfigElement.createExecutableExtension("class");
    }

    @Override
    public boolean performFinish() {
        return false;
    }
}
