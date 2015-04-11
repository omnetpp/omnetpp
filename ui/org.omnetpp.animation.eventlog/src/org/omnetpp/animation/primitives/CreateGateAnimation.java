/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.GateModel;

public class CreateGateAnimation extends AbstractInfiniteAnimation {
	protected GateModel gate;

	public CreateGateAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, GateModel gate) {
		super(animationController, eventNumber, simulationTime);
		this.gate = gate;
	}
}
