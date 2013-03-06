package org.omnetpp.common.simulation;



/**
 * Allows listening on the state of the simulation process. In addition
 * to listening on termination of the process, it also provides the ability 
 * to listen on process suspend/resume events coming from the debugger framework.
 *
 * @author Andras
 */
public interface ISimulationProcess {
    public void addListener(ISimulationProcessListener listener);
    public void removeListener(ISimulationProcessListener listener);
    public boolean canCancel();
    public void cancel();
    public boolean isSuspended();
    public boolean isTerminated();
}
