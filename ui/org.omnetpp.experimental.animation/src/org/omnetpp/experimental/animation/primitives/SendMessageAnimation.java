package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ConnectionFigure;

public class SendMessageAnimation extends AbstractAnimationPrimitive {
	private ConnectionId connectionId;

	private double endSimulationTime;	
	
	private Ellipse message;
	
	public SendMessageAnimation(ReplayAnimationController animationController,
								long eventNumber,
								double beginSimulationTime,
								double endSimulationTime,
								ConnectionId connectionId) {
		super(animationController, eventNumber, beginSimulationTime);
		this.endSimulationTime = endSimulationTime;
		this.connectionId = connectionId;
		this.message = new Ellipse();
		this.message.setForegroundColor(new Color(null, 128, 0, 0));
		this.message.setBackgroundColor(message.getForegroundColor());
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
				double alpha = (simulationTime - beginSimulationTime) / (endSimulationTime - beginSimulationTime);
				Point p = new Point((1 - alpha) * p1.x + alpha * p2.x, (1 - alpha) * p1.y + alpha * p2.y);
				setConstraint(message, new Rectangle(p.x - 3, p.y - 3, 7, 7));

				showFigure(message);
			}
		}
		else
			hideFigure(message);
	}
}
