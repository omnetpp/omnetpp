package org.omnetpp.animation.providers;

import java.util.List;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.eventlog.engine.IEvent;

/**
 * Interface that provides animation primitives for the animation controller for a given animation position.
 */
public interface IAnimationPrimitiveProvider {
    /**
     * Sets the corresponding animation controller that will ask for animation primitives from this provider.
     */
    public void setAnimationController(AnimationController animationController);

    /**
     * Returns a list of animation primitives that are active at the given animation position.
     */
    // TODO: this should rather be something like
    // public List<IAnimationPrimitive> collectAnimationPrimitives(AnimationPosition animationPosition);
    public List<IAnimationPrimitive> collectAnimationPrimitivesForEvents(IEvent beginEvent, IEvent endEvent);
}
