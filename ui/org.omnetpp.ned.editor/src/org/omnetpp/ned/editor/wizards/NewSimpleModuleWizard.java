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
        setWindowTitle(isImporting() ? "Import Simple Module" : "New Simple Module");
        getFirstPage().setTitle(isImporting() ? "Import Simple Module" : "New Simple Module");
        getFirstPage().setDescription("Choose NED file (generated C++ header and source files will be named similarly)");
    }

}
