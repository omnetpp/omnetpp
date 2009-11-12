package org.omnetpp.ned.editor.wizards;


/**
 * "New NED file" wizard
 * 
 * @author Andras
 */
public class NewSimulationWizard extends AbstractNedFileWizard {

    public NewSimulationWizard() {
        setWizardType("simulation");
    }

    @Override
    public void addPages() {
        super.addPages();
        setWindowTitle("New Simulation");
        getFirstPage().setTitle("New Simulation");
    }

}
