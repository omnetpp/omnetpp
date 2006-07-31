package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class SendBroadcastAnimation extends AbstractAnimationPrimitive {
	private double endSimulationTime;
	
	private Point location;

	private Ellipse circle;
	
	public SendBroadcastAnimation(ReplayAnimationController animationController,
								  long eventNumber,
								  double beginSimulationTime,
								  long animationNumber,
								  double endSimulationTime,
								  Point location) {
		super(animationController, eventNumber, beginSimulationTime, animationNumber);
		this.circle = new Ellipse();
		this.location = location;
		this.endSimulationTime = endSimulationTime;
		this.circle.setFill(false);
	}
	
	public void redo() {
		addFigure(circle);
	}

	public void undo() {
		removeFigure(circle);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		int radius = (int)Math.floor(100 * (simulationTime - beginSimulationTime) / (endSimulationTime - beginSimulationTime));
		Rectangle r = new Rectangle(location.x - radius, location.y - radius, radius * 2, radius * 2);
		setConstraint(circle, r);
	}
	
	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
}
