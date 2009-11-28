package org.omnetpp.ned.editor.wizards;


/**
 * "New Compound Module" wizard
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
