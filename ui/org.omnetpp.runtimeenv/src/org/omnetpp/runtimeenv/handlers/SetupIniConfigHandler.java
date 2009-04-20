package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.SimulationManager;
import org.omnetpp.runtimeenv.SimulationManager.SimState;


/**
 * Handler for the "Setup Inifile Configuration" action
 * @author Andras
 */
public class SetupIniConfigHandler extends AbstractHandler {

	public SetupIniConfigHandler() {
	}

	//XXX same as in SetupNetwork -- factor out somewhere
	protected boolean checkRunning() {
		SimState state = Activator.getSimulationManager().getState();
		if (state==SimState.RUNNING) {
			MessageDialog.openInformation(null, "Simulation Running", "Sorry, you cannot do this while the simulation is running.");
			return true;
		}
		if (state==SimState.BUSY) {
			MessageDialog.openInformation(null, "Simulation Busy", "The simulation is waiting for external synchronization -- press STOP to interrupt it.");
			return true;
		}
		return false;
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
		if (checkRunning())
			return null;

		RunSelectionDialog dialog = new RunSelectionDialog(null); //XXX parent!
		//XXX set old values
		// set configname [opp_getactiveconfigname]
		// set runnumber  [opp_getactiverunnumber]
		if (dialog.open() == ListDialog.OK) {
			//XXX next stuff should be using a progress monitor...
			String configName = dialog.getConfigName();
			int runNumber = dialog.getRunNumber();
			SimulationManager simulationManager = Activator.getSimulationManager();
			simulationManager.newRun(configName, runNumber);
			cModule systemModule = cSimulation.getActiveSimulation().getSystemModule();
			if (systemModule != null)
				Activator.openInspector2(systemModule, true);
		}
		return null;
	}
}
