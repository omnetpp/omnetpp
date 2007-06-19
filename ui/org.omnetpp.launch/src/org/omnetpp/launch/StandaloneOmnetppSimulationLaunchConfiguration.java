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
        System.out.println("Simulation launched. ");
        String wdAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_WORKING_DIRECTORY, "");
        String projAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");
        String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
        String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
        System.out.println("WD: "+wdAttr);
        System.out.println("Project: "+projAttr);
        System.out.println("Program: "+progAttr);
        System.out.println("Arguments: "+argAttr);
        System.out.println("========== exetuting ==========");

    }

}
