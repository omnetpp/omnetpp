/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public abstract class AbstractSendMessageAnimation extends AbstractAnimationPrimitive {
	protected IRuntimeMessage msg; //FIXME should not be here...

	protected double propagationTime;
	
	protected double transmissionTime;

	protected double endSimulationTime;

	public AbstractSendMessageAnimation(ReplayAnimationController animationController,
								AnimationPosition animationPosition,
								double propagationTime,
								double transmissionTime,
								IRuntimeMessage msg) {
		super(animationController, animationPosition);
		this.propagationTime = propagationTime;
		this.transmissionTime = transmissionTime;
		this.endSimulationTime = getBeginSimulationTime() + propagationTime + transmissionTime;
		this.msg = msg;
	}

	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	@Override
	public double getEndAnimationTime() {
		if (endSimulationTime == getBeginSimulationTime())
			return animationEnvironment.getAnimationTimeForAnimationNumber(getAnimationNumber() + 1);
		else {
			return animationEnvironment.getAnimationTimeForSimulationTime(endSimulationTime);
		}
	}
	
	protected abstract Point getBeginPoint();
	
	protected abstract Point getEndPoint();

	protected Point[] getMessageSendPoints(AnimationPosition animationPosition, int orthogonalTranslation) {
		Point p1 = getBeginPoint();
		Point p2 = getEndPoint();
		double simulationTimeDelta = endSimulationTime - getBeginSimulationTime() - transmissionTime;

		// translate connection line coordinates orthogonal to the line
		if (orthogonalTranslation != 0) {
			PrecisionPoint n = new PrecisionPoint(p1.y - p2.y, p2.x - p1.x);
			n.performScale(orthogonalTranslation / Math.sqrt(n.x * n.x + n.y * n.y));
			p1.translate(n);
			p2.translate(n);
		}

		double alpha;
		if (simulationTimeDelta != 0)
			alpha = (animationPosition.getSimulationTime() - getBeginSimulationTime()) / simulationTimeDelta;
		else
			alpha = (animationPosition.getAnimationTime() - getBeginAnimationTime()) / (getEndAnimationTime() - getBeginAnimationTime());
		alpha = Math.max(0, Math.min(alpha, 1));
		
		Point pAlpha; 
		Point pBeta;
		
		if (transmissionTime == 0)
			pAlpha = pBeta = getConvexCombination(p1, p2, alpha);
		else {
			double beta;
			if (simulationTimeDelta != 0) {
				beta = (animationPosition.getSimulationTime() - getBeginSimulationTime() - transmissionTime) / simulationTimeDelta;
				beta = Math.max(0, Math.min(beta, 1));
			}
			else {
				alpha = 0;
				beta = 1;
			}

			pAlpha = getConvexCombination(p1, p2, alpha);
			pBeta = getConvexCombination(p1, p2, beta);
		}

		return new Point[] {pAlpha, pBeta};
	}
}
