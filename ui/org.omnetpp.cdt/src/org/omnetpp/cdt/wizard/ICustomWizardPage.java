package org.omnetpp.cdt.wizard;

import org.eclipse.jface.wizard.IWizardPage;

/**
 * Custom wizard page for use with IProjectTemplate.
 * 
 * @author Andras
 */
public interface ICustomWizardPage extends IWizardPage {

    /**
     * Fill widgets of this wizard page from the context variables.
     * This is called by the container before the page gets shown.
     * Note: this call may take place BEFORE createControl(); in that case,
     * the page is supposed to remember the context, and populate the
     * widgets from it at the end of createControl(). 
     */
    void populatePage(CreationContext context);

    /**
     * Update the context variables based on the contents of this wizard page.
     * This is called by the container when the user navigates to another
     * wizard page or clicks Finish.
     */
    void extractPageContent(CreationContext context);

    /**
     * Returns whether this wizard page may be shown, according to some condition
     * expressed in terms of context variables. This is called by the container 
     * when the user clicks Next in a previous wizard page, in order to decide
     * which page to show next.    
     */
    boolean isEnabled(CreationContext context);

}
