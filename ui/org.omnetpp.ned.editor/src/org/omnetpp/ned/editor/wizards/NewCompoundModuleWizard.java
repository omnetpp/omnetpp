package org.omnetpp.ned.editor.wizards;


/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NewCompoundModuleWizard extends AbstractNedFileWizard {

    public NewCompoundModuleWizard() {
        setWizardType("compoundmodule");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New Compound Module");
        getFirstPage().setTitle("New Compound Module");
    }
    
}
