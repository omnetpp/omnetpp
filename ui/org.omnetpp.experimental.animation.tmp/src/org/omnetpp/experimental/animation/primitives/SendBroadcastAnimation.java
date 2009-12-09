/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.simulation.model.IRuntimeMessage;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class SendBroadcastAnimation extends AbstractSendMessageAnimation {
	protected int sourceModuleId;

	protected int destinationModuleId;

	protected Ellipse circle;

	protected double hidePropagationTime;

	public SendBroadcastAnimation(ReplayAnimationController animationController,
								  AnimationPosition animationPosition,
								  double propagationTime,
								  double transmissionTime,
								  int sourceModuleId,
								  int destinationModuleId,
								  IRuntimeMessage msg) {
		super(animationController, animationPosition, propagationTime, transmissionTime, msg);
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
		circle.setForegroundColor(ColorFactory.getGoodDarkColor(msg == null ? 0 : msg.getEncapsulationId()));
	}

	@Override
	public void redo() {
		if (isDisplayed())
			getEnclosingModuleFigure().getForegroundDecorationLayer().add(circle);  //XXX was: .addMessageFigure(circle)
	}

	@Override
	public void undo() {
		if (isDisplayed())
			getEnclosingModuleFigure().getForegroundDecorationLayer().add(circle);
	}

	@Override
	public void animateAt(AnimationPosition animationPosition) {
		if (isDisplayed()) {
			Point p = getBeginPoint();
			Point[] ps = getMessageSendPoints(animationPosition, 0);
			//System.out.println("Source: " + p + " message:" + ps[0] + ":" + ps[1]);
			double r = ps[0].getDistance(p);
			double width = Math.max(10, r - ps[1].getDistance(p));
			//System.out.println("Radius: " + r + " width:" + width);
			int radius = (int)r;
			circle.setLineWidth((int)width);
			circle.setBounds(new Rectangle(p.x - radius, p.y - radius, radius * 2 - 1, radius * 2 - 1));
		}
	}

	@Override
	protected Point getBeginPoint() {
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(sourceModuleId));
	}

	@Override
	protected Point getEndPoint() {
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(destinationModuleId));
	}

	protected boolean isDisplayed() {
		// FIXME:
		return true;
	}
}
