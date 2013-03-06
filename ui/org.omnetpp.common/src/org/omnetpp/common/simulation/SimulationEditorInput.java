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

    // only if simulation was started via a launch configuration (and not attached to)
    private String launchConfigurationName;

    public SimulationEditorInput(String name, String hostName, int portNumber) {
        this(name, hostName, portNumber, null, null);
    }

    public SimulationEditorInput(String name, String hostName, int portNumber, ISimulationProcess simulationProcess, String launchConfigurationName) {
        this.name = name;
        this.hostName = hostName;
        this.portNumber = portNumber;
        this.simulationProcess = simulationProcess;
        this.launchConfigurationName = launchConfigurationName;
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

    public ISimulationProcess getSimulationProcess() {
        return simulationProcess;
    }

    public String getLaunchConfigurationName() {
        return launchConfigurationName;
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
