/*--------------------------------------------------------------*

  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.engine.IEvent;

/**
 * Abstract base class for animation primitives that are infinite extent with
 * respect to their end animation position.
 *
 * @author levy
 */
public class AbstractInfiniteAnimation extends AbstractAnimationPrimitive {
	public AbstractInfiniteAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime) {
		super(animationController);
		IEvent lastEvent = animationController.getEventLogInput().getEventLog().getLastEvent();
		// NOTE: there are no events after the last one, but we can pretend that simulation time goes on
		// this is necessary, because some animation might go beyond that (e.g. a message send)
        setSourceEventNumber(eventNumber);
        setBeginEventNumber(eventNumber);
		setEndEventNumber(lastEvent.getEventNumber());
        setBeginSimulationTime(simulationTime);
	    setEndSimulationTime(BigDecimal.getPositiveInfinity());
	    setFrameRelativeEndAnimationTime(Double.POSITIVE_INFINITY);
	}
}
