package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.omnetpp.runtimeenv.Activator;


/**
 * Handler for the "Stop Simulation" action
 * @author Andras
 */
public class StopHandler extends AbstractHandler {
	public StopHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
        Activator.getSimulationManager().stop();
        return null;
	}
}
