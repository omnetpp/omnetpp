/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.GateAnchor;
import org.omnetpp.figures.ModuleFigure;

public class CreateConnectionAnimation extends AbstractInfiniteAnimation {
	protected GateId sourceGateId;

	protected GateId targetGateId;

	protected ConnectionId connectionId;

	public CreateConnectionAnimation(ReplayAnimationController animationController,
									 AnimationPosition animationPosition,
									 GateId sourceGateId,
									 GateId targetGateId) {
		super(animationController, animationPosition);
		this.sourceGateId = sourceGateId;
		this.targetGateId = targetGateId;
		this.connectionId = new ConnectionId(sourceGateId.getModuleId(), sourceGateId.getGateId());
	}

	@Override
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

	@Override
	public void undo() {
		ConnectionFigure connectionFigure = (ConnectionFigure)animationEnvironment.getFigure(connectionId);
		if (connectionFigure==null) {
			getEnclosingModuleFigure().removeConnectionFigure(connectionFigure);
			animationEnvironment.setFigure(connectionId, null);
		}
	}
	
	protected GateAnchor getGateAnchor(GateId gateId) {
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
