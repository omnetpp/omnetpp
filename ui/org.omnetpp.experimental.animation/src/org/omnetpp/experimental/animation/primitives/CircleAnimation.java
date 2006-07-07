package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class CircleAnimation extends AbstractAnimationPrimitive {
	private double endSimulationTime;
	
	private Point location;

	private Ellipse circle;
	
	private Timer radiusTimer;
	
	public CircleAnimation(ReplayAnimationController animationController,
						   double beginSimulationTime,
						   double endSimulationTime,
						   Point location) {
		super(animationController, beginSimulationTime);
		this.circle = new Ellipse();
		this.location = location;
		this.endSimulationTime = endSimulationTime;
		this.circle.setFill(false);
		this.radiusTimer = new Timer(20, false, false) {
			public void run() {
				setRadius(CircleAnimation.this.animationController.getSimulationTime());
			}
		};
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (beginSimulationTime <= simulationTime && simulationTime <= endSimulationTime) {
			setRadius(simulationTime);
			show();
		}
		else
			hide();
	}
	
	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	protected void setRadius(double simulationTime) {
		int radius = (int)Math.floor(100 * (simulationTime - beginSimulationTime) / (endSimulationTime - beginSimulationTime));
		Rectangle r = new Rectangle(location.x - radius, location.y - radius, radius * 2, radius * 2);
		setConstraint(circle, r);
	}
	
	protected void show() {
		if (!shown) {
			showFigure(circle);
			radiusTimer.reset();
			getTimerQueue().addTimer(radiusTimer);
		}
	}
	
	protected void hide() {
		if (shown) {
			hideFigure(circle);
			getTimerQueue().removeTimer(radiusTimer);
		}
	}
}
