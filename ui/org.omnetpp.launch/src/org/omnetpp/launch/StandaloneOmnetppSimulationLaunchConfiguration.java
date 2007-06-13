package org.omnetpp.launch;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;

public class StandaloneOmnetppSimulationLaunchConfiguration extends LaunchConfigurationDelegate {

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
//        DebugPlugin.exec(cmdLine, workingDirectory)
        System.out.println("Simulation launched");

    }

}
