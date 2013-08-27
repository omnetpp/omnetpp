package org.omnetpp.ide.installer;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;

public class InstallSimulationModelsHandler extends AbstractHandler {
    @Override
    public Object execute(ExecutionEvent event) throws ExecutionException {
        new InstallSimulationModelsDialog(null).open();
        return null;
    }
}
