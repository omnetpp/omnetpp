/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;

import java.lang.reflect.InvocationTargetException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.SafeRunner;
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
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.DetailMessageDialog;
import org.omnetpp.common.wizard.TemplateSelectionPage.ITemplateAddedCallback;
import org.osgi.framework.Bundle;

import freemarker.template.TemplateException;


/**
 * Generic "New..." wizard which supports dynamically contributed templates.
 *
 * @author Andras
 */
public abstract class TemplateBasedWizard extends Wizard implements INewWizard {
    public static final String TEMPLATES_FOLDER_NAME = "templates";

    public static final String CONTENTTEMPLATE_EXTENSIONPOINT_ID = "org.omnetpp.common.wizard.contenttemplates";
    public static final String PLUGIN_ELEMENT = "plugin";
    public static final String PLUGINID_ATT = "pluginId";
    public static final String FOLDER_ATT = "folder";

    private String wizardType;
    private TemplateSelectionPage templateSelectionPage;
    private List<IContentTemplate> dynamicallyAddedTemplates = new ArrayList<IContentTemplate>();
    
    // selection state:
    private ICustomWizardPage[] templateCustomPages = new ICustomWizardPage[0]; // never null
    private IContentTemplate creatorOfCustomPages;
    private CreationContext context;
    private WizardPage dummyPage;


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
        addPage(templateSelectionPage = new TemplateSelectionPage());
        
        templateSelectionPage.setTemplateAddedCallback(new ITemplateAddedCallback() {
            public void addTemplateFrom(URL url) throws CoreException {
                TemplateBasedWizard.this.addTemplateFrom(url);
            }});

        // a dummy page is needed, otherwise the Next button will be disabled on the template selection page
        addPage(dummyPage = new DummyPage());
    }

    /**
     * Called when the user manually enters a template URL at the template selection page.
     */
    protected void addTemplateFrom(URL url) throws CoreException {
        IContentTemplate newTemplate = loadTemplateFromURL(url, null);
        dynamicallyAddedTemplates.add(newTemplate);
        templateSelectionPage.setTemplates(getTemplates()); // refresh page
        templateSelectionPage.setSelectedTemplate(newTemplate);
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

    /**
     * Return the templates to be shown on the Template Selection page. This implementation
     * returns the templates that match the wizards' type (see getWizardType()).
     * Override this method to add more templates, or to introduce additional
     * filtering (i.e. by options the user selected on previous wizard pages,
     * such as the "With C++ Support" checkbox in the New OMNeT++ Project wizard).
     */
    protected List<IContentTemplate> getTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        result.addAll(loadBuiltinTemplates(getWizardType()));
        result.addAll(loadTemplatesFromWorkspace(getWizardType()));
        result.addAll(dynamicallyAddedTemplates);
        //TODO define an ITemplateSource interface and a matching extension point
        return result;
    }

    /**
     * Utility method for the "Clone Built-in Template" wizard wizard.
     */
    public List<IContentTemplate> getAllTemplates() {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        result.addAll(loadBuiltinTemplates(null));
        result.addAll(loadTemplatesFromWorkspace(null));
        return result;
    }

    /**
     * Loads built-in templates from all plug-ins registered via the
     * org.omnetpp.common.wizard.contenttemplates extension point.
     */
    protected List<IContentTemplate> loadBuiltinTemplates(final String wizardType) {
        final List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        try {
            IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(CONTENTTEMPLATE_EXTENSIONPOINT_ID);
            for (IConfigurationElement e : config) {
                Assert.isTrue(e.getName().equals(PLUGIN_ELEMENT));
                final String pluginName = e.getAttribute(PLUGINID_ATT);
                final String folderName = StringUtils.defaultIfEmpty(e.getAttribute(FOLDER_ATT), TEMPLATES_FOLDER_NAME);
                ISafeRunnable runnable = new ISafeRunnable() {
                    public void run() throws Exception {
                        Bundle bundle = Platform.getBundle(pluginName);
                        if (bundle == null)
                            throw new RuntimeException("Cannot resolve bundle " + pluginName);
                        else
                            result.addAll(loadBuiltinTemplates(bundle, folderName, wizardType));
                    }
                    public void handleException(Throwable e) {
                        CommonPlugin.logError("Cannot read templates from plug-in " + pluginName, e);
                    }
                };
                SafeRunner.run(runnable);
            }
        } catch (Exception ex) {
            CommonPlugin.logError("Error loading built-in templates from plug-ins", ex);
        }
        return result;
    }

    /**
     * Loads built-in templates that support the given wizard type from the given plugin.
     */
    @SuppressWarnings("unchecked")
    protected List<IContentTemplate> loadBuiltinTemplates(Bundle bundle, String folderName, String wizardType) {
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        try {
            Enumeration e = bundle.findEntries(folderName, FileBasedContentTemplate.TEMPLATE_PROPERTIES_FILENAME, true);
            if (e != null) {
                while (e.hasMoreElements()) {
                    URL propFileUrl = (URL) e.nextElement();
                    URL templateUrl = new URL(StringUtils.removeEnd(propFileUrl.toString(), FileBasedContentTemplate.TEMPLATE_PROPERTIES_FILENAME));
                    IContentTemplate template = loadTemplateFromURL(templateUrl, bundle);
                    if (wizardType==null || template.getSupportedWizardTypes().isEmpty() || template.getSupportedWizardTypes().contains(wizardType))
                        if (isSuitableTemplate(template))
                            result.add(template);
                }
            }
        } catch (Exception e) {
            CommonPlugin.logError("TemplateBasedWizard: Could not load content templates from plug-ins", e);
        }
        return result;
    }

    /**
     * Factory method used by loadBuiltinTemplates(); override if you subclass
     * from FileBasedContentTemplate. bundleOfTemplate may be null.
     */
    protected IContentTemplate loadTemplateFromURL(URL templateUrl, Bundle bundleOfTemplate) throws CoreException {
        return new FileBasedContentTemplate(templateUrl, bundleOfTemplate);
    }

    /**
     * Utility method for getTemplates()
     */
    protected List<IContentTemplate> loadTemplatesFromWorkspace(String wizardType) {
        // check the "templates/" subdirectory of each OMNeT++ project
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        for (IProject project : ProjectUtils.getOmnetppProjects()) {
            IFolder rootFolder = project.getFolder(new Path(TEMPLATES_FOLDER_NAME));
            if (rootFolder.exists()) {
                try {
                    // each template is a folder which contains a "template.properties" file
                    for (IResource resource : rootFolder.members()) {
                        if (resource instanceof IFolder && FileBasedContentTemplate.looksLikeTemplateFolder((IFolder)resource)) {
                            IFolder folder = (IFolder)resource;
                            IContentTemplate template = loadTemplateFromWorkspace(folder);
                            if (wizardType==null || template.getSupportedWizardTypes().isEmpty() || template.getSupportedWizardTypes().contains(wizardType))
                                if (isSuitableTemplate(template))
                                    result.add(template);
                        }
                    }
                } catch (CoreException e) {
                    CommonPlugin.logError("Error loading project templates from " + rootFolder.toString(), e);
                }
            }
        }
        return result;
    }

    /**
     * Override for further filtering of templates (beyond wizardType). The default 
     * implementation just returns true. 
     */
    protected boolean isSuitableTemplate(IContentTemplate template) {
        return true;
    }

    /**
     * Factory method used by loadTemplatesFromWorkspace(); override if you subclass
     * from FileBasedContentTemplate.
     */
    protected IContentTemplate loadTemplateFromWorkspace(IFolder folder) throws CoreException {
        return new FileBasedContentTemplate(folder);
    }

    public TemplateSelectionPage getTemplateSelectionPage() {
        return templateSelectionPage;
    }

    public IContentTemplate getSelectedTemplate() {
        return templateSelectionPage.getSelectedTemplate();
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
        // update template list just before flipping to the template selection page.
        if (ArrayUtils.indexOf(getPages(),page) == ArrayUtils.indexOf(getPages(),templateSelectionPage)-1) {
            List<IContentTemplate> templates = getTemplates();
            templateSelectionPage.setTemplates(templates);
            templateSelectionPage.setSelectedTemplate(getLastChosenTemplate(templates));
            
            return templateSelectionPage;
        }

        // if there is a template selected, return its first enabled custom page (if it has one)
        if (page == templateSelectionPage) {
            IContentTemplate selectedTemplate = templateSelectionPage.getSelectedTemplate();
            if (selectedTemplate == null) {
                context = null;
            }
            else {
                if (selectedTemplate != creatorOfCustomPages) {
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

    protected IContentTemplate getLastChosenTemplate(List<IContentTemplate> templates) {
        String ident = getDialogSettings().get(getWizardType()+".template");
        for (IContentTemplate template : templates)
            if (template.getName().equals(ident))
                return template;
        return null;
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
        final IContentTemplate template = templateSelectionPage.getSelectedTemplate();
        getDialogSettings().put(getWizardType()+".template", template.getName()); // see also getLastChosenTemplate(); FIXME use templateURL or something

        // if we are on or before the template selection page, create a fresh context with the selected template
        if (finishingPage.getWizard()==this && ArrayUtils.indexOf(getPages(),finishingPage) <= ArrayUtils.indexOf(getPages(),templateSelectionPage)) {
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
        Assert.isTrue(template==null || context.getFolder().equals(folder));

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
                        Assert.isTrue(context.getFolder().equals(folder));
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

    private String messageOf(Throwable e) {
        String message = e.getMessage();
        if (StringUtils.isEmpty(message))
            return e.getClass().getSimpleName();
        else
            return message.replace("assertion failed:", "").trim(); // strip "assertion failed" text that some CoreExceptions contain
    }

}


