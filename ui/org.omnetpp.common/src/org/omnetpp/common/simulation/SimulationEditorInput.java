package org.omnetpp.common.simulation;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

/**
 * IEditorInput for launching SimulationEditor, the simulation front-end.
 *
 * @author Andras
 */
public class SimulationEditorInput implements IEditorInput {
    // mandatory fields
    private String name; // display name
    private String hostName;
    private int portNumber;
    private ISimulationProcess simulationProcess;
    private boolean cancelJobOnDispose;

    // only if simulation was started via a launch configuration (and not attached to)
    private String launchConfigurationName;

    /**
     * Constructor typically used when attaching to a process.
     */
    public SimulationEditorInput(String name, String hostName, int portNumber) {
        this(name, hostName, portNumber, null, null, false);
    }

    /**
     * Constructor typically used when simulation was launched as a Run or Debug session.
     */
    public SimulationEditorInput(String name, String hostName, int portNumber, ISimulationProcess simulationProcess, String launchConfigurationName, boolean cancelJobOnDispose) {
        this.name = name;
        this.hostName = hostName;
        this.portNumber = portNumber;
        this.simulationProcess = simulationProcess;
        this.launchConfigurationName = launchConfigurationName;
        this.cancelJobOnDispose = cancelJobOnDispose;
    }

    @Override
    public boolean exists() {
        return false;
    }

    @Override
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    @Override
    public String getName() {
        return name;
    }

    /**
     * Host where the simulation runs (and accepts HTTP requests)
     */
    public String getHostName() {
        return hostName;
    }

    /**
     * Port where the simulation listens to HTTP requests.
     */
    public int getPortNumber() {
        return portNumber;
    }

    /**
     * Allows listening on simulation process termination / suspend / resume events.
     */
    public ISimulationProcess getSimulationProcess() {
        return simulationProcess;
    }

    /**
     * Name of the launch configuration used to start the simulation; only informational
     * (currently used as resource key for storing the editor state).
     */
    public String getLaunchConfigurationName() {
        return launchConfigurationName;
    }

    /**
     * Whether to kill the simulation process when the simulation front-end 
     * (the editor) is closed.
     */
    public boolean getCancelJobOnDispose() {
        return cancelJobOnDispose;
    }

    public IPersistableElement getPersistable() {
        return null;
    }

    public String getToolTipText() {
        return "";
    }

    public Object getAdapter(@SuppressWarnings("rawtypes") Class adapter) {
        return null;
    }
}
