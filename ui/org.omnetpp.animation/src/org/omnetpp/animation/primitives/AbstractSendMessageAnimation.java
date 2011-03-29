/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogMessage;

public abstract class AbstractSendMessageAnimation extends AbstractAnimationPrimitive {
	protected EventLogMessage message;

	protected BigDecimal propagationDelay;

	protected BigDecimal transmissionDelay;

	public AbstractSendMessageAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, EventLogMessage message) {
		super(animationController);
		this.propagationDelay = propagationDelay;
		this.transmissionDelay = transmissionDelay;
		this.message = message;
	}

	protected abstract Point getBeginPoint();

	protected abstract Point getEndPoint();

	@Override
	public void activate() {
	    super.activate();
	}

	protected Point[] getMessageSendPoints(AnimationPosition animationPosition, int orthogonalTranslation) {
		Point p1 = getBeginPoint().getCopy();
		Point p2 = getEndPoint().getCopy();
		double simulationTimeDelta = getEndSimulationTime().subtract(getBeginSimulationTime()).subtract(transmissionDelay).doubleValue();

		// translate connection line coordinates orthogonal to the line
		if (orthogonalTranslation != 0) {
			PrecisionPoint n = new PrecisionPoint(p1.y - p2.y, p2.x - p1.x);
			n.performScale(orthogonalTranslation / Math.sqrt(n.x * n.x + n.y * n.y));
			p1.translate(n);
			p2.translate(n);
		}

		double alpha;
		if (simulationTimeDelta != 0)
			alpha = animationPosition.getSimulationTime().subtract(getBeginSimulationTime()).doubleValue() / simulationTimeDelta;
		else
			alpha = (animationPosition.getOriginRelativeAnimationTime() - getOriginRelativeBeginAnimationTime()) / (getOriginRelativeEndAnimationTime() - getOriginRelativeBeginAnimationTime());
		alpha = Math.max(0, Math.min(alpha, 1));

		Point pAlpha;
		Point pBeta;

		if (transmissionDelay.equals(BigDecimal.getZero()))
			pAlpha = pBeta = getConvexCombination(p1, p2, alpha);
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

			pAlpha = getConvexCombination(p1, p2, alpha);
			pBeta = getConvexCombination(p1, p2, beta);
			// System.out.println("*** Alpha: " + alpha + ", beta: " + beta);
		}

		return new Point[] {pAlpha, pBeta};
	}
}
