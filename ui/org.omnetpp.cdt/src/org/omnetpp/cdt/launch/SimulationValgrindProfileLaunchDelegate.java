/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.launch;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.linuxtools.internal.valgrind.launch.ValgrindLaunchConfigurationDelegate;
import org.omnetpp.launch.IOmnetppLaunchConstants;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * A proxy launch delegate that uses opp_run from the bin directory if no executable was specified.
 */
@SuppressWarnings("restriction")
public class SimulationValgrindProfileLaunchDelegate extends ValgrindLaunchConfigurationDelegate {
    @Override
    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {
        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode, true);
        OmnetppLaunchUtils.replaceConfigurationInLaunch(launch, configuration);

        super.launch(launch.getLaunchConfiguration(), mode, launch, monitor);
    }

    @Override
    public boolean buildForLaunch(ILaunchConfiguration config, String mode, IProgressMonitor monitor) throws CoreException {
        SubMonitor localmonitor = SubMonitor.convert(monitor, "", 1); //$NON-NLS-1$
        try {
            IProject project = OmnetppLaunchUtils.getMappedProject(config);
            if (project == null)
                return false;

            int buildBeforeValue = config.getAttribute(IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH, IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH_DEPENDENCIES);
            IProject[] projects = (buildBeforeValue == IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH_PROJECT_ONLY) ? new IProject[] { project } :
                       (buildBeforeValue == IOmnetppLaunchConstants.OPP_BUILD_BEFORE_LAUNCH_DEPENDENCIES) ? computeReferencedBuildOrder(new IProject[] { project }) : null;
            if (projects == null)
                return false;

            OmnetppLaunchUtils.setActiveProjectConfigurationsIfNeeded(config, mode, projects);

            buildProjects(projects, localmonitor.newChild(1));
            return false;
        } finally {
            localmonitor.done();
        }
    }
}
