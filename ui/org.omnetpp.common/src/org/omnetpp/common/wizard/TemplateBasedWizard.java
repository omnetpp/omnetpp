/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.project.ProjectUtils;


/**
 * Generic "New..." wizard which supports dynamically contributed templates.
 *
 * @author Andras
 */
public abstract class TemplateBasedWizard extends Wizard implements INewWizard {
    public static final Image ICON_CATEGORY = null; //FIXME CommonPlugin.getImageDescriptor("icons/full/obj16/templatecategory.gif").createImage();

    private TemplateSelectionPage templateSelectionPage;
    private ICustomWizardPage[] templateCustomPages = new ICustomWizardPage[0]; // never null
    private IContentTemplate creatorOfCustomPages;
    private CreationContext context;
    
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        setForcePreviousAndNextButtons(true);
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
    }

    /**
     * Returns the resource the wizard should create, or in which it should create content.
     * The resource may be a folder or project to be created, or an existing
     * folder or project in which the wizard will create files/folders.
     */
    protected abstract IContainer getFolder();

    /**
     * Returns the templates to be shown on the Template Selection page. The set of templates 
     * returned may depend on options the user chose on previous wizard pages (such as the 
     * "With C++ Support" checkbox in the New OMNeT++ Project wizard.)
     */
    protected abstract List<IContentTemplate> getTemplates();

    /**
     * Utility method for getTemplates()
     */
    protected List<IContentTemplate> loadTemplatesFromWorkspace() {
        // check the "templates/project" subdirectory of each OMNeT++ project
        List<IContentTemplate> result = new ArrayList<IContentTemplate>();
        for (IProject project : ProjectUtils.getOmnetppProjects()) {
            IFolder rootFolder = project.getFolder(new Path("templates/project"));
            if (rootFolder.exists()) {
                try {
                    // each template is a folder which contains a "template.properties" file
                    for (IResource resource : rootFolder.members()) {
                        if (resource instanceof IFolder && ((IFolder)resource).getFile(WorkspaceBasedContentTemplate.TEMPLATE_PROPERTIES_FILENAME).exists())
                            result.add(loadTemplateFrom((IFolder)resource));
                    }
                } catch (CoreException e) {
                    CommonPlugin.logError("Error loading project templates from " + rootFolder.toString(), e);
                }
            }
        }
        return result;
    }

    protected IContentTemplate loadTemplateFrom(IFolder folder) throws CoreException {
        return new WorkspaceBasedContentTemplate(folder);
    }
    
    @Override
    public IWizardPage getPreviousPage(IWizardPage page) {
    	// store page content before navigating away
    	//
    	// NOTE: THIS DOES NOT ALWAYS GET CALLED BY THE DEFAULT WizardPage IMPLEMENTATION 
    	// WHEN 'BACK' IS CLICKED! ICustomWizardPage MUST OVERRIDE THAT IMPLEMENTATION
    	// TO CALL THIS METHOD EVERY TIME, EVENIF IT DOESN'T USE THE RETURN VALUE.
		if (ArrayUtils.contains(templateCustomPages, page))
			((ICustomWizardPage)page).extractPageContent(context);
    	return super.getPreviousPage(page);
    }
    
    @Override
    public IWizardPage getNextPage(IWizardPage page) {
        // update template list just before flipping to the template selection page.
    	if (ArrayUtils.indexOf(getPages(),page) == ArrayUtils.indexOf(getPages(),templateSelectionPage)-1) {
            templateSelectionPage.setTemplates(getTemplates());  
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
    					context = selectedTemplate.createContext(getFolder());
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
            return getFirstExtraPage();  // first CDT page or something
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
			
			return getFirstExtraPage();  // first CDT page or something
    	}
    	
    	return super.getNextPage(page);
    }

    protected static ICustomWizardPage getNextEnabledCustomPage(ICustomWizardPage[] pages, int start, CreationContext context) {
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
    	
    	// if we are before the template selection page, we'll use no template at all, otherwise we'll use the selected one
    	final IContentTemplate template;
        if (ArrayUtils.indexOf(getPages(),finishingPage) < ArrayUtils.indexOf(getPages(),templateSelectionPage)) {
    		template = null;
    		context = null;
    		templateCustomPages = new ICustomWizardPage[0];
    	}
    	else {
        	template = templateSelectionPage.getSelectedTemplate();
    	}

    	// if we are on the template selection page, create a fresh context with the selected template
    	if (finishingPage == templateSelectionPage) {
    		context = template!=null ? template.createContext(folder) : null;
    		templateCustomPages = new ICustomWizardPage[0]; // no pages (yet)
    	}
    	
    	// sanity check
    	Assert.isTrue(context==null || context.getFolder().equals(folder));
    	
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
                		Assert.isTrue(context.getFolder() == folder);
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
            String msg = StringUtils.defaultIfEmpty(t.getMessage(), t.getClass().getSimpleName());
            MessageDialog.openError(getShell(), "Creation problems", "Error: " + msg);
            return false;
        }
        return true;
    }

    
}


