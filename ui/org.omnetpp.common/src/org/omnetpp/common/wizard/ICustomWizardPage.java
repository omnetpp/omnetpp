package org.omnetpp.common.wizard;

import org.eclipse.jface.wizard.IWizardPage;

/**
 * Custom wizard page for use with IProjectTemplate.
 * 
 * IMPORTANT: Implementation must override getPreviousPage() to
 * call the wizard's similar function every time, even if it doesn't use
 * the return value! This is the only way the wizard gets a chance to
 * extract and store page contents when 'Back' is clicked.
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

    /**
     * Needs to override the default WizardPage implementation! See class doc. 
     */
    IWizardPage getPreviousPage();
}
