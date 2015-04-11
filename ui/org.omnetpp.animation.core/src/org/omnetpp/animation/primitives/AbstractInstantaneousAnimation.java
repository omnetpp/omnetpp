/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;

/**
 * Abstract base class for animation primitives that are instantaneous with
 * respect to their event number and simulation time.
 *
 * @author levy
 */
public class AbstractInstantaneousAnimation extends AbstractAnimationPrimitive {
	public AbstractInstantaneousAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime) {
		super(animationController);
		setSourceEventNumber(eventNumber);
		setBeginEventNumber(eventNumber);
		setEndEventNumber(eventNumber);
		setBeginSimulationTime(simulationTime);
        setEndSimulationTime(simulationTime);
	}
}
