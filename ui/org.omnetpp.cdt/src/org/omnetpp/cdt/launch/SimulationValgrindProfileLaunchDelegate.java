/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.launch;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.linuxtools.internal.valgrind.launch.ValgrindLaunchConfigurationDelegate;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * A proxy launch delegate that uses opp_run from the bin directory if no executable was specified.
 */
@SuppressWarnings("restriction")
public class SimulationValgrindProfileLaunchDelegate extends ValgrindLaunchConfigurationDelegate {
    @Override
    public void launch(ILaunchConfiguration config, String mode,
            ILaunch launch, IProgressMonitor monitor) throws CoreException {
        OmnetppLaunchUtils.updateLaunchConfigurationWithProgramAttributes(mode, launch);
        // we must use the updated configuration in 'launch' instead the original passed to us
        super.launch(launch.getLaunchConfiguration(), mode, launch, monitor);
    }
}
