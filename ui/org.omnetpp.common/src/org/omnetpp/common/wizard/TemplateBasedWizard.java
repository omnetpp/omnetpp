/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;

import java.lang.reflect.InvocationTargetException;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.ui.DetailMessageDialog;
import org.omnetpp.common.util.StringUtils;

import freemarker.template.TemplateException;


/**
 * A generic wizard which supports content templates (IContentTemplate).
 * The wizard may come up with a template selection page, or with a pre-selected
 * template. The former mode is the default; the latter mode can be selected by
 * calling setTemplate() right after wizard creation.
 *
 * @author Andras
 */
public abstract class TemplateBasedWizard extends Wizard implements ITemplateBasedWizard {
    private String wizardType;
    private IContentTemplate preselectedTemplate; // if this is given, templateSelectionPage will remain null
    private TemplateSelectionPage templateSelectionPage;

    // selection state:
    private ICustomWizardPage[] templateCustomPages = new ICustomWizardPage[0]; // never null
    private IContentTemplate creatorOfCustomPages;
    private CreationContext context;
    private WizardPage dummyPage;
    private boolean importing;

    static class DummyPage extends WizardPage {
        public DummyPage() {
            super("dummy");
            setTitle("Done");
            setDescription("This wizard does not need more input.");
        }

        public void createControl(Composite parent) {
            Composite composite = new Composite(parent, SWT.NONE);
            composite.setLayout(new GridLayout());
            Label label = new Label(composite, SWT.NONE);
            label.setText("Click Finish to complete the wizard.");
            setControl(composite);
        }
    }

    public TemplateBasedWizard() {
        // code copied from PreferencesExportWizard
        IDialogSettings workbenchSettings = CommonPlugin.getDefault().getDialogSettings();
        IDialogSettings section = workbenchSettings.getSection("TemplateBasedWizard");
        if (section == null)
            section = workbenchSettings.addNewSection("TemplateBasedWizard");
        setDialogSettings(section);
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
    }

    public void setTemplate(IContentTemplate preselectedTemplate) {
        if (preselectedTemplate == null)
            throw new NullPointerException("argument cannot be null");
        if (templateSelectionPage != null)
            throw new IllegalArgumentException("too late: template selection page already created and added");
        this.preselectedTemplate = preselectedTemplate;
    }

    public void setImporting(boolean importing) {
        this.importing = importing;
    }

    public boolean isImporting() {
        return importing;
    }

    public String getWizardType() {
        return wizardType;
    }

    public void setWizardType(String wizardType) {
        this.wizardType = wizardType;
    }

    protected CreationContext getContext() {
        return context;
    }

    /**
     * Template custom pages will always directly follow the template selection page. To add
     * extra pages before the template selection page or after the last template custom page,
     * override this method and add the pages before/after the super.addPages() call.
     */
    @Override
    public void addPages() {
        // note: template custom pages will be added in getNextPage() of the template selection page.
        if (preselectedTemplate == null)
            addPage(templateSelectionPage = createTemplateSelectionPage());

        // a dummy page is needed, otherwise the Next button will be disabled on the template selection page
        addPage(dummyPage = new DummyPage());
    }

    /**
     * Override if you want to customize the template selection page.
     */
    protected TemplateSelectionPage createTemplateSelectionPage() {
        return new TemplateSelectionPage(getWizardType(), true);
    }

    /**
     * Return the resource the wizard should create, or in which it should create content.
     * The resource may be a folder or project to be created, or an existing
     * folder or project in which the wizard will create files/folders.
     *
     * Suggested implementation is to call the appropriate getter method of the
     * file/folder selection page of the wizard (which should be before the template
     * selection page.)
     */
    protected abstract IContainer getFolder();

    public TemplateSelectionPage getTemplateSelectionPage() {
        return templateSelectionPage;
    }

    public IContentTemplate getSelectedTemplate() {
        return preselectedTemplate!=null ? preselectedTemplate : templateSelectionPage.getSelectedTemplate();
    }

    public CreationContext getCreationContext() {
        return context;
    }

    @Override
    public IWizardPage getPreviousPage(IWizardPage page) {
        // store page content before navigating away
        //
        // NOTE: THIS DOES NOT ALWAYS GET CALLED BY THE DEFAULT WizardPage IMPLEMENTATION
        // WHEN 'BACK' IS CLICKED! ICustomWizardPage MUST OVERRIDE THAT IMPLEMENTATION
        // TO CALL THIS METHOD EVERY TIME, EVEN IF IT DOESN'T USE THE RETURN VALUE.
        if (ArrayUtils.contains(templateCustomPages, page))
            ((ICustomWizardPage)page).extractPageContent(context);
        return super.getPreviousPage(page);
    }

    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        if (templateSelectionPage!=null && ArrayUtils.indexOf(getPages(),page) == ArrayUtils.indexOf(getPages(),templateSelectionPage)-1)
            return templateSelectionPage;

        // if there is a template selected, return its first enabled custom page (if it has one)
        if (page == templateSelectionPage || (page==getStartingPage() && templateSelectionPage==null)) {
            IContentTemplate selectedTemplate = getSelectedTemplate();
            if (selectedTemplate == null) {
                context = null;
            }
            else {
                if (selectedTemplate != creatorOfCustomPages || !equal(getFolder(), context.getFolder())) {
                    try {
                        context = createContext(selectedTemplate, getFolder());
                        templateCustomPages = selectedTemplate.createCustomPages();
                    } catch (CoreException e) {
                        ErrorDialog.openError(getShell(), "Error", "Error creating wizard pages", e.getStatus());
                        CommonPlugin.logError(e);
                        templateCustomPages = new ICustomWizardPage[0];
                    }
                    Assert.isNotNull(templateCustomPages);
                    for (IWizardPage customPage : templateCustomPages)
                        addPage(customPage);
                    creatorOfCustomPages = selectedTemplate;
                }

                ICustomWizardPage firstCustomPage = getNextEnabledCustomPage(templateCustomPages, 0, context);
                if (firstCustomPage != null) {
                    firstCustomPage.populatePage(context);
                    return firstCustomPage;
                }
            }
            return getFirstExtraPage()!=null ? getFirstExtraPage() : dummyPage;  // first CDT page or the like
        }

        // next custom page
        int indexInTemplateCustomPages = ArrayUtils.indexOf(templateCustomPages, page);
        if (indexInTemplateCustomPages != -1) {
            // store page content before navigating away
            ((ICustomWizardPage)page).extractPageContent(context);

            // return next custom page if there is one
            ICustomWizardPage nextPage = getNextEnabledCustomPage(templateCustomPages, indexInTemplateCustomPages+1, context);
            if (nextPage != null) {
                nextPage.populatePage(context);
                return nextPage;
            }

            // XXX we need to fake a dummy page: if all subsequent pages are conditional, and get enabled only by a control on THIS page, the Next button may stay disabled!!!
            return getFirstExtraPage()!=null ? getFirstExtraPage() : dummyPage;
        }

        // there's nothing after the dummy page
        if (page == dummyPage) {
            return null;
        }

        return super.getNextPage(page);
    }

    /**
     * Produces an initial creation context for the given template. Override if you need to
     * put extra variables into the context (i.e. a file name).
     */
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = selectedTemplate.createContextFor(folder, this, getWizardType());
        return context;
    }

    private static ICustomWizardPage getNextEnabledCustomPage(ICustomWizardPage[] pages, int start, CreationContext context) {
        for (int k = start; k < pages.length; k++)
            if (pages[k].isEnabled(context))
                return pages[k];
        return null;
    }

    /**
     * Return the first page after the template selection page and template custom pages
     */
    protected abstract IWizardPage getFirstExtraPage();

    @Override
    public boolean performFinish() {
        // first, make sure we have good template, context and templateCustomPages
        // variables, depending on the page the user pressed Finish on
        IWizardPage finishingPage = getContainer().getCurrentPage();
        final IContainer folder = getFolder();

        // use the selected template (if we are before the template selection page, this will be
        // the "default" template, i.e. the one with "templateIsDefault=true")
        final IContentTemplate template = preselectedTemplate!=null ? preselectedTemplate : templateSelectionPage.getSelectedTemplate();
        getDialogSettings().put(getWizardType()+".template", template.getIdentifierString()); // see also getLastChosenTemplate()

        // if we are on or before the template selection page, create a fresh context with the selected template
        if (preselectedTemplate==null && finishingPage.getWizard()==this && ArrayUtils.indexOf(getPages(),finishingPage) <= ArrayUtils.indexOf(getPages(),templateSelectionPage)) {
            context = template!=null ? createContext(template, folder) : null;
            templateCustomPages = new ICustomWizardPage[0]; // no pages
        }
        else if (template!=null && (context==null || context.getTemplate()!=template)) {
            // first "if" doesn't fire if user presses Finish right on the 1st page of New OMNeT++ Project wizard
            context = createContext(template, folder);
            templateCustomPages = new ICustomWizardPage[0]; // no pages
        }

        // sanity check
        Assert.isTrue((template==null) == (context==null));
        Assert.isTrue(template==null || context.getTemplate() == template);
        Assert.isTrue(template==null || equal(context.getFolder(), folder));
        checkFolder(folder);

        // store custom page content before navigating away
        if (ArrayUtils.contains(templateCustomPages, finishingPage))
            ((ICustomWizardPage)finishingPage).extractPageContent(context);

        // define the operation for configuring the new project
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                // apply template: this may create files, set project properties, etc.
                if (template != null) {
                    try {
                        context.setProgressMonitor(monitor);
                        Assert.isTrue(equal(context.getFolder(), folder));
                        template.performFinish(context);
                    } finally {
                        context.setProgressMonitor(null);
                    }
                }
            }
        };

        // run the operation
        try {
            getContainer().run(true, true, op);
        }
        catch (InterruptedException e) {
            return false;
        }
        catch (InvocationTargetException e) {
            Throwable t = e.getTargetException();
            CommonPlugin.logError(t);

            // try to present a sensible error dialog to the user
            String errorMessage;
            String detailsText;
            if (t.getCause() instanceof TemplateException) {
                TemplateException templateException = (TemplateException) t.getCause();
                Exception causeException = templateException.getCauseException();
                errorMessage = messageOf(causeException != null ? causeException : templateException);
                String templateStack = templateException.getFTLInstructionStack();
                detailsText = templateException.getMessage() + "\n\nTemplate stack:\n" + templateStack.trim();
            }
            else {
                errorMessage = messageOf(t);
                detailsText = null;
            }

            DetailMessageDialog.openError(getShell(), "Creation problems", "Error: " + errorMessage, detailsText);

            return false;
        }
        return true;
    }

    protected void checkFolder(IContainer folder) {
        // default policy: folder is null only for export wizards
        Assert.isTrue((folder==null) == getWizardType().equals("export"));
    }

    private static boolean equal(Object o1, Object o2) {
        return o1==null ? o2==null : o1.equals(o2);
    }

    private String messageOf(Throwable e) {
        String message = e.getMessage();
        if (StringUtils.isEmpty(message))
            return e.getClass().getSimpleName();
        else
            return message.replace("assertion failed:", "").trim(); // strip "assertion failed" text that some CoreExceptions contain
    }

}


