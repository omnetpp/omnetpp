package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;

public class CreateConnectionAnimation extends AbstractAnimationPrimitive {
	private ConnectionFigure connectionFigure;

	private GateId sourceGateId;

	private GateId targetGateId;

	public CreateConnectionAnimation(IAnimationController controller,
									 double beginSimulationTime,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(controller, beginSimulationTime);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionFigure = new ConnectionFigure();
	}
	
	public void gotoSimulationTime(double t) {
		if (t >= beginSimulationTime) {
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
