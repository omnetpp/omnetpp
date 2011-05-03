/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogGate;

public class CreateGateAnimation extends AbstractInfiniteAnimation {
	protected EventLogGate gate;

	public CreateGateAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, EventLogGate gate) {
		super(animationController, eventNumber, simulationTime);
		this.gate = gate;
	}

	@Override
	public void activate() {
	    super.activate();
	}

	@Override
	public void deactivate() {
        super.deactivate();
	}
}
