package org.omnetpp.common.simulation;

/**
 * For notifying the simulation front-end that the simulation process has been suspended
 * in the debugger or resumed execution.
 *
 * @author andras
 */
public interface ISuspendResumeListener {
	void simulationProcessSuspended();
	void simulationProcessResumed();
}
