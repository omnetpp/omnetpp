package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;

public class SetConnectionDisplayStringAnimation extends AbstractAnimationPrimitive {
	private ConnectionId connectionId;

	private IDisplayString displayString;
	
	private IDisplayString oldDisplayString; // FIXME: this is a temproray hack to be able to undo changes

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
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
	
	public void redo() {
		IRuntimeModule module = getSourceModule();
		if (module != null && module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
			oldDisplayString = connectionFigure.getLastDisplayString();
			connectionFigure.setDisplayString(displayString);
		}
	}

	public void undo() {
		IRuntimeModule module = getSourceModule();
		if (module != null && module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
			connectionFigure.setDisplayString(oldDisplayString);// FIXME: this is a temproray hack to be able to undo changes
		}
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}

	private IRuntimeModule getSourceModule() {
		return animationEnvironment.getSimulation().getModuleByID(connectionId.getModuleId());
	}
}
