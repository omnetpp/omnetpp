package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.editors.SimulationEditorContributor;

/**
 * The "Kill simulation on editor close" action.
 *
 * @author Andras
 */
public class LinkWithSimulationAction extends AbstractSimulationActionDelegate {
    @Override
    protected void registerInContributor(IAction thisAction) {
        SimulationEditorContributor.linkWithSimulationAction = thisAction;
    }

    @Override
    public void run(IAction action) {
        try {
            if (!haveSimulationProcess())
                return;

            getSimulationController().getSimulation().setCancelJobOnDispose(isChecked());
        }
        finally {
            updateState();
        }
    }

    @Override
    public void updateState() {
        Simulation simulation = getSimulationController().getSimulation();
        setEnabled(simulation.canCancelLaunch());
        setChecked(simulation.getCancelJobOnDispose());
    }
}
