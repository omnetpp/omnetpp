package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;

public class SetConnectionDisplayStringAnimation extends AbstractAnimationPrimitive {
	private ConnectionId connectionId;

	private IDisplayString displayString;
	
	public SetConnectionDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 ConnectionId connectionId,
									 IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.connectionId = connectionId;
		this.displayString = displayString;
		//System.out.println(displayString);
	}
	
	@Override
	public double getEndSimulationTime() {
		return Double.MAX_VALUE;
	}
	
	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
	
	public void redo() {
//XXX		
//		if (animationEnvironment.getSimulation().getModuleByID(connectionId.getModuleId()).getParentModule()==animationEnvironment.getSimulation().getRootModule()) { //FIXME
//			ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
//			connectionFigure.setDisplayString(displayString);
//		}
	}

	public void undo() {
		// TODO: not enough info
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}
