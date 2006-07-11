package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Polyline;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;

public class SendMessageAnimation extends AbstractAnimationPrimitive {
	private static final Color FOREGROUND_COLOR = new Color(null, 128, 0, 0);

	private ConnectionId connectionId;
	
	private double transmissionTime;
	
	private double endSimulationTime;
	
	private Ellipse messageEllipse;
	
	private Polyline messageLine;

	public SendMessageAnimation(ReplayAnimationController animationController,
								long eventNumber,
								double beginSimulationTime,
								double propagationTime,
								double transmissionTime,
								ConnectionId connectionId) {
		super(animationController, eventNumber, beginSimulationTime);
		this.transmissionTime = transmissionTime;
		this.endSimulationTime = beginSimulationTime + propagationTime + transmissionTime;
		this.connectionId = connectionId;
		
		messageEllipse = new Ellipse();
		messageEllipse.setForegroundColor(FOREGROUND_COLOR);
		messageEllipse.setBackgroundColor(FOREGROUND_COLOR);

		messageLine = new Polyline();
		messageLine.setForegroundColor(FOREGROUND_COLOR);
		messageLine.setLineWidth(5);
	}
	
	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	public void animateAt(long eventNumber, double simulationTime) {
		if (beginSimulationTime <= simulationTime && simulationTime <= endSimulationTime) {
			ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(connectionId);

			if (connectionFigure != null) {
				Point p1 = connectionFigure.getStart();
				Point p2 = connectionFigure.getEnd();				
				double simulationTimeDelta = endSimulationTime - beginSimulationTime - transmissionTime;

				double alpha;
				if (simulationTimeDelta != 0) {
					alpha = (simulationTime - beginSimulationTime) / simulationTimeDelta;
					alpha = Math.max(0, Math.min(alpha, 1));
				}
				else
					alpha = 0.5;
				
				if (transmissionTime == 0) {
					Point p = getConvexCombination(p1, p2, alpha);
					setConstraint(messageEllipse, new Rectangle(p.x - 3, p.y - 3, 7, 7));
					showFigure(messageEllipse);
				}
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

					Point pAlpha = getConvexCombination(p1, p2, alpha);
					Point pBeta = getConvexCombination(p1, p2, beta);
					messageLine.setEndpoints(pAlpha, pBeta);
					showFigure(messageLine);
				}
			}
		}
		else {
			hideFigure(messageEllipse);
			hideFigure(messageLine);
		}
	}

	private Point getConvexCombination(Point p1, Point p2, double alpha) {
		return new Point(Math.round((1 - alpha) * p1.x + alpha * p2.x), Math.round((1 - alpha) * p1.y + alpha * p2.y));
	}
}
