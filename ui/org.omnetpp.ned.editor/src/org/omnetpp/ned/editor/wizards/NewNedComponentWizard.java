package org.omnetpp.ned.editor.wizards;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;

/**
 */
public class NewNedComponentWizard extends Wizard implements INewWizard {
	protected NewNEDComponentWizardPage page = null;
    protected IStructuredSelection selection;
    protected IWorkbench workbench;
    protected NewNEDComponentWizardPage.Type type;

    public static class NewSimpleModuleWizard extends NewNedComponentWizard {
    	public NewSimpleModuleWizard() {
    		super(NewNEDComponentWizardPage.Type.SIMPLE);
    		setWindowTitle("New Simple Module");
		}
    }

    public static class NewCompoundModuleWizard extends NewNedComponentWizard {
    	public NewCompoundModuleWizard() {
    		super(NewNEDComponentWizardPage.Type.COMPOUND);
    		setWindowTitle("New Compound Module");
		}
    }
    
    public static class NewNetworkWizard extends NewNedComponentWizard {
    	public NewNetworkWizard() {
    		super(NewNEDComponentWizardPage.Type.NETWORK);
    		setWindowTitle("New Network");
		}
    }
    
    protected NewNedComponentWizard(NewNEDComponentWizardPage.Type type) {
    	this.type = type;
	}
    
    @Override
    public void addPages() {
        page = new NewNEDComponentWizardPage(workbench, selection, type);
        addPage(page);
    }

    public void init(IWorkbench aWorkbench, IStructuredSelection currentSelection) {
        workbench = aWorkbench;
        selection = currentSelection;
    }

    @Override
    public boolean performFinish() {
        return page.finish();
    }

}