package org.omnetpp.ned.editor.wizards;


/**
 * "New Network" wizard
 *
 * @author Andras
 */
public class NewNetworkWizard extends AbstractNedFileWizard {

    public NewNetworkWizard() {
        setWizardType("network");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New Network");
        getFirstPage().setTitle("New Network");
    }

}
