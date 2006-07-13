package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
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
									 double simulationTime,
									 long animationNumber,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionFigure = new ConnectionFigure();
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (simulationTime >= beginSimulationTime) {
			connectionFigure.setSourceAnchor(getGateAnchor(sourceGateId));
			connectionFigure.setTargetAnchor(getGateAnchor(targetGateId));
			animationEnvironment.setFigure(new ConnectionId(sourceGateId.getModuleId(), sourceGateId.getGateId()), connectionFigure);
			showFigure(connectionFigure);
		}
		else {
			hideFigure(connectionFigure);
		}
	}
	
	private GateAnchor getGateAnchor(GateId gateId) {
		GateAnchor gateAnchor = (GateAnchor)animationEnvironment.getFigure(gateId);
		
		if (gateAnchor == null) {
			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(animationEnvironment.getModuleByID(gateId.getModuleId()));
			// TODO: use gate name
			gateAnchor = new GateAnchor(moduleFigure);
			animationEnvironment.setFigure(gateId, gateAnchor);
		}

		return gateAnchor;
	}
}
