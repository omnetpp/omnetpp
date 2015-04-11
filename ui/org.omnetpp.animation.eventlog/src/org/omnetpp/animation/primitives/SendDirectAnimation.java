/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenS	im Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.MessageModel;

/**
 * Draws an ellipse if the message transmission time is 0. Otherwise it draws a growing line following the connection as time goes on.
 */
public class SendDirectAnimation extends SendMessageAnimation {
	protected int sourceModuleId;

	protected int destinationModuleId;

	public SendDirectAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, int sourceModuleId, int destinationModuleId, MessageModel message, double messageHighlightAnimationTime) {
		super(animationController, propagationDelay, transmissionDelay, null, message, messageHighlightAnimationTime);
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;
	}

    @Override
    protected Layer getDecorationLayer() {
        return getEnclosingCompoundModuleFigure(destinationModuleId).getForegroundDecorationLayer();
    }

	@Override
	protected Point getBeginPoint() {
        // TODO: this should not calculate with the decoration icons
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(sourceModuleId));
	}

	@Override
	protected Point getEndPoint() {
        // TODO: this should not calculate with the decoration icons
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(destinationModuleId));
	}
}
