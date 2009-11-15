package org.omnetpp.common.wizardwizard;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.wizard.IWizardPage;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.TemplateBasedWizard;


/**
 * "New Wizard" wizard
 * 
 * @author Andras
 */
public class NewWizardWizard extends TemplateBasedWizard {
    private NewWizardProjectSelectionPage firstPage;
    
    @Override
    public void addPages() {
        setWindowTitle("New Wizard");
        addPage(firstPage = new NewWizardProjectSelectionPage("first page"));
        super.addPages();
    }
    
    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        // TODO Auto-generated method stub
        return null;
    }

    public String getWizardType() {
        return "wizard";
    }
    
    @Override
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        result.addAll(loadBuiltinTemplates(getWizardType()));
        result.addAll(loadTemplatesFromWorkspace(getWizardType()));
        return result;
    }

}
