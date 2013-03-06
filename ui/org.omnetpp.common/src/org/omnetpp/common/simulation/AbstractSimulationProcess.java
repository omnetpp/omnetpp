package org.omnetpp.common.simulation;

import org.eclipse.core.runtime.ListenerList;


/**
 * Base class for ISimulationProcess implementations.
 *
 * @author Andras
 */
public abstract class AbstractSimulationProcess implements ISimulationProcess {
    private ListenerList listeners = new ListenerList();

    @Override
    public void addListener(ISimulationProcessListener listener) {
        listeners.add(listener);
    }

    @Override
    public void removeListener(ISimulationProcessListener listener) {
        listeners.remove(listener);
    }

    protected void fireSimulationProcessSuspended() {
        for (Object o : listeners.getListeners())
            ((ISimulationProcessListener) o).simulationProcessSuspended();
    }

    protected void fireSimulationProcessResumed() {
        for (Object o : listeners.getListeners())
            ((ISimulationProcessListener) o).simulationProcessResumed();
    }

    protected void fireSimulationProcessTerminated() {
        for (Object o : listeners.getListeners())
            ((ISimulationProcessListener) o).simulationProcessTerminated();
    }
}
