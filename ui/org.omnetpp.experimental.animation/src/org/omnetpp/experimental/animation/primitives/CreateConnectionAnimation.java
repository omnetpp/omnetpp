package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;

public class CreateConnectionAnimation extends AbstractAnimationPrimitive {
	private ConnectionFigure connectionFigure;

	private GateId sourceGateId;

	private GateId targetGateId;

	public CreateConnectionAnimation(ReplayAnimationController animationController,
									 double beginSimulationTime,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(animationController, beginSimulationTime);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionFigure = new ConnectionFigure();
	}
	
	public void gotoSimulationTime(double simulationTime) {
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
		return (GateAnchor)animationController.getFigure(gateId);
	}
}
