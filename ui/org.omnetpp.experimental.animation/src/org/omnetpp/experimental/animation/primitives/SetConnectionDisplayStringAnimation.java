package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;

public class SetConnectionDisplayStringAnimation extends AbstractAnimationPrimitive {
	private ConnectionId connectionId;

	private DisplayString displayString;
	
	public SetConnectionDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 ConnectionId connectionId,
									 String displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.connectionId = connectionId;
		this.displayString = new DisplayString(null, null, displayString);
		//System.out.println(displayString);
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (simulationTime >= beginSimulationTime) {
			ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
			connectionFigure.setDisplayString(displayString);
		}
		else {
			// TODO: or what to do?
		}
	}
}
