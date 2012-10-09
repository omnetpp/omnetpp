package org.omnetpp.simulation.liveanimation;

/**
 *
 * @author Andras
 */
public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {

    public abstract boolean updateFor(double time);

}
