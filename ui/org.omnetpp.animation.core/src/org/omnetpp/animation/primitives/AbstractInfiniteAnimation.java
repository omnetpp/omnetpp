/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;

/**
 * Abstract base class for animation primitives that are infinite extent with
 * respect to their end animation position.
 *
 * @author levy
 */
public class AbstractInfiniteAnimation extends AbstractAnimationPrimitive {
	public AbstractInfiniteAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime) {
		super(animationController);
		long lastEventNumber = animationController.getAnimationCoordinateSystem().getLastEventNumber();
		// NOTE: there are no events after the last one, but we can pretend that simulation time goes on
		// this is necessary, because some animation might go beyond that (e.g. a message send)
        setSourceEventNumber(eventNumber);
        setBeginEventNumber(eventNumber);
		setEndEventNumber(lastEventNumber);
        setBeginSimulationTime(simulationTime);
	    setEndSimulationTime(BigDecimal.getPositiveInfinity());
	    setFrameRelativeEndAnimationTime(Double.POSITIVE_INFINITY);
	}
}
