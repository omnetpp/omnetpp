package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.ModuleFigure;

public class CreateConnectionAnimation extends AbstractAnimationPrimitive {
	private GateId sourceGateId;

	private GateId targetGateId;

	private ConnectionId connectionId;

	public CreateConnectionAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionId = new ConnectionId(sourceGateId.getModuleId(), sourceGateId.getGateId());
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
		if (animationEnvironment.getSimulation().getModuleByID(sourceGateId.getModuleId()).getParentModule()==animationEnvironment.getSimulation().getRootModule() &&
			animationEnvironment.getSimulation().getModuleByID(targetGateId.getModuleId()).getParentModule()==animationEnvironment.getSimulation().getRootModule()) //FIXME 
		{
			ConnectionFigure connectionFigure = new ConnectionFigure();
			connectionFigure.setSourceAnchor(getGateAnchor(sourceGateId));
			connectionFigure.setTargetAnchor(getGateAnchor(targetGateId));
			animationEnvironment.setFigure(connectionId, connectionFigure);
			getEnclosingModuleFigure().addConnectionFigure(connectionFigure);
		}
	}

	public void undo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure==null) {
			removeFigure(connectionFigure);
			animationEnvironment.setFigure(connectionId, null);
		}
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
	
	private GateAnchor getGateAnchor(GateId gateId) {
		GateAnchor gateAnchor = (GateAnchor)animationEnvironment.getFigure(gateId);
		
		if (gateAnchor == null) {
			ModuleFigure moduleFigure = (ModuleFigure)animationEnvironment.getFigure(animationEnvironment.getSimulation().getModuleByID(gateId.getModuleId()));
			// TODO: use gate name
			gateAnchor = new GateAnchor(moduleFigure);
			animationEnvironment.setFigure(gateId, gateAnchor);
		}

		return gateAnchor;
	}
}
