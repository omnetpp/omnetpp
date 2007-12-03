package org.omnetpp.cdt.wizard;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.ui.wizards.CDTCommonProjectWizard;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;

public class OmnetppCCProjectWizard extends CDTCommonProjectWizard {

    public OmnetppCCProjectWizard() {
        super("OMNEST/OMNeT++ Project", "Create an OMNEST/OMNeT++ project which supports c++ development"); 
    }
    
    public String[] getNatures() {
        return new String[] { CCProjectNature.CC_NATURE_ID };
    }
    
    protected IProject continueCreation(IProject prj) {
        try {
            CCProjectNature.addCCNature(prj, new NullProgressMonitor());
        } catch (CoreException e) {}
        return prj;
    }
    
    @Override
    public void addPages() {
        fMainPage= new OmnetppCDTMainWizardPage("PageName");
        fMainPage.setTitle("title");
        fMainPage.setDescription("desc");
        addPage(fMainPage);
    }
    
    @Override
    public boolean performFinish() {
        
        return super.performFinish();
    }

}
