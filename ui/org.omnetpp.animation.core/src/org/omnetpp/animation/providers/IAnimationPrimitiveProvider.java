/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.animation.providers;

import java.util.ArrayList;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.IAnimationPrimitive;

/**
 * Interface that provides active animation primitives for the animation
 * controller at the given animation position.
 *
 * @author levy
 */
public interface IAnimationPrimitiveProvider {
    /**
     * Sets the corresponding animation controller that will ask for animation
     * primitives from this provider.
     */
    public void setAnimationController(AnimationController animationController);

    /**
     * Returns the list of animation primitives that are active at the given
     * animation position.
     */
    public ArrayList<IAnimationPrimitive> loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition);
}
