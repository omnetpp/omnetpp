package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class SendBroadcastAnimation extends AbstractAnimationPrimitive {
	private double propagationTime;
	
	private double transmissionTime;
	
	private int sourceModuleId;

	private int destinationModuleId;

	private Ellipse circle;
	
	public SendBroadcastAnimation(ReplayAnimationController animationController,
								  long eventNumber,
								  double beginSimulationTime,
								  long animationNumber,
								  double propagationTime,
								  double transmissionTime,
								  int sourceModuleId,
								  int destinationModuleId) {
		super(animationController, eventNumber, beginSimulationTime, animationNumber);
		this.propagationTime = propagationTime;
		this.transmissionTime = transmissionTime;
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;

		circle = new Ellipse() {
			@Override
			public void paint(Graphics graphics) {
				graphics.pushState();
				graphics.setAlpha(128);
				super.paint(graphics);
				graphics.popState();
			}
		};
		circle.setFill(true);
	}
	
	public void redo() {
		getEnclosingModuleFigure().addMessageFigure(circle);
	}

	public void undo() {
		getEnclosingModuleFigure().removeMessageFigure(circle);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		Point sourceLocation = getSubmoduleFigureCenter(sourceModuleId);
		Point destinationLocation = getSubmoduleFigureCenter(destinationModuleId);
		int radius = (int)Math.floor(sourceLocation.getDistance(destinationLocation) * (simulationTime - beginSimulationTime) / propagationTime);
		int width = (int)(radius * Math.min(1, transmissionTime / propagationTime));
		radius = Math.max(0, radius - width / 2);
		width /= 2;
		Rectangle r = new Rectangle(sourceLocation.x - radius, sourceLocation.y - radius, radius * 2, radius * 2);
		circle.setLineWidth(width);
		circle.setBounds(r);
	}
}
