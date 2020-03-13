package org.omnetpp.common.simulation;

import org.eclipse.core.runtime.ListenerList;


/**
 * Base class for ISimulationProcess implementations.
 *
 * @author Andras
 */
public abstract class AbstractSimulationProcess implements ISimulationProcess {
    private ListenerList<ISimulationProcessListener> listeners = new ListenerList<>();

    @Override
    public void addListener(ISimulationProcessListener listener) {
        listeners.add(listener);
    }

    @Override
    public void removeListener(ISimulationProcessListener listener) {
        listeners.remove(listener);
    }

    protected void fireSimulationProcessSuspended() {
        for (ISimulationProcessListener listener : listeners)
            listener.simulationProcessSuspended();
    }

    protected void fireSimulationProcessResumed() {
        for (ISimulationProcessListener listener : listeners)
            listener.simulationProcessResumed();
    }

    protected void fireSimulationProcessTerminated() {
        for (ISimulationProcessListener listener : listeners)
            listener.simulationProcessTerminated();
    }
}
