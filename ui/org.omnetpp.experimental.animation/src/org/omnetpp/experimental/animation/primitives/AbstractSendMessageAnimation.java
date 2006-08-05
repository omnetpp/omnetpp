package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public abstract class AbstractSendMessageAnimation extends AbstractAnimationPrimitive {
	protected IRuntimeMessage msg; //FIXME should not be here...

	protected double propagationTime;
	
	protected double transmissionTime;

	protected double endSimulationTime;

	public AbstractSendMessageAnimation(ReplayAnimationController animationController,
								long eventNumber,
								double simulationTime,
								long animationNumber,
								double propagationTime,
								double transmissionTime,
								IRuntimeMessage msg) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.propagationTime = propagationTime;
		this.transmissionTime = transmissionTime;
		this.endSimulationTime = simulationTime + propagationTime + transmissionTime;
		this.msg = msg;
	}

	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	@Override
	public double getEndAnimationTime() {
		if (endSimulationTime == beginSimulationTime)
			return animationEnvironment.getAnimationTimeForAnimationNumber(animationNumber + 1);
		else {
			return animationEnvironment.getAnimationTimeForSimulationTime(endSimulationTime);
		}
	}
	
	protected abstract Point getBeginPoint();
	
	protected abstract Point getEndPoint();

	protected Point[] getMessageSendPoints(double simulationTime, double animationTime, int orthogonalTranslation) {
		Point p1 = getBeginPoint();
		Point p2 = getEndPoint();
		double simulationTimeDelta = endSimulationTime - beginSimulationTime - transmissionTime;

		// translate connection line coordinates orthogonal to the line
		if (orthogonalTranslation != 0) {
			PrecisionPoint n = new PrecisionPoint(p1.y - p2.y, p2.x - p1.x);
			n.performScale(orthogonalTranslation / Math.sqrt(n.x * n.x + n.y * n.y));
			p1.translate(n);
			p2.translate(n);
		}

		double alpha;
		if (simulationTimeDelta != 0)
			alpha = (simulationTime - beginSimulationTime) / simulationTimeDelta;
		else
			alpha = (animationTime - getBeginAnimationTime()) / (getEndAnimationTime() - getBeginAnimationTime());
		alpha = Math.max(0, Math.min(alpha, 1));
		
		Point pAlpha; 
		Point pBeta;
		
		if (transmissionTime == 0)
			pAlpha = pBeta = getConvexCombination(p1, p2, alpha);
		else {
			double beta;
			if (simulationTimeDelta != 0) {
				beta = (simulationTime - beginSimulationTime - transmissionTime) / simulationTimeDelta;
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
