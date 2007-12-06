package org.omnetpp.cdt.wizard.tmp;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionManager;
import org.eclipse.cdt.core.settings.model.extension.CConfigurationData;
import org.eclipse.cdt.managedbuilder.core.IBuilder;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.managedbuilder.internal.core.Configuration;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.internal.core.ManagedProject;
import org.eclipse.cdt.managedbuilder.internal.core.ToolChain;
import org.eclipse.cdt.managedbuilder.ui.wizards.CfgHolder;
import org.eclipse.cdt.ui.newui.UIMessages;
import org.eclipse.cdt.ui.templateengine.IWizardDataPage;
import org.eclipse.cdt.ui.templateengine.Template;
import org.eclipse.cdt.ui.templateengine.TemplateEngineUI;
import org.eclipse.cdt.ui.templateengine.TemplatesChoiceWizard;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.wizard.IWizardPage;

import org.omnetpp.ide.wizard.NewOmnetppProjectWizard.NewOmnetppProjectCreationPage;


public class OmnetCDTNewProjectWizard extends TemplatesChoiceWizard {

    class NewProjectPage extends NewOmnetppProjectCreationPage implements IWizardDataPage {
        
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

        public void setNextPage(IWizardPage next) {
            // TODO Auto-generated method stub
            
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
//        boolean result = true;
//        IProjectType pt = ManagedBuildManager.getProjectType(ManagedBuildManager.BUILD_ARTEFACT_TYPE_PROPERTY_EXE)
//        ManagedBuildManager.createManagedProject(resource, pt);
//        List configs = new ArrayList();
//        IConfiguration cfg = ManagedBuildManager.getExtensionConfiguration("cdt.managedbuild.config.gnu.mingw.exe.release");
//        configs.add(cfg);
//        getSelectedTemplate().getTemplateInfo().setConfigurations(configs);
//        boolean result = super.performFinish();
//        try {
//            cfg.getEditableBuilder().setManagedBuildOn(false);
//            String projectName = (String)getAllDataInNonTemplatePages().get("projectName");
//            ManagedBuildManager.saveBuildInfo(ResourcesPlugin.getWorkspace().getRoot().getProject(projectName), true);
//        }
//        catch (CoreException e) {
//            // TODO Auto-generated catch block
//            e.printStackTrace();
//        }

        String projectName = (String)getAllDataInNonTemplatePages().get("projectName");
        IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
//        try {
//            createProject(project, false, false);
//        }
//        catch (CoreException e) {
//            // TODO Auto-generated catch block
//            e.printStackTrace();
//        }
        //ICProjectDescription pd = CCorePlugin.getDefault().createProjectDescription(project, true, true);
        
        try {
            CCorePlugin.getDefault().createCDTProject(project.getDescription(), project, null);
        }
        catch (OperationCanceledException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        catch (CoreException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return true;
    }

    public void createProject(IProject project, boolean defaults, boolean onFinish)  throws CoreException {
        ICProjectDescriptionManager mngr = CoreModel.getDefault().getProjectDescriptionManager();
        ICProjectDescription des = mngr.createProjectDescription(project, false, !onFinish);
        ManagedBuildInfo info = ManagedBuildManager.createBuildInfo(project);
        ManagedProject mProj = new ManagedProject(des);
        info.setManagedProject(mProj);

//        cfgs = CfgHolder.unique(fConfigPage.getCfgItems(defaults));
//        cfgs = CfgHolder.reorder(cfgs);
            
        IConfiguration conf = ManagedBuildManager.getExtensionConfiguration("cdt.managedbuild.config.gnu.mingw.exe.release");
        CfgHolder cfgs[] = new CfgHolder[] {new CfgHolder(conf.getToolChain(),conf)};
        
        for (int i=0; i<cfgs.length; i++) {
            String s = (cfgs[i].getToolChain() == null) ? "0" : ((ToolChain)(cfgs[i].getToolChain())).getId();  //$NON-NLS-1$
            Configuration cfg = new Configuration(mProj, (ToolChain)cfgs[i].getToolChain(), ManagedBuildManager.calculateChildId(s, null), cfgs[i].getName());
            IBuilder bld = cfg.getEditableBuilder();
            if (bld != null) {
                if(bld.isInternalBuilder()){
                    IConfiguration prefCfg = ManagedBuildManager.getPreferenceConfiguration(false);
                    IBuilder prefBuilder = prefCfg.getBuilder();
                    cfg.changeBuilder(prefBuilder, ManagedBuildManager.calculateChildId(cfg.getId(), null), prefBuilder.getName());
                    bld = cfg.getEditableBuilder();
                    bld.setBuildPath(null);
                }
                bld.setManagedBuildOn(false);
            } else {
                System.out.println(UIMessages.getString("StdProjectTypeHandler.3")); //$NON-NLS-1$
            }
            cfg.setArtifactName(project.getName());
            CConfigurationData data = cfg.getConfigurationData();
            des.createConfiguration(ManagedBuildManager.CFG_DATA_PROVIDER_ID, data);
        }
        mngr.setProjectDescription(project, des);
    }
}
