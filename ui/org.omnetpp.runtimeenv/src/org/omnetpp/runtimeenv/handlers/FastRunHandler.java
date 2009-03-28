package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.omnetpp.runtimeenv.Activator;

public class FastRunHandler extends AbstractHandler {
	public FastRunHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
        Activator.getSimulationManager().fastRun();
        return null;
	}
}
