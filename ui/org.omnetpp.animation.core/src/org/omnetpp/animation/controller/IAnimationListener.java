/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.controller;

/**
 * Notification interface for events and state changes during the animation.
 *
 * @author levy
 */
public interface IAnimationListener {
    /**
     * Notification sent to the listener when the animation has been started.
     */
    public void animationStarted();

    /**
     * Notification sent to the listener when the animation has been stopped.
     */
    public void animationStopped();

	/**
	 * Notification sent to the listener about a new animation position when the animation is running.
	 */
	public void animationPositionChanged(AnimationPosition animationPosition);
}
