package org.omnetpp.common.wizardwizard;

import java.util.List;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.TemplateBasedWizard;
import org.omnetpp.common.wizard.TemplateSelectionPage;

/**
 * Custom page for the "Clone Built-in Template" wizard wizard
 * @author Andras
 */
public class ChooseTemplateToClonePage extends TemplateSelectionPage implements ICustomWizardPage {
    public ChooseTemplateToClonePage(String name, IContentTemplate creatorTemplate, String condition) {
        super();
    }

    @Override
    public void createControl(Composite parent) {
        super.createControl(parent);
        
        List<IContentTemplate> templates = ((TemplateBasedWizard)getWizard()).getAllTemplates();
        setTemplates(templates);
    }
    
    public boolean isEnabled(CreationContext context) {
        return true;
    }

    public void populatePage(CreationContext context) {
        // nothing
    }

    public void extractPageContent(CreationContext context) {
        context.setVariable("templateToClone", getSelectedTemplate());
    }

}
