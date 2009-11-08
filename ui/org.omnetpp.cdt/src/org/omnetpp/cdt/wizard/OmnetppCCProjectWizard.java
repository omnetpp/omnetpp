/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CProjectNature;
import org.eclipse.cdt.ui.CUIPlugin;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.wizards.CDTCommonProjectWizard;
import org.eclipse.cdt.ui.wizards.CDTMainWizardPage;
import org.eclipse.cdt.ui.wizards.EntryDescriptor;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.internal.ide.dialogs.ProjectContentsLocationArea;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.TemplateSelectionPage;
import org.omnetpp.ide.wizard.NewOmnetppProjectWizard;


/**
 * Like OmnetppNewProjectWizard, but continues in a customized
 * "New CDT Project" Wizard.
 *
 * @author Andras
 */
@SuppressWarnings("restriction")
public class OmnetppCCProjectWizard extends NewOmnetppProjectWizard implements INewWizard {
    public static final Image ICON_CATEGORY = Activator.getCachedImage("icons/full/obj16/templatecategory.gif");

    private CCProjectWizard nestedWizard;
    private TemplateSelectionPage templatePage;
    private ICustomWizardPage[] templateCustomPages = new ICustomWizardPage[0]; // never null
    private IContentTemplate creatorOfCustomPages;
    private CreationContext context;
    
    public class NewOmnetppCppProjectCreationPage extends NewOmnetppProjectCreationPage {
        private Button supportCppButton;

        public NewOmnetppCppProjectCreationPage() {
            setDescription("Creates an OMNeT++ project, optionally with support for C++ development using CDT.");
        }

        @Override
        public void createControl(Composite parent) {
            super.createControl(parent);
            // add a new supports C++ development checkbox
            Composite comp = new Composite((Composite)getControl(), SWT.NONE);
            comp.setLayout(new GridLayout(1,false));
            comp.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            supportCppButton = new Button(comp, SWT.CHECK);
            supportCppButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
            supportCppButton.setText("&Support C++ Development");
            supportCppButton.setSelection(true);
            supportCppButton.addSelectionListener(new SelectionAdapter() {
                public void widgetSelected(SelectionEvent e) {
                    getWizard().getContainer().updateButtons();
                }
            });
        }

        public boolean withCplusplusSupport() {
            return supportCppButton.getSelection();
        }

    }

    /**
     * Customizations:
     *   (1) hide project name / location because we have a separate page for that
     *   (2) filter the project types and templates shown
     */
    public class CustomizedCDTMainPage extends CDTMainWizardPage {
        public CustomizedCDTMainPage() {
            super(CUIPlugin.getResourceString("CProjectWizard"));
            setTitle("C++ Project Type");
            setDescription("Select C++ project type and toolchain");
        }

        @Override
        public void createControl(Composite parent) {
            super.createControl(parent);

            // hide project name and location -- we have a separate project page
            hideControl(getProjectNameField().getParent());
            hideControl((Control)ReflectionUtils.getFieldValue(getLocationArea(), "locationLabel"));
            hideControl((Control)ReflectionUtils.getFieldValue(getLocationArea(), "locationPathField"));
            hideControl((Control)ReflectionUtils.getFieldValue(getLocationArea(), "browseButton"));
            hideControl((Control)ReflectionUtils.getFieldValue(getLocationArea(), "useDefaultsButton"));

            // select "Show supported configurations only" checkbox (this comes from a preference in CDT, but we don't care) 
            ((Button)ReflectionUtils.getFieldValue(this, "show_sup")).setSelection(true);
        }

        private void hideControl(Control control) {
            control.setVisible(false);
            GridData gridData = new GridData();
            gridData.exclude = true;
            control.setLayoutData(gridData);
        }

        @Override
        public void setVisible(boolean visible) {
            super.setVisible(visible);

            // copy project name and location from the project page
            String projectName = projectPage.getProjectName();
            boolean useDefaultLocation = projectPage.useDefaults();
            IPath location = projectPage.getLocationPath();
            getProjectNameField().setText(projectName);
            ((Button)ReflectionUtils.getFieldValue(getLocationArea(), "useDefaultsButton")).setSelection(useDefaultLocation);
            if (!useDefaultLocation)
                ((Text)ReflectionUtils.getFieldValue(getLocationArea(), "locationPathField")).setText(location.toString());
        }
        
		public ProjectContentsLocationArea getLocationArea() {
        	return (ProjectContentsLocationArea)ReflectionUtils.getFieldValue(this, "locationArea");
        }

        public Text getProjectNameField() {
        	return (Text)ReflectionUtils.getFieldValue(this, "projectNameField");
        }
        
        @SuppressWarnings("unchecked")
		@Override
        public List<EntryDescriptor> filterItems(List items) {
            ArrayList<EntryDescriptor> newItems = new ArrayList<EntryDescriptor>();
            for (Object o : items) {
            	EntryDescriptor entry = (EntryDescriptor)o;
                if (entry.getId().startsWith("org.omnetpp"))
                    newItems.add(entry);
            }
            return newItems;
        }
    }

    
    public class CCProjectWizard extends CDTCommonProjectWizard {
        public CCProjectWizard() {
            super(UIMessages.getString("NewModelProjectWizard.2"), UIMessages.getString("NewModelProjectWizard.3")); //$NON-NLS-1$ //$NON-NLS-2$
        }

        @Override
        public void addPages() {
            fMainPage = new CustomizedCDTMainPage();
            addPage(fMainPage);
        }

        public String[] getNatures() {
            return new String[] { CProjectNature.C_NATURE_ID, CCProjectNature.CC_NATURE_ID };
        }

        protected IProject continueCreation(IProject prj) {
            try {
                CProjectNature.addCNature(prj, new NullProgressMonitor());
                CCProjectNature.addCCNature(prj, new NullProgressMonitor());
            } catch (CoreException e) {
                Activator.logError(e);
            }
            return prj;
        }
    }
    
    @Override
    public void init(IWorkbench workbench, IStructuredSelection selection) {
        nestedWizard = new CCProjectWizard();
        nestedWizard.init(workbench, selection);
        nestedWizard.setContainer(getContainer());
        setForcePreviousAndNextButtons(true);
    }

    @Override
    public void addPages() {
        addPage(projectPage = new NewOmnetppCppProjectCreationPage());
        addPage(templatePage = new TemplateSelectionPage());
        nestedWizard.addPages(); // this actually adds pages to the *nested* wizard, not this one!
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
		int indexInTemplateCustomPages = ArrayUtils.indexOf(templateCustomPages, page);
    	if (page == projectPage) {
            templatePage.setTemplates(getTemplates());  // so that it can adapt to the withCplusplusSupport setting
            return templatePage;
    	}
    	else if (page == templatePage) {
    		// if there is a template selected, return its first enabled custom page (if it has one)
    		IContentTemplate selectedTemplate = templatePage.getSelectedTemplate();
    		if (selectedTemplate == null) {
    			context = null;
    		}
    		else {
    			if (selectedTemplate != creatorOfCustomPages) {
    				try {
    					context = selectedTemplate.createContext(projectPage.getProjectHandle());
    					context.getVariables().put("wizardType", "project");
    					context.getVariables().put("nedPackageName", StringUtils.makeValidIdentifier(projectPage.getProjectName()).toLowerCase());
						templateCustomPages = selectedTemplate.createCustomPages();
					} catch (CoreException e) {
						ErrorDialog.openError(getShell(), "Error", "Error creating wizard pages", e.getStatus());
						Activator.logError(e);
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
    		// or go right to the first CDT page
    		return withCplusplusSupport() ? nestedWizard.getStartingPage() : null;
    	}
    	else if (indexInTemplateCustomPages != -1) {
        	// store page content before navigating away
    		((ICustomWizardPage)page).extractPageContent(context);
    		
    		// return next custom page if there is one
			ICustomWizardPage nextPage = getNextEnabledCustomPage(templateCustomPages, indexInTemplateCustomPages+1, context);
			if (nextPage != null) {
				nextPage.populatePage(context);
				return nextPage;
			}
			
			// otherwise go to the first CDT page
			return withCplusplusSupport() ? nestedWizard.getStartingPage() : null;
    	}
    	else {
    		return super.getNextPage(page);
    	}
    }
    
    protected static ICustomWizardPage getNextEnabledCustomPage(ICustomWizardPage[] pages, int start, CreationContext context) {
    	for (int k = start; k < pages.length; k++)
    		if (pages[k].isEnabled(context))
    			return pages[k];
    	return null;
    }
    
    @Override
    public boolean performFinish() {
    	// first, make sure we have good template, context and templateCustomPages 
    	// variables, depending on the page the user pressed Finish on
    	IWizardPage finishingPage = getContainer().getCurrentPage();
    	final IProject project = projectPage.getProjectHandle();
    	
    	// if we are on the first page, we use no template at all, otherwise we use the selected one
    	final IContentTemplate template;
    	if (finishingPage == projectPage) {
    		template = null;
    		context = null;
    		templateCustomPages = new ICustomWizardPage[0];
    	}
    	else {
        	template = templatePage.getSelectedTemplate();
    	}

    	// if we are on the template selection page, create a fresh context with the selected template
    	if (finishingPage == templatePage) {
    		context = template!=null ? template.createContext(project) : null;
    		if (context!=null) {
    		    context.getVariables().put("wizardType", "project");
                context.getVariables().put("nedPackageName", StringUtils.makeValidIdentifier(projectPage.getProjectName()).toLowerCase());
    		}

    		templateCustomPages = new ICustomWizardPage[0]; // no pages (yet)
    	}
    	
    	// sanity check
    	Assert.isTrue(context==null || context.getFolder().equals(project));
    	
    	// store custom page content before navigating away
    	if (ArrayUtils.contains(templateCustomPages, finishingPage))
    		((ICustomWizardPage)finishingPage).extractPageContent(context);
    	
    	// if we are not yet on a CDT page, the CDT wizard is not yet created and 
    	// its performFinish() would not be called, so we have to do it manually
    	final boolean withCPlusPlus = withCplusplusSupport();
		if (finishingPage == projectPage || finishingPage == templatePage || ArrayUtils.contains(templateCustomPages, finishingPage)) {
            if (withCPlusPlus) {
                // show it manually (and create) and do it
                getContainer().showPage(nestedWizard.getStartingPage());
                nestedWizard.performFinish();
            }
            else {
                // just call the plain OMNeT++ wizard
                super.performFinish();
            }
        }

        // define the operation for configuring the new project
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                // add OMNeT++ nature
                ProjectUtils.addOmnetppNature(project, monitor);

                // if C++ project, add a default .buildspec file (templates may overwrite it)
                if (withCPlusPlus)
                    BuildSpecification.createInitial(project).save();

                // apply template: this may create files, set project properties, configure the CDT project, etc.
                if (template != null) {
                	try {
                		context.setProgressMonitor(monitor);
                		Assert.isTrue(context.getFolder().equals(project));
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
            Activator.logError(t);
            String msg = StringUtils.defaultIfEmpty(t.getMessage(), t.getClass().getSimpleName());
            MessageDialog.openError(getShell(), "Creation problems", "Error: " + msg);
            return false;
        }
        return true;
    }

    public boolean withCplusplusSupport() {
        return ((NewOmnetppCppProjectCreationPage)projectPage).withCplusplusSupport();
    }

	protected List<IContentTemplate> getTemplates() {
		List<IContentTemplate> result = new ArrayList<IContentTemplate>();
		
		// built-in templates
        if (withCplusplusSupport())
        	result.addAll(BuiltinProjectTemplates.getCppTemplates());
        else 
        	result.addAll(BuiltinProjectTemplates.getNoncppTemplates());
        
        // templates loaded from workspace projects
        //TODO C++ or not C++ projects? (i.e. some templates may require C++ support)
        result.addAll(loadTemplatesFromWorkspace());

        // TODO: templates defined via (future) extension point
        
		return result;
	}

	protected List<IContentTemplate> loadTemplatesFromWorkspace() {
		// check the "templates/project" subdirectory of each OMNeT++ project
		List<IContentTemplate> result = new ArrayList<IContentTemplate>();
		for (IProject project : ProjectUtils.getOmnetppProjects()) {
			IFolder rootFolder = project.getFolder(new Path("templates"));
			if (rootFolder.exists()) {
				try {
					// each template is a folder which contains a "template.properties" file
					for (IResource resource : rootFolder.members()) {
						if (resource instanceof IFolder && ((IFolder)resource).getFile(WorkspaceBasedProjectTemplate.TEMPLATE_PROPERTIES_FILENAME).exists())
							result.add(loadTemplateFrom((IFolder)resource));
					}
				} catch (CoreException e) {
					Activator.logError("Error loading project templates from " + rootFolder.toString(), e);
				}
			}
		}
		return result;
	}

	protected IContentTemplate loadTemplateFrom(IFolder folder) throws CoreException {
		return new WorkspaceBasedProjectTemplate(folder);
	}

    
}


