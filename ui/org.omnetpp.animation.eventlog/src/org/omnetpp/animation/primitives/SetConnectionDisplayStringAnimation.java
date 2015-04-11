/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.GateModel;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.ned.model.DisplayString;

public class SetConnectionDisplayStringAnimation extends AbstractInfiniteAnimation {
	protected GateModel sourceGate;

	protected IDisplayString displayString;

	protected IDisplayString oldDisplayString;

	public SetConnectionDisplayStringAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, GateModel sourceGate, IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime);
		this.sourceGate = sourceGate;
		this.displayString = displayString;
	}

	@Override
	public void activate() {
        super.activate();
		ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(sourceGate, ConnectionFigure.class);
		if (connectionFigure != null) {
		    oldDisplayString = sourceGate.getDisplayString();
		    sourceGate.setDisplayString(displayString);
			connectionFigure.setDisplayString(displayString);
		}
	}

	@Override
	public void deactivate() {
        super.deactivate();
		ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(sourceGate, ConnectionFigure.class);
		if (connectionFigure != null) {
		    sourceGate.setDisplayString(oldDisplayString);
			connectionFigure.setDisplayString(oldDisplayString == null ? new DisplayString("") : oldDisplayString);
		}
	}
}
