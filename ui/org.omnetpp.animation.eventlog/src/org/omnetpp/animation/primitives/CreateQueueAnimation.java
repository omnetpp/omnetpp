/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.figures.QueueFigure;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.QueueModel;

public class CreateQueueAnimation extends AbstractInfiniteAnimation {
    private QueueModel queue;

	public CreateQueueAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, QueueModel queueModel) {
		super(animationController, eventNumber, simulationTime);
		this.queue = queueModel;
	}

	@Override
	public void activate() {
	    super.activate();
	    IFigure figure = new QueueFigure(queue);
        animationController.setFigure(queue, figure);
	    animationController.getAnimationCanvas().addFigure(figure);
	}

	@Override
	public void deactivate() {
	    super.deactivate();
        IFigure figure = animationController.getFigure(queue, QueueFigure.class);
        animationController.setFigure(queue, QueueFigure.class, null);
        animationController.getAnimationCanvas().removeFigure(figure);
	}
}
