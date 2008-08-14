package org.omnetpp.cdt.launch;

import org.eclipse.cdt.launch.internal.LocalCDILaunchDelegate;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * A proxy launch delegate that uses opp_run from the bin directory if no executable was specified.
 */
@SuppressWarnings("restriction")
public class SimulationDebugLaunchDelegate extends LocalCDILaunchDelegate {
	@Override
	public void launch(ILaunchConfiguration config, String mode,
			ILaunch launch, IProgressMonitor monitor) throws CoreException {
		// check if program name is not provided in this case we should use opp_run as the executable
    	config = OmnetppLaunchUtils.convertLaunchConfig(config, mode);
		
		super.launch(config, mode, launch, monitor);
	}
}
