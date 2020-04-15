package org.omnetpp.cdt.wizard;

import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CProjectNature;
import org.eclipse.cdt.ui.CUIPlugin;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.wizards.CDTCommonProjectWizard;
import org.eclipse.cdt.ui.wizards.CDTMainWizardPage;
import org.eclipse.cdt.ui.wizards.EntryDescriptor;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.eclipse.ui.internal.ide.dialogs.ProjectContentsLocationArea;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.BuildSpecification;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplate;
import org.omnetpp.common.wizard.ICustomWizardPage;
import org.omnetpp.common.wizard.TemplateSelectionPage;
import org.omnetpp.common.wizard.XSWTDataBinding;
import org.omnetpp.common.wizards.NewOmnetppProjectWizard;
import org.osgi.framework.Bundle;

/**
 * "New OMNeT++ Project" wizard, with C++ support. After the template pages
 * (IContentTemplate), it continues in a customized "New CDT Project" Wizard.
 *
 * @author Andras
 */
@SuppressWarnings("restriction")
public class OmnetppCCProjectWizard extends NewOmnetppProjectWizard {
    private CCProjectWizard nestedWizard;

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
            ((Button)ReflectionUtils.getFieldValue(this, "showSup")).setSelection(true);
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
            String projectName = getProjectCreationPage().getProjectName();
            boolean useDefaultLocation = getProjectCreationPage().useDefaults();
            IPath location = getProjectCreationPage().getLocationPath();
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

        @SuppressWarnings("rawtypes")
        @Override
        public List<EntryDescriptor> filterItems(List items) {
            ArrayList<EntryDescriptor> newItems = new ArrayList<EntryDescriptor>();
            for (Object o : items) {
                EntryDescriptor entry = (EntryDescriptor)o;
                if (entry.getId().startsWith("org.omnetpp") ) {
                    entry.setParentId(null); // fix: CDTMainWIzardPage.updateData requires the parent ("Others") to be in the result list as well. We rather move the item to the top level.
                    newItems.add(entry);
                }
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

    public OmnetppCCProjectWizard() {
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
        super.addPages();
        nestedWizard.addPages();  // note: this actually adds pages to the *nested* wizard, not this one!
    }

    @Override
    protected TemplateSelectionPage createTemplateSelectionPage() {
        // we have a special template type and special template filtering, so
        // need to override a few methods in TemplateSelectionPage
        return new TemplateSelectionPage(getWizardType(), true) {
            @Override
            protected IContentTemplate loadTemplateFromWorkspace(IFolder folder) throws CoreException {
                return new FileBasedProjectTemplate(folder);
            }

            @Override
            protected IContentTemplate loadTemplateFromURL(URL templateUrl, Bundle bundleOfTemplate) throws CoreException {
                return new FileBasedProjectTemplate(templateUrl, bundleOfTemplate);
            }

            @Override
            protected boolean isSuitableTemplate(IContentTemplate template) {
                if (!withCplusplusSupport()) {
                    // for projects without C++, don't show templates that require C++
                    String reqCppSetting = template.getTemplateProperty(FileBasedProjectTemplate.PROP_REQUIRESCPLUSPLUS);
                    boolean requiresCPluspPlus =
                        (!StringUtils.isEmpty(reqCppSetting) && XSWTDataBinding.toBoolean(reqCppSetting)) ||
                        !StringUtils.isEmpty(template.getTemplateProperty(FileBasedProjectTemplate.PROP_SOURCEFOLDERS)) ||
                        !StringUtils.isEmpty(template.getTemplateProperty(FileBasedProjectTemplate.PROP_MAKEMAKEOPTIONS));
                    if (requiresCPluspPlus)
                        return false;
                }
                return super.isSuitableTemplate(template);
            }
        };
    }

    @Override
    protected WizardNewProjectCreationPage createProjectCreationPage() {
        WizardNewProjectCreationPage page = new NewOmnetppCppProjectCreationPage(); // custom one, with the "[] support C++ development" checkbox
        page.setTitle(isImporting() ? "Import into OMNeT++ Project" : "New OMNeT++ Project");
        setWindowTitle(isImporting() ? "Import into OMNeT++ Project" : "New OMNeT++ Project");
        return page;
    }

    @Override
    protected IWizardPage getFirstExtraPage() {
        return withCplusplusSupport() ? nestedWizard.getStartingPage() : null;
    }

    @Override
    protected CreationContext createContext(IContentTemplate selectedTemplate, IContainer folder) {
        CreationContext context = super.createContext(selectedTemplate, folder);
        context.getVariables().put("withCplusplusSupport", withCplusplusSupport());
        return context;
    }

    public boolean withCplusplusSupport() {
        return ((NewOmnetppCppProjectCreationPage)getProjectCreationPage()).withCplusplusSupport();
    }

    @Override
    public boolean performFinish() {
        // Note: this is a small hack to enforce that the nested CC wizard gets invoked
        // first (to create the project), and then us, regardless on which page the user
        // clicked Finish. This code heavily relies on the particular implementation of
        // WizardDialog.finishPressed(), and may need to be revised if that changes.
        IWizardPage finishingPage = getContainer().getCurrentPage();
        if (withCplusplusSupport() && finishingPage.getWizard() == this) {
            if (finishingPage instanceof ICustomWizardPage)
                ((ICustomWizardPage)finishingPage).extractPageContent(getContext()); // save page content first
            getContainer().showPage(nestedWizard.getStartingPage());
            return nestedWizard.performFinish();
        }
        return super.performFinish();
    }

    @Override
    protected boolean createNewProject() {
        if (!withCplusplusSupport()) {
            // just delegate to the C++-less New OMNeT++ Project wizard
            return super.createNewProject();
        }
        else {
            // add omnetpp nature, initial buildspec, etc.
            IProject project = getProjectCreationPage().getProjectHandle();
            try {
                // add the project nature after now, after project creation, so that builders
                // get properly configured (Project.create() doesn't do it).
                ProjectUtils.addOmnetppNature(project, null);

                // create initial buildspec file. it may get overwritten by the template
                BuildSpecification.createInitial(project).save();
            }
            catch (CoreException e) {
                Activator.logError(e);
            }
            return true;
        }
    }

}
