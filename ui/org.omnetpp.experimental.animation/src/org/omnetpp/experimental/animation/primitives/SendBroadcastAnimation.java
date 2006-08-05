package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class SendBroadcastAnimation extends AbstractSendMessageAnimation {
	private int sourceModuleId;

	private int destinationModuleId;

	private Ellipse circle;
	
	private double hidePropagationTime;
	
	public SendBroadcastAnimation(ReplayAnimationController animationController,
								  long eventNumber,
								  double beginSimulationTime,
								  long animationNumber,
								  double propagationTime,
								  double transmissionTime,
								  int sourceModuleId,
								  int destinationModuleId,
								  IRuntimeMessage msg) {
		super(animationController, eventNumber, beginSimulationTime, animationNumber, propagationTime, transmissionTime, msg);
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;
		this.hidePropagationTime = propagationTime * 0.2;

		circle = new Ellipse() {
			@Override
			public void paint(Graphics graphics) {
				graphics.pushState();
				double alpha = 64 * (animationEnvironment.getSimulationTime() < endSimulationTime - hidePropagationTime ?
						1 : (1 - (animationEnvironment.getSimulationTime() - endSimulationTime + hidePropagationTime) / hidePropagationTime));
				graphics.setAlpha((int)alpha);
				super.paint(graphics);
				graphics.popState();
			}
		};
		circle.setFill(false);
		circle.setForegroundColor(ColorFactory.getGoodColor(msg == null ? 0 : msg.getEncapsulationId()));
	}
	
	public void redo() {
		if (isDisplayed())
			getEnclosingModuleFigure().addMessageFigure(circle);
	}

	public void undo() {
		if (isDisplayed())
			getEnclosingModuleFigure().removeMessageFigure(circle);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (isDisplayed()) { 
			Point p = getBeginPoint();
			Point[] ps = getMessageSendPoints(simulationTime, animationTime, 0);
			//System.out.println("Source: " + p + " message:" + ps[0] + ":" + ps[1]);
			double r = ps[0].getDistance(p);
			double width = r - ps[1].getDistance(p);
			//System.out.println("Radius: " + r + " width:" + width);
			int radius = (int)r;
			circle.setLineWidth((int)width);
			circle.setBounds(new Rectangle(p.x - radius, p.y - radius, radius * 2 - 1, radius * 2 - 1));
		}
	}

	protected Point getBeginPoint() {
		return getSubmoduleFigureCenter(sourceModuleId);
	}
	
	protected Point getEndPoint() {
		return getSubmoduleFigureCenter(destinationModuleId);
	}

	protected boolean isDisplayed() {
		// FIXME:
		return true;
	}
}
