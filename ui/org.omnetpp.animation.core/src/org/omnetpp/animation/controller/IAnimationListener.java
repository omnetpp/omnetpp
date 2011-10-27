/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
     * Notifies the listener when the animation has been started.
     */
    public void animationStarted();

    /**
     * Notifies the listener when the animation has been stopped.
     */
    public void animationStopped();

	/**
	 * Notifies the listener about a new animation position when the animation is running.
	 */
	public void animationPositionChanged(AnimationPosition animationPosition);
}
