/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
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
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.internal.ide.dialogs.ProjectContentsLocationArea;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.ReflectionUtils;
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
    private IWizardPage[] templateCustomPages = new IWizardPage[0]; // never null
    private IProjectTemplate creatorOfCustomPages;
    
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

    
    public class TemplateSelectionPage extends WizardPage {
        private TreeViewer treeViewer;

        protected TemplateSelectionPage() {
            super("OmnetppTemplateSelectionPage");
            setTitle("Project Contents");
            setDescription("Choose initial project contents");
        }

        public void createControl(Composite parent) {
            Composite composite = new Composite(parent, SWT.NONE);
            composite.setLayout(new GridLayout(1,false));
            setControl(composite);

            // create tree and label
            Label label = new Label(composite, SWT.NONE);
            label.setText("Select template:");
            treeViewer = new TreeViewer(composite, SWT.BORDER);
            treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            treeViewer.setLabelProvider(new LabelProvider() {
                @Override
                public String getText(Object element) {
                    element = ((GenericTreeNode)element).getPayload();
                    return element instanceof IProjectTemplate ? ((IProjectTemplate)element).getName() : element.toString();
                }
                @Override
                public Image getImage(Object element) {
                    element = ((GenericTreeNode)element).getPayload();
                    return element instanceof IProjectTemplate ? ((IProjectTemplate)element).getImage() : ICON_CATEGORY;
                }
            });
            treeViewer.setContentProvider(new GenericTreeContentProvider());

            // show the descriptions in a tooltip
            new HoverSupport().adapt(treeViewer.getTree(), new IHoverTextProvider() {
                public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
                    Item item = treeViewer.getTree().getItem(new Point(x,y));
                    Object element = item==null ? null : item.getData();
                    element = (element instanceof GenericTreeNode) ? ((GenericTreeNode)element).getPayload() : null;
                    if (element instanceof IProjectTemplate) {
                        String description = ((IProjectTemplate)element).getDescription();
                        if (description != null)
                            return HoverSupport.addHTMLStyleSheet(description);
                    }
                    return null;
                }
            });

            // always complete
            setPageComplete(true);
        }

        public void updateTemplateList() {
            // categorize and add templates into the tree
            // NOTE: gets called from first page's getNextPage() method             
            List<IProjectTemplate> templates = getTemplates();
            GenericTreeNode root = new GenericTreeNode("root");
            Set<String> categories = new LinkedHashSet<String>(); 
            for (IProjectTemplate template : templates)
                categories.add(template.getCategory());
            for (String category : categories) {
                GenericTreeNode categoryNode = new GenericTreeNode(category);
                root.addChild(categoryNode);
                for (IProjectTemplate template : templates)
                    if (category.equals(template.getCategory()))
                        categoryNode.addChild(new GenericTreeNode(template));
            }                
            treeViewer.setInput(root);
            treeViewer.expandAll();
        }

        public IProjectTemplate getSelectedTemplate() {
            Object element = ((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
            element = element == null ? null : ((GenericTreeNode)element).getPayload();
            return (element instanceof IProjectTemplate) ? (IProjectTemplate)element : null;
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
    public IWizardPage getNextPage(IWizardPage page) {
		int indexInTemplateCustomPages = ArrayUtils.indexOf(templateCustomPages, page);
    	if (page == projectPage) {
            templatePage.updateTemplateList();  // so that it can adapt to the withCplusplusSupport setting
            return templatePage;
    	}
    	else if (page == templatePage) {
    		// if there is a template selected, return its first custom page (if it has one)
    		IProjectTemplate selectedTemplate = templatePage.getSelectedTemplate();
    		if (selectedTemplate != null) {
    			if (selectedTemplate != creatorOfCustomPages) {
    				try {
						templateCustomPages = selectedTemplate.createCustomPages();
					} catch (CoreException e) {
						ErrorDialog.openError(getShell(), "Error", "Error creating wizard pages", e.getStatus());
						Activator.logError(e);
						templateCustomPages = new IWizardPage[0];
					}
    				Assert.isNotNull(templateCustomPages);
    				for (IWizardPage customPage : templateCustomPages)
    					addPage(customPage);
    				creatorOfCustomPages = selectedTemplate;
    			}
				if (templateCustomPages.length > 0)
					return templateCustomPages[0];
    		}
    		// or go right to the first CDT page
    		return withCplusplusSupport() ? nestedWizard.getStartingPage() : null;
    	}
    	else if (indexInTemplateCustomPages != -1) {
    		// return next custom page, or first CDT page if we are on the last custom page
    		if (indexInTemplateCustomPages < templateCustomPages.length-1)
    			return templateCustomPages[indexInTemplateCustomPages+1];
    		else
    			return withCplusplusSupport() ? nestedWizard.getStartingPage() : null;
    	}
    	else {
    		return super.getNextPage(page);
    	}
    }
    
    
    @Override
    public boolean performFinish() {
        // if we are on the first page, the CDT wizard is not yet created and its perform finish 
        // will not be called, so we have to do it manually
        final boolean withCPlusPlus = withCplusplusSupport();
        IWizardPage lastPage = getContainer().getCurrentPage();
		if (lastPage == projectPage || lastPage == templatePage || ArrayUtils.contains(templateCustomPages, lastPage)) {
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
        final IProject project = projectPage.getProjectHandle();
        final IProjectTemplate template = templatePage.getSelectedTemplate();
        final Map<String, Object> vars = template.extractVariablesFromPages(templateCustomPages);
        WorkspaceModifyOperation op = new WorkspaceModifyOperation() {
            protected void execute(IProgressMonitor monitor) throws CoreException {
                // add OMNeT++ nature
                ProjectUtils.addOmnetppNature(project, monitor);

                // if C++ project, add a default .buildspec file (templates may overwrite it)
                if (withCPlusPlus)
                    BuildSpecification.createInitial(project).save();

                // apply template: this may create files, set project properties, configure the CDT project, etc.
                if (template != null)
                    template.configure(project, vars, monitor);
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

	protected List<IProjectTemplate> getTemplates() {
		List<IProjectTemplate> result = new ArrayList<IProjectTemplate>();
		
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

	protected List<IProjectTemplate> loadTemplatesFromWorkspace() {
		// check the "templates/project" subdirectory of each OMNeT++ project
		List<IProjectTemplate> result = new ArrayList<IProjectTemplate>();
		for (IProject project : ProjectUtils.getOmnetppProjects()) {
			IFolder rootFolder = project.getFolder(new Path("templates/project"));
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

	protected IProjectTemplate loadTemplateFrom(IFolder folder) throws CoreException {
		return new WorkspaceBasedProjectTemplate(folder);
	}

    
}


