/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.eventlog.engine.IEvent;

public class AbstractInfiniteAnimation extends AbstractAnimationPrimitive {
	public AbstractInfiniteAnimation(AnimationController animationController) {
		super(animationController);
		IEvent lastEvent = animationController.getEventLogInput().getEventLog().getLastEvent();
		setEndEventNumber(lastEvent.getEventNumber());
		setEndSimulationTime(lastEvent.getSimulationTime());
		setRelativeEndAnimationTime(0);
	}
}
