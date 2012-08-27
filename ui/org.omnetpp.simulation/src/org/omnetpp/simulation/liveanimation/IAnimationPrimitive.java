package org.omnetpp.simulation.liveanimation;

/**
 * 
 * @author Andras
 */
public interface IAnimationPrimitive {

    /**
     * For time, zero means the beginning of the currently animated event, and is
     * measured in real-time seconds (provided animation speed is set to 1.0; if not,
     * then the time argument will be scaled).
     * 
     * @return true if more animation is needed by this primitive
     */
    boolean updateFor(double time);

}
