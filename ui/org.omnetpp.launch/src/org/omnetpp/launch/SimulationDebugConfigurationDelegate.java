package org.omnetpp.launch;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.ILaunchConfigurationDelegate;

/**
 * Starts the simulation in debug mode using CDT
 */
public class SimulationDebugConfigurationDelegate implements ILaunchConfigurationDelegate {

    public SimulationDebugConfigurationDelegate() {
        // TODO Auto-generated constructor stub
    }

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
        // TODO Auto-generated method stub

    }

}
