package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

/**
 * Draws an ellipse if the message transmission time is 0. Otherwise it draws a
 * growing line following the connection as time goes on.
 */
public class SendDirectAnimation extends SendMessageAnimation {
	private int senderModuleId;

	private int destModuleId;

	public SendDirectAnimation(ReplayAnimationController animationController, 
							   long eventNumber, 
							   double simulationTime,
							   long animationNumber, 
							   double propagationTime, 
							   double transmissionTime, 
							   int senderModuleId, 
							   int destModuleId,
							   IRuntimeMessage msg) {
		super(animationController, eventNumber, simulationTime, animationNumber, propagationTime, transmissionTime, null, msg);
		this.senderModuleId = senderModuleId;
		this.destModuleId = destModuleId;
	}

	public void redo() {
		SubmoduleFigure senderModuleFigure = getSubmoduleFigure(senderModuleId);
		SubmoduleFigure destModuleFigure = getSubmoduleFigure(destModuleId);

		if (senderModuleFigure!=null && destModuleFigure!=null)
			redoInternal();
	}

	public void undo() {
		SubmoduleFigure senderModuleFigure = getSubmoduleFigure(senderModuleId);
		SubmoduleFigure destModuleFigure = getSubmoduleFigure(destModuleId);

		if (senderModuleFigure!=null && destModuleFigure!=null)
			undoInternal();
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		SubmoduleFigure senderModuleFigure = getSubmoduleFigure(senderModuleId);
		SubmoduleFigure destModuleFigure = getSubmoduleFigure(destModuleId);

		if (senderModuleFigure!=null && destModuleFigure!=null)
			animateSendMessageAt(simulationTime, animationTime, getSubmoduleFigureCenter(senderModuleId), getSubmoduleFigureCenter(destModuleId));
	}
}
