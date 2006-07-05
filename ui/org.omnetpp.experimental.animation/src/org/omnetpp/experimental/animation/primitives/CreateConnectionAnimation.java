package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;

public class CreateConnectionAnimation extends AbstractAnimationPrimitive {
	private double creationSimulationTime;
	
	private ConnectionFigure connectionFigure;

	private GateId sourceGateId;

	private GateId targetGateId;

	public CreateConnectionAnimation(IAnimationController controller,
									 double creationSimulationTime,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(controller);
		this.creationSimulationTime = creationSimulationTime;
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionFigure = new ConnectionFigure();
	}
	
	public void gotoSimulationTime(double t) {
		if (t >= creationSimulationTime) {
			connectionFigure.setSourceAnchor(getGateAnchor(sourceGateId));
			connectionFigure.setTargetAnchor(getGateAnchor(targetGateId));
			controller.setFigure(new ConnectionId(sourceGateId.getModuleId(), sourceGateId.getGateId()), connectionFigure);
			showFigure(connectionFigure);
		}
		else {
			hideFigure(connectionFigure);
		}
	}
	
	private GateAnchor getGateAnchor(GateId gateId) {
		return (GateAnchor)controller.getFigure(gateId);
	}
}
