package org.omnetpp.runtimeenv.handlers;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.omnetpp.runtimeenv.Activator;

public class ExpressRunHandler extends AbstractHandler {
	public ExpressRunHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
        Activator.getSimulationManager().expressRun();
        return null;
	}
}
