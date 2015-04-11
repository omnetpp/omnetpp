/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.MessageModel;

public abstract class AbstractSendMessageAnimation extends AbstractAnimationPrimitive {
	protected MessageModel message;

	protected BigDecimal propagationDelay;

	protected BigDecimal transmissionDelay;

	protected double messageHighlightAnimationDuration;

	public AbstractSendMessageAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, MessageModel message, double messageHighlightAnimationDuration) {
		super(animationController);
		this.propagationDelay = propagationDelay;
		this.transmissionDelay = transmissionDelay;
		this.message = message;
		this.messageHighlightAnimationDuration = messageHighlightAnimationDuration;
	}

	protected abstract Point getBeginPoint();

	protected abstract Point getEndPoint();

	@Override
	public void activate() {
	    super.activate();
	}

	protected Point[] getMessageSendPoints(AnimationPosition animationPosition, int orthogonalTranslation) {
		Point connectionBegin = getBeginPoint().getCopy();
		Point connectionEnd = getEndPoint().getCopy();
		double simulationTimeDelta = getEndSimulationTime().subtract(getBeginSimulationTime()).subtract(transmissionDelay).doubleValue();

		// translate connection line coordinates orthogonal to the line
		if (orthogonalTranslation != 0) {
			PrecisionPoint n = new PrecisionPoint(connectionBegin.y - connectionEnd.y, connectionEnd.x - connectionBegin.x);
			n.performScale(orthogonalTranslation / Math.sqrt(n.x * n.x + n.y * n.y));
			connectionBegin.translate(n);
			connectionEnd.translate(n);
		}

		double alpha;
		if (simulationTimeDelta != 0)
			alpha = animationPosition.getSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / simulationTimeDelta;
		else
			alpha = (animationPosition.getOriginRelativeAnimationTime() - getOriginRelativeBeginAnimationTime()) / (getOriginRelativeEndAnimationTime() - messageHighlightAnimationDuration - getOriginRelativeBeginAnimationTime());
		alpha = Math.max(0, Math.min(alpha, 1));

		Point messageBegin;
		Point messageEnd;

		if (transmissionDelay.equals(BigDecimal.getZero())) {
	        if (propagationDelay.equals(BigDecimal.getZero())) {
	            messageBegin = connectionBegin;
	            messageEnd = connectionEnd;
	        }
	        else
	            messageBegin = messageEnd = getConvexCombination(connectionBegin, connectionEnd, alpha);
		}
		else {
			double beta;
			if (simulationTimeDelta != 0) {
				beta = animationPosition.getSimulationTime().subtract(getBeginSimulationTime()).subtract(transmissionDelay).doubleValue() / simulationTimeDelta;
				beta = Math.max(0, Math.min(beta, 1));
			}
			else {
				alpha = 0;
				beta = 1;
			}

			messageBegin = getConvexCombination(connectionBegin, connectionEnd, alpha);
			messageEnd = getConvexCombination(connectionBegin, connectionEnd, beta);
			// System.out.println("*** Alpha: " + alpha + ", beta: " + beta);
		}

		return new Point[] {messageBegin, messageEnd};
	}
}
