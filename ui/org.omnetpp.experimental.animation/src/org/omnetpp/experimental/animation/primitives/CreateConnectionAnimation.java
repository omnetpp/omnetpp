package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.ModuleFigure;

public class CreateConnectionAnimation extends AbstractAnimationPrimitive {
	private ConnectionFigure connectionFigure;

	private GateId sourceGateId;

	private GateId targetGateId;

	public CreateConnectionAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double beginSimulationTime,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(animationController, eventNumber, beginSimulationTime);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionFigure = new ConnectionFigure();
	}
	
	public void animateAt(long eventNumber, double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			connectionFigure.setSourceAnchor(getGateAnchor(sourceGateId));
			connectionFigure.setTargetAnchor(getGateAnchor(targetGateId));
			animationController.setFigure(new ConnectionId(sourceGateId.getModuleId(), sourceGateId.getGateId()), connectionFigure);
			showFigure(connectionFigure);
		}
		else {
			hideFigure(connectionFigure);
		}
	}
	
	private GateAnchor getGateAnchor(GateId gateId) {
		GateAnchor gateAnchor = (GateAnchor)animationController.getFigure(gateId);
		
		if (gateAnchor == null) {
			ModuleFigure moduleFigure = (ModuleFigure)animationController.getFigure(animationController.getSimulation().getModuleByID(gateId.getModuleId()));
			// TODO: use gate name
			gateAnchor = new GateAnchor(moduleFigure, String.valueOf(gateId.getGateId()));
			animationController.setFigure(gateId, gateAnchor);
		}

		return gateAnchor;
	}
}
