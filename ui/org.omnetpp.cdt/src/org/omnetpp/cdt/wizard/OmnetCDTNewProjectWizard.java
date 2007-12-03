package org.omnetpp.cdt.wizard;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.ui.templateengine.IWizardDataPage;
import org.eclipse.cdt.ui.templateengine.Template;
import org.eclipse.cdt.ui.templateengine.TemplateEngineUI;
import org.eclipse.cdt.ui.templateengine.TemplatesChoiceWizard;
import org.eclipse.jface.viewers.Viewer;

import org.omnetpp.ide.wizard.NewOmnetppProjectWizardPage;


public class OmnetCDTNewProjectWizard extends TemplatesChoiceWizard {

    class NewProjectPage extends NewOmnetppProjectWizardPage implements IWizardDataPage {
        public Map getPageData() {
                Map data = new HashMap();
                String projName = super.getProjectName().trim();
                String basename = projName;
                data.put("projectName", projName); //$NON-NLS-1$
                data.put("baseName", basename); //$NON-NLS-1$
                data.put("baseNameUpper", basename.toUpperCase() ); //$NON-NLS-1$
                data.put("baseNameLower", basename.toLowerCase() ); //$NON-NLS-1$
                data.put("location", super.getLocationPath().toPortableString()); //$NON-NLS-1$
                return data;
        }
    }

    IWizardDataPage[] beforePages = new IWizardDataPage[] { new NewProjectPage() };
    
    public OmnetCDTNewProjectWizard() {
    }

    @Override
    protected IWizardDataPage[] getPagesAfterTemplatePages() {
        return new IWizardDataPage[0];
    }

    @Override
    protected IWizardDataPage[] getPagesAfterTemplateSelection() {
        return new IWizardDataPage[0];
    }

    @Override
    protected IWizardDataPage[] getPagesBeforeTemplatePages() {
        return beforePages;
    }

    public String getDescription(Object object) {
        if (object instanceof Template)
            return ((Template)object).getDescription();
        return "";
    }

    public Template[] getTemplates() {
        return TemplateEngineUI.getDefault().getTemplates();
    }

    public boolean showTemplatesInTreeView() {
        return false;
    }

    public Object[] getChildren(Object parentElement) {
        return null;
    }

    public Object getParent(Object element) {
        return null;
    }

    public boolean hasChildren(Object element) {
        return false;
    }

    public Object[] getElements(Object inputElement) {
        return getTemplates();
    }

    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        // TODO Auto-generated method stub
        
    }
    
    @Override
    public boolean performFinish() {
        List configs = new ArrayList();
        configs.add(ManagedBuildManager.getExtensionConfiguration("cdt.managedbuild.config.gnu.mingw.exe.release"));
        getSelectedTemplate().getTemplateInfo().setConfigurations(configs);
        return super.performFinish();
    }

}
