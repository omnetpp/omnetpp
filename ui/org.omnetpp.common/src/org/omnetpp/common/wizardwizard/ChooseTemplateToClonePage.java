package org.omnetpp.common.wizardwizard;

import org.apache.commons.lang.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.TemplateSelectionPage;

/**
 * Custom page for the "Clone Built-in Template" wizard wizard
 * @author Andras
 */
public class ChooseTemplateToClonePage extends TemplateSelectionPage implements ICustomWizardPage {
    public ChooseTemplateToClonePage(String name, IContentTemplate creatorTemplate, String condition) {
        super(null, true);
    }

    @Override
    protected String getTemplateHoverText(IContentTemplate template) {
        String text = super.getTemplateHoverText(template);
        text += "<br/><br/>Supported wizard types: " + StringUtils.join(template.getSupportedWizardTypes(), ", ");
        return text;
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
