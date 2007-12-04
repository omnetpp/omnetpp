package org.omnetpp.cdt.wizard;

import java.util.ArrayList;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CProjectNature;
import org.eclipse.cdt.ui.CUIPlugin;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.wizards.CDTCommonProjectWizard;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.cdtpatches.CDTMainWizardPage_Patched;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ide.wizard.NewOmnetppProjectWizardPage;


/**
 * Like CDT's CCProjectWizard, but a bit customized for our purposes.
 * 
 * @author Andras
 */
public class OmnetppCCProjectWizard extends CDTCommonProjectWizard {
    /**
     * Customizations: 
     *   (1) hide project name / location because we have a separate page for that
     *   (2) filter the project types and templates shown
     */
    protected class CustomizedCDTMainPage extends CDTMainWizardPage_Patched {
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
        protected ArrayList filterItems(ArrayList items) {
            //TODO: show only OMNeT++-related templates
            return super.filterItems(items);
        }
    }

    protected NewOmnetppProjectWizardPage projectPage;

    public OmnetppCCProjectWizard() {
        super(UIMessages.getString("NewModelProjectWizard.2"), UIMessages.getString("NewModelProjectWizard.3")); //$NON-NLS-1$ //$NON-NLS-2$
    }
    
    @Override
    public void addPages() {
        projectPage = new NewOmnetppProjectWizardPage();
        addPage(projectPage);

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

    @Override
    public boolean performFinish() {
        boolean ok = super.performFinish();
        if (ok) {
            getProject(false);  //XXX "get" function with side effect?
            //TODO: configure project for OMNeT++
        }
        return ok;
    }
}
