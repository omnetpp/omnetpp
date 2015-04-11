/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.figures.NameValue;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.VectorResult;

public class CreateVectorAnimation extends AbstractInfiniteAnimation {
    private String name;

	public CreateVectorAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, String name) {
		super(animationController, eventNumber, simulationTime);
		this.name = name;
	}

	@Override
	public void activate() {
	    super.activate();
	    IFigure figure = new NameValue(name);
        animationController.setFigure(name, VectorResult.class, figure);
	    animationController.getAnimationCanvas().addFigure(figure);
	}

	@Override
	public void deactivate() {
	    super.deactivate();
        IFigure figure = animationController.getFigure(name, VectorResult.class);
        animationController.setFigure(name, VectorResult.class, null);
        animationController.getAnimationCanvas().removeFigure(figure);
	}
}
