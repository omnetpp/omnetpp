/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogMessage;

public class SendBroadcastAnimation extends AbstractSendMessageAnimation {
	protected int sourceModuleId;

	protected int destinationModuleId;

	protected Ellipse circle;

	protected double fadeOutTime;

	public SendBroadcastAnimation(AnimationController animationController, BigDecimal propagationDelay, BigDecimal transmissionDelay, int sourceModuleId, int destinationModuleId, EventLogMessage message) {
		super(animationController, propagationDelay, transmissionDelay, message);
		this.sourceModuleId = sourceModuleId;
		this.destinationModuleId = destinationModuleId;
		this.fadeOutTime = propagationDelay.doubleValue() * 0.2;
		circle = new Ellipse();
		circle.setFill(false);
		circle.setForegroundColor(ColorFactory.getGoodDarkColor(message == null ? 0 : message.getEncapsulationId()));
	}

	@Override
	public void activate() {
	    super.activate();
	    getEnclosingCompoundModuleFigure(destinationModuleId).getForegroundDecorationLayer().add(circle);
	}

	@Override
	public void deactivate() {
        super.deactivate();
        getEnclosingCompoundModuleFigure(destinationModuleId).getForegroundDecorationLayer().remove(circle);
	}

	@Override
	public void refreshAnimation(AnimationPosition animationPosition) {
		Point p = getBeginPoint().getCopy();
		Point[] ps = getMessageSendPoints(animationPosition, 0);
		//System.out.println("Source: " + p + " message: " + ps[0] + " - " + ps[1]);
		double r = ps[0].getDistance(p);
		double width = Math.max(10, r - ps[1].getDistance(p));
		//System.out.println("Radius: " + r + " width:" + width);
		int radius = (int)r;
        BigDecimal currentSimulationTime = animationController.getAnimationPosition().getSimulationTime();
        double alpha = 64 * (currentSimulationTime.doubleValue() < getEndSimulationTime().doubleValue() - fadeOutTime ?
                1 : (1 - (currentSimulationTime.subtract(getEndSimulationTime()).doubleValue() + fadeOutTime) / fadeOutTime));
        circle.setAlpha((int)alpha);
		circle.setLineWidth((int)width);
		Rectangle bounds = new Rectangle(p.x - radius, p.y - radius, radius * 2 - 1, radius * 2 - 1);
		//System.out.println("Bounds: " + bounds);
        circle.setBounds(bounds);
	}

	@Override
	protected Point getBeginPoint() {
	    // TODO: this should not calculate with the decoration icons
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(sourceModuleId));
	}

	@Override
	protected Point getEndPoint() {
        // TODO: this should not calculate with the decoration icons
		return getSubmoduleFigureCenter(getAncestorUnderParentModule(destinationModuleId));
	}
}
