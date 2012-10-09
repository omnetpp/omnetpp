package org.omnetpp.common.wizard;

import org.eclipse.ui.IWorkbenchWizard;

/**
 * Interface for template-based wizards.
 *
 * @author Andras
 */
public interface ITemplateBasedWizard extends IWorkbenchWizard {

    /**
     * By default, the wizard will start with a template selection page.
     * To start with a specific template instead (and skip the page),
     * call this method right after wizard creation.
     */
    void setTemplate(IContentTemplate template);

    /**
     * Sets whether this wizard acts as a "New..." wizard (false; this is
     * the default), or as an Import wizard (true). This setting may affect
     * the labels on the resource selection page of the wizard
     * (e.g. display "Import Network" instead of "New Network").
     */
    void setImporting(boolean importing);
}
