package org.omnetpp.common.simulation;

import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;

/**
 * IEditorInput for launching SimulationEditor, the simulation front-end.
 *
 * @author Andras
 */
public class SimulationEditorInput implements IEditorInput {
    private String name;
    private String hostName = "localhost";
    private int portNumber = 4242;
    private Job launcherJob;


    public SimulationEditorInput(String name, String hostName, int portNumber, Job launcherJob) {
        this.name = name;
        this.hostName = hostName;
        this.portNumber = portNumber;
        this.launcherJob = launcherJob;
    }

    //@Override
    public boolean exists() {
        return false;
    }

    //@Override
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    //@Override
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

    public Job getLauncherJob() {
        return launcherJob;
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
