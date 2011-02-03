/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;

/**
 * IAnimationPrimitive is an atomic animation element managed by an AnimationController. An animation primitive
 * may or may not be active depending on the current animation position provided by its controller. Being active
 * means having effect on the simulation model state and/or the animation graphics state.
 *
 * The controller is responsible for notifying the animation primitive when it becomes active or inactive based
 * on the current and new animation positions.
 *
 * The animation primitive life cycle is the following: construct (activate, refreshAnimation*, deactivate)* finalize
 */
public interface IAnimationPrimitive {
    /**
     * Returns the animation position when this animation primitive begins and shall become active.
     * Before the returned position the primitive will not have any effect on the state of the animation.
     */
    public AnimationPosition getBeginAnimationPosition();

    /**
     * Returns the animation position when this animation primitive ends and shall become inactive.
     * After the returned position the primitive will not have any effect on the state of the animation.
     */
    public AnimationPosition getEndAnimationPosition();

    /**
     * Returns the total duration of this animation primitive in terms of simulation time.
     */
    public BigDecimal getSimulationTimeDuration();

    /**
     * Returns the total duration of this animation primitive in terms of animation time.
     */
    public Double getAnimationTimeDuration();

	/**
	 * Applies the changes to the model. This method is called by the controller when the model needs to be
	 * updated to an animation position where this animation primitive is active while it was not active at the
	 * current animation position.
	 */
	public void activate();

	/**
	 * Reverses the changes to the model. This method is called by the controller when the model needs to be
	 * updated to an animation position where this animation primitive is not active while it was active at the
	 * current animation position.
	 */
	public void deactivate();

	/**
	 * Updates graphics and figures according to the given animation position. This method will be called only
	 * when the animation primitive is active. The method will be called several times as the animation progresses
	 * (forward or backward) between the begin and end animation positions.
	 */
	public void refreshAnimation(AnimationPosition animationPosition);
}
