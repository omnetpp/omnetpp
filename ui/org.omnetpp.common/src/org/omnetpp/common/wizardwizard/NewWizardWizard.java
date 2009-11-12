package org.omnetpp.common.wizardwizard;

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

    @Override
    protected IWizardPage getFirstExtraPage() {
        return null;
    }

    @Override
    protected IContainer getFolder() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    protected List<IContentTemplate> getTemplates() {
        // TODO Auto-generated method stub
        return null;
    }

}
