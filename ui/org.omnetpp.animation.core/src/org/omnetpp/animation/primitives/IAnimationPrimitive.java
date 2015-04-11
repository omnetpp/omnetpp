/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;

/**
 * <p>
 * IAnimationPrimitive is an atomic animation element managed by its exclusive
 * AnimationController owner. An animation primitive may or may not be active
 * depending on the current animation position provided by its controller. Being
 * active means having effect on the simulation model and/or the animation
 * graphics.
 * </p>
 * <p>
 * The controller is responsible for notifying the animation primitive when it
 * becomes active or inactive based on how the current animation position
 * changes over time. The animation range specified by the animation primitive
 * is inclusive at the beginning and exclusive at the end.
 * </p>
 * <p>
 * The animation primitive life cycle is the following: <code>
 * construct (activate refreshAnimation* deactivate)* finalize
 * </code>
 * </p>
 *
 * @author levy
 */
public interface IAnimationPrimitive {
    /**
     * Returns the animation position when this animation primitive begins and
     * shall become active. Before the returned position the primitive will not
     * have any effect on the state of the animation. This end of the range is
     * inclusive.
     */
    public AnimationPosition getBeginAnimationPosition();

    /**
     * Returns the animation position when this animation primitive ends and
     * shall become inactive. After the returned position the primitive will not
     * have any effect on the state of the animation. This end of the range is
     * exclusive.
     */
    public AnimationPosition getEndAnimationPosition();

    /**
     * Returns the total duration of this animation primitive in terms of
     * simulation time. This function must return non-negative value.
     */
    public BigDecimal getSimulationTimeDuration();

    /**
     * Returns the total duration of this animation primitive in terms of
     * animation time. Note that it does not matter whether it is based on frame
     * relative or origin relative animation times. This function must return a
     * non-zero positive value.
     */
    public Double getAnimationTimeDuration();

    /**
     * Applies the changes to the model and the graphics. This method is called
     * by the controller when the model needs to be updated to a new animation
     * position where this animation primitive is active while it was not active
     * at the old animation position.
     */
    public void activate();

    /**
     * Reverses the changes to the model and the graphics. This method is called
     * by the controller when the model needs to be updated to a new animation
     * position where this animation primitive is not active while it was active
     * at the old animation position.
     */
    public void deactivate();

    /**
     * Updates graphics and figures according to the given animation position.
     * This method will be called only when the animation primitive is active.
     * even if the animation is currently not running. The method might be
     * called several times as the animation progresses (forward or backward)
     * between the begin and end animation positions of the animation primitive.
     */
    public void refreshAnimation(AnimationPosition animationPosition);
}
