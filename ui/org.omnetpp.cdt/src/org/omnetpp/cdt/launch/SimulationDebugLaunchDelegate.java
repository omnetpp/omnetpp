package org.omnetpp.cdt.launch;

import org.eclipse.cdt.launch.internal.LocalCDILaunchDelegate;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.launch.IOmnetppLaunchConstants;

/**
 * A proxy launch delegate that uses opp_run from the bin directory if no executable was specified.
 */
@SuppressWarnings("restriction")
public class SimulationDebugLaunchDelegate extends LocalCDILaunchDelegate {
	@Override
	public void launch(ILaunchConfiguration config, String mode,
			ILaunch launch, IProgressMonitor monitor) throws CoreException {
		// check if program name is not provided in this case we should use opp_run as the executable
		if (StringUtils.isEmpty(config.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, ""))) {
			ILaunchConfigurationWorkingCopy cfg = config.copy("opp_run temporary configuration");
			String oppRunName = OmnetppMainPlugin.getOmnetppBinDir()+"/opp_run"; 
			if(Platform.getOS().equals(Platform.OS_WIN32))
				oppRunName += ".exe";
				
			cfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, oppRunName);
			// use the first librarie's project as main project
			cfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, cfg.getAttribute(IOmnetppLaunchConstants.ATTR_SHARED_LIB_PROJECT_NAME, ""));
			config = cfg;
		}
		
		super.launch(config, mode, launch, monitor);
	}
}
