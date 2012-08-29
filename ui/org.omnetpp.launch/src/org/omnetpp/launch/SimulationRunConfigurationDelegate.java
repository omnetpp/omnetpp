/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;


import java.net.ServerSocket;
import java.net.SocketException;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.simulation.SimulationEditorInput;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;


/**
 * Can launch a single or multiple simulation process. Understands OMNeT++ specific launch attributes.
 * see IOmnetppLaunchConstants.
 *
 * @author rhornig
 */
public class SimulationRunConfigurationDelegate extends LaunchConfigurationDelegate {

    public void launch(ILaunchConfiguration oldConfig, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {
        OmnetppLaunchUtils.updateLaunchConfigurationWithProgramAttributes(mode, launch);

        // we must use the updated configuration in 'launch' instead the original passed to us
        ILaunchConfiguration configuration = launch.getLaunchConfiguration();
        final String name = configuration.getName();

        if (monitor == null)
            monitor = new NullProgressMonitor();

        monitor.beginTask("Launching Simulation", 1);

        int runs[] = OmnetppLaunchUtils.parseRuns(configuration.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, ""),
                                                OmnetppLaunchUtils.getMaxNumberOfRuns(configuration));
        Assert.isTrue(runs != null && runs.length > 0);

        // show the debug view if option is checked
        if (configuration.getAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, false)) {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                    try {
                        workbenchPage.showView(IDebugUIConstants.ID_DEBUG_VIEW, null, IWorkbenchPage.VIEW_VISIBLE);
                    } catch (PartInitException e) {
                        LaunchPlugin.logError("Cannot initialize the Debug View", e);
                    }
                }
            });
        }

        String envirStr = StringUtils.defaultIfEmpty(configuration.getAttribute(IOmnetppLaunchConstants.OPP_USER_INTERFACE, "").trim(), IOmnetppLaunchConstants.UI_FALLBACKVALUE);
        boolean openSimulationEditor = envirStr.equals(IOmnetppLaunchConstants.UI_IDE);
        final int port = openSimulationEditor ? findFirstAvailableTcpPort(6000, 7000) : -1; //TODO let user specify a port manually, via the "-p" option

        int numProcesses = configuration.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        boolean reportProgress = StringUtils.contains(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, ""), "-u Cmdenv");
        // start a single or batched launch job
        Job job;
        if (runs.length == 1)
            job = new SimulationLauncherJob(configuration, launch, runs[0], reportProgress, port);
        else
            job = new BatchedSimulationLauncherJob(configuration, launch, runs, numProcesses);

        job.schedule();
        monitor.done();

        // open simulation front-end
        if (openSimulationEditor) {
            final Job launcherjob = job;
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    IWorkbenchPage workbenchPage = workbenchWindow.getActivePage();
                    IEditorInput input = new SimulationEditorInput(name, "localhost", port, launcherjob);
                    String SIMULATION_EDITOR_ID = "org.omnetpp.simulation.editors.SimulationEditor";
                    try {
                        IDE.openEditor(workbenchPage, input, SIMULATION_EDITOR_ID);
                    }
                    catch (PartInitException e) {
                        ErrorDialog.openError(workbenchWindow.getShell(), "Error", "Could not open animation window for the running simulation.", e.getStatus());
                        LaunchPlugin.logError(e);
                    }
                    //TODO offer switching to "Simulation" perspective!
                }
            });
        }
    }

    /**
     * Searches for the first free TCP port number on localhost, and returns it.
     * Returns -1 if no available TCP port was found.
     */
    protected int findFirstAvailableTcpPort(int startPort, int endPort) {
        for (int port = startPort; port <= endPort; port++) {
            try {
                ServerSocket sock = new ServerSocket(port);
                sock.close();
                return port;
            }
            catch (SocketException e) {
                // ignore, and go on to next port
            }
            catch (Exception e) {
                return -1; // unexpected error -- give up
            }
        }
        return -1;  // no available port found
    }

    @Override
    protected IProject[] getProjectsForProblemSearch(ILaunchConfiguration configuration, String mode) throws CoreException {
        // NOTE: we need to do this twice: here and in launch() which is kind of superfluous
        //       but it is unclear whether those two incoming configurations are the same or not
        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode);
        String projectName = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");

        if (StringUtils.isEmpty(projectName))
            return ProjectUtils.getOpenProjects();
        else {
            IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
            return ProjectUtils.getAllReferencedProjects(project, false, true);
        }
    }
}
