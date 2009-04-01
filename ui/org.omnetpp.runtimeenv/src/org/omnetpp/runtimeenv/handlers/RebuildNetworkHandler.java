package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.omnetpp.runtimeenv.Activator;

/**
 * Handler for the "Rebuild Network" action
 * @author Andras
 */
public class RebuildNetworkHandler extends AbstractHandler {
	public RebuildNetworkHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
        Activator.getSimulationManager().rebuildNetwork();
        return null;
	}
}
