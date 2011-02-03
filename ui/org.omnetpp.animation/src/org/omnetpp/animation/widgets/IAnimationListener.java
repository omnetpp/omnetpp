/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

/**
 * Notification interface for events and state changes within the animation.
 *
 * @author levy
 */
public interface IAnimationListener {
    /**
     * Notifies the listener when the animation has been stopped or started.
     */
	public void runningStateChanged(boolean isRunning);

	/**
	 * Notifies the listener about a new animation position.
	 */
	public void animationPositionChanged(AnimationPosition animationPosition);
}
