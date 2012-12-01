package org.omnetpp.simulation.actions;

import org.eclipse.jface.action.IAction;
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

            getSimulationController().setCancelJobOnDispose(isChecked());
        }
        finally {
            updateState();
        }
    }

    @Override
    public void updateState() {
        setEnabled(getSimulationController().canCancelLaunch());
        setChecked(getSimulationController().getCancelJobOnDispose());
    }
}
