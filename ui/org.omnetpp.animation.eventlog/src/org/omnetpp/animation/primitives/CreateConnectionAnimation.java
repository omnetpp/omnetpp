/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.ConnectionModel;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateConnectionAnimation extends AbstractInfiniteAnimation {
    protected ConnectionModel connection;

	public CreateConnectionAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, ConnectionModel connection) {
		super(animationController, eventNumber, simulationTime);
		this.connection = connection;
	}

	@Override
	public void activate() {
	    super.activate();
        ModuleModel sourceModule = getSimulation().getModuleById(connection.getSourceGate().getOwnerModuleId());
	    ModuleModel targetModule = getSimulation().getModuleById(connection.getDestinationGate().getOwnerModuleId());
	    ConnectionFigure connectionFigure = null;
	    if (sourceModule.getParentModule().equals(targetModule.getParentModule())) {
            SubmoduleFigure sourceSubmoduleFigure = getSubmoduleFigure(sourceModule);
            SubmoduleFigure targetSubmoduleFigure = getSubmoduleFigure(targetModule);
            if (sourceSubmoduleFigure != null && targetSubmoduleFigure != null) {
                connectionFigure = new ConnectionFigure();
                connectionFigure.setSourceAnchor(getGateAnchor(connection.getSourceGate(), SubmoduleFigure.class));
                connectionFigure.setTargetAnchor(getGateAnchor(connection.getDestinationGate(), SubmoduleFigure.class));
                getEnclosingCompoundModuleFigure(connection.getSourceGate().getOwnerModuleId()).getConnectionLayer().add(connectionFigure);
            }
	    }
	    else if (sourceModule.equals(targetModule.getParentModule())) {
            CompoundModuleFigure sourceCompoundModuleFigure = getCompoundModuleFigure(sourceModule);
            SubmoduleFigure targetSubmoduleFigure = getSubmoduleFigure(targetModule);
            if (sourceCompoundModuleFigure != null && targetSubmoduleFigure != null) {
                connectionFigure = new ConnectionFigure();
                connectionFigure.setSourceAnchor(getGateAnchor(connection.getSourceGate(), CompoundModuleFigure.class));
                connectionFigure.setTargetAnchor(getGateAnchor(connection.getDestinationGate(), SubmoduleFigure.class));
                sourceCompoundModuleFigure.getConnectionLayer().add(connectionFigure);
            }
	    }
        else if (targetModule.equals(sourceModule.getParentModule())) {
            SubmoduleFigure sourceSubmoduleFigure = getSubmoduleFigure(sourceModule);
            CompoundModuleFigure targetCompoundModuleFigure = getCompoundModuleFigure(targetModule);
            if (sourceSubmoduleFigure != null && targetCompoundModuleFigure != null) {
                connectionFigure = new ConnectionFigure();
                connectionFigure.setSourceAnchor(getGateAnchor(connection.getSourceGate(), SubmoduleFigure.class));
                connectionFigure.setTargetAnchor(getGateAnchor(connection.getDestinationGate(), CompoundModuleFigure.class));
                targetCompoundModuleFigure.getConnectionLayer().add(connectionFigure);
            }
        }
        else
            connectionFigure = null;
	    if (connectionFigure != null) {
	        final ConnectionFigure finalConnectionFigure = connectionFigure;
            connectionFigure.setArrowHeadEnabled(true);
    	    connectionFigure.addMouseListener(new MouseListener() {
                public void mouseDoubleClicked(MouseEvent me) {
                }

                public void mousePressed(MouseEvent me) {
                }

                public void mouseReleased(MouseEvent me) {
                    animationController.getAnimationCanvas().setSelectedElement(finalConnectionFigure, connection);
                }
            });
            animationController.setFigure(connection.getSourceGate(), connectionFigure);
	    }
	}

    @Override
	public void deactivate() {
        super.deactivate();
		ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(connection.getSourceGate(), ConnectionFigure.class);
		if (connectionFigure != null) {
		    connectionFigure.getParent().remove(connectionFigure);
			animationController.setFigure(connection.getSourceGate(), ConnectionFigure.class, null);
		}
	}
}
