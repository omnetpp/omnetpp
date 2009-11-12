package org.omnetpp.ned.editor.wizards;


/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NewSimpleModuleWizard extends AbstractNedFileWizard {

    public NewSimpleModuleWizard() {
        setWizardType("simplemodule");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New Simple Module");
        getFirstPage().setTitle("New Simple Module");
    }
    
}
