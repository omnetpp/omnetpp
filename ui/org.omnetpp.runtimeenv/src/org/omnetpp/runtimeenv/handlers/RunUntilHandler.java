package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.omnetpp.runtimeenv.Activator;

public class RunUntilHandler extends AbstractHandler {
	public RunUntilHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
        Activator.getSimulationManager().runUntil();
        return null;
	}
}
