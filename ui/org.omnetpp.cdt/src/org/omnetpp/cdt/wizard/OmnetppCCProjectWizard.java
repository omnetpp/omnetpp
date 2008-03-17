package org.omnetpp.cdt.wizard;

import java.util.ArrayList;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CProjectNature;
import org.eclipse.cdt.ui.CUIPlugin;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.wizards.CDTCommonProjectWizard;
import org.eclipse.cdt.ui.wizards.EntryDescriptor;
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
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

import org.omnetpp.cdt.cdtpatches.CDTMainWizardPage_Patched;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ide.wizard.NewOmnetppProjectWizard;


/**
 * Like OmnetppNewProjectWizard, but continues in a customized
 * "New CDT Project" Wizard.
 *
 * @author Andras
 */
public class OmnetppCCProjectWizard extends NewOmnetppProjectWizard implements INewWizard {

//    protected NewOmnetppCppProjectWizardPage projectPage;
    private CCProjectWizard nestedWizard;

    /**
     * Customizations:
     *   (1) hide project name / location because we have a separate page for that
     *   (2) filter the project types and templates shown
     */
    class CustomizedCDTMainPage extends CDTMainWizardPage_Patched {
        public CustomizedCDTMainPage() {
            super(CUIPlugin.getResourceString("CProjectWizard"));
            setTitle("Select Project Type");
            setDescription("Select type of project to be created");
        }

        @Override
        public void createControl(Composite parent) {
            super.createControl(parent);

            // hide project name and location -- we have a separate project page
            hideControl(projectNameField.getParent());
            hideControl((Control)ReflectionUtils.getFieldValue(locationArea, "locationLabel"));
            hideControl((Control)ReflectionUtils.getFieldValue(locationArea, "locationPathField"));
            hideControl((Control)ReflectionUtils.getFieldValue(locationArea, "browseButton"));
            hideControl((Control)ReflectionUtils.getFieldValue(locationArea, "useDefaultsButton"));
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
            projectNameField.setText(projectName);
            ((Button)ReflectionUtils.getFieldValue(locationArea, "useDefaultsButton")).setSelection(useDefaultLocation);
            if (!useDefaultLocation)
                ((Text)ReflectionUtils.getFieldValue(locationArea, "locationPathField")).setText(location.append(projectName.trim()).toString());
        }

        @Override
        protected ArrayList<EntryDescriptor> filterItems(ArrayList<EntryDescriptor> items) {
            ArrayList<EntryDescriptor> newItems = new ArrayList<EntryDescriptor>();
            for (EntryDescriptor entry : items) {
                if (entry.getId().startsWith("org.omnetpp"))
                    newItems.add(entry);
            }
            return newItems;
        }
    }

    class CCProjectWizard extends CDTCommonProjectWizard {

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
            } catch (CoreException e) {} //FIXME report error
            return prj;
        }
    }

    class NewOmnetppCppProjectCreationPage extends NewOmnetppProjectCreationPage {
        private Button supportCppButton;

        public NewOmnetppCppProjectCreationPage() {
            setDescription("Creates a Project that can handle OMNeT++ specific files and supports C++ development using CDT.");
        }

        @Override
        public void createControl(Composite parent) {
            super.createControl(parent);
            // add a new supports C++ development checkbox
            Composite comp = new Composite((Composite)getControl(), SWT.NONE);
            comp.setLayout(new GridLayout(1,false));
            comp.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            supportCppButton = new Button(comp ,SWT.CHECK);
            supportCppButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
            supportCppButton.setText("Support C++ Development");
            supportCppButton.setSelection(true);
            supportCppButton.addSelectionListener(new SelectionAdapter() {
                public void widgetSelected(SelectionEvent e) {
                    getWizard().getContainer().updateButtons();
                }
            });
        }

        public boolean supportsCpp() {
            return supportCppButton.getSelection();
        }

        @Override
        public IWizardPage getNextPage() {
            return supportsCpp() ? nestedWizard.getStartingPage() : null;
        }
    }

    @Override
    public void addPages() {
        projectPage = new NewOmnetppCppProjectCreationPage() ;
        addPage(projectPage);
        nestedWizard.addPages();
    }

    @Override
    public boolean performFinish() {
        // if we are on the first page the CDT wizard is not yet created and its perform finis will not be called
        // so we have to do it manually
        if (getContainer().getCurrentPage() == projectPage) {
            if (((NewOmnetppCppProjectCreationPage)projectPage).supportsCpp()) {
                // show it manually (and create) and
                getContainer().showPage(nestedWizard.getStartingPage());
                nestedWizard.performFinish();
            }
            else {
                // just call the plain OMNET++ wizard
                super.performFinish();
            }
        }
        // the OMNET nature is always added
        IProject project = projectPage.getProjectHandle();
        ProjectUtils.addOmnetppNature(project);

        return true;
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
        nestedWizard = new CCProjectWizard();
        nestedWizard.init(workbench, selection);
        nestedWizard.setContainer(getContainer());
    }
}


