package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

/**
 * Draws an ellipse if the message transmission time is 0. Otherwise it draws a
 * growing line following the connection as time goes on.
 */
public class SendDirectAnimation extends SendMessageAnimation {
	protected int sourceModuleId;

	protected int destinationModuleId;

	public SendDirectAnimation(ReplayAnimationController animationController, 
							   long eventNumber, 
							   double simulationTime,
							   long animationNumber, 
							   double propagationTime, 
							   double transmissionTime, 
							   int sourceModuleId, 
							   int destinationModuleId,
							   IRuntimeMessage msg) {
		super(animationController, eventNumber, simulationTime, animationNumber, propagationTime, transmissionTime, null, msg);
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;
	}

	protected Point getBeginPoint() {
		return getSubmoduleFigureCenter(sourceModuleId);
	}
	
	protected Point getEndPoint() {
		return getSubmoduleFigureCenter(destinationModuleId);
	}

	protected boolean isDisplayed() {
		return getSubmoduleFigure(sourceModuleId) != null && getSubmoduleFigure(destinationModuleId) != null;
	}
}
