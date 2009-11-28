package org.omnetpp.ned.editor.wizards;


/**
 * "New Simple Module" wizard
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
        getFirstPage().setDescription("Choose NED file (generated C++ header and source files will be named similarly)");
    }

}
