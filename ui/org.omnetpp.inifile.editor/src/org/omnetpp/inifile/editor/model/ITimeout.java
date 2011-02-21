package org.omnetpp.inifile.editor.model;

/**
 * Long blocking computations request remaining time through this interface.
 * 
 * @author tomi
 */
public interface ITimeout {
    /**
     * Returns the remaining time for the computation in milliseconds.
     * Non-positive value indicates that the computation should be interrupted.
     */
    long getRemainingTime();
}
