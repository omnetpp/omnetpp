/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

/**
 * Draws an ellipse if the message transmission time is 0. Otherwise it draws a
 * growing line following the connection as time goes on.
 */
public class SendDirectAnimation extends SendMessageAnimation {
	protected int sourceModuleId;

	protected int destinationModuleId;

	public SendDirectAnimation(ReplayAnimationController animationController,
							   AnimationPosition animationPosition,
							   double propagationTime,
							   double transmissionTime,
							   int sourceModuleId,
							   int destinationModuleId,
							   IRuntimeMessage msg) {
		super(animationController, animationPosition, propagationTime, transmissionTime, null, msg);
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;
	}

	@Override
	protected Point getBeginPoint() {
		return getSubmoduleFigureCenter(sourceModuleId);
	}

	@Override
	protected Point getEndPoint() {
		return getSubmoduleFigureCenter(destinationModuleId);
	}

	@Override
	protected boolean isDisplayed() {
		return getSubmoduleFigure(sourceModuleId) != null && getSubmoduleFigure(destinationModuleId) != null;
	}
}
