package org.omnetpp.common.simulation;


/**
 * Provides the ability to subscribe to process suspend/resume events coming
 * from the debugger framework.
 *
 * @author Andras
 */
public interface ISuspendResume {
    public void addSuspendResumeListener(ISuspendResumeListener listener);
    public void removeSuspendResumeListener(ISuspendResumeListener listener);
}
