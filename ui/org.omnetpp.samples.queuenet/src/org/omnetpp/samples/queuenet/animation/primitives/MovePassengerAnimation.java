/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.samples.queuenet.animation.primitives;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.AbstractInfiniteAnimation;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.MessageModel;

public class MovePassengerAnimation extends AbstractInfiniteAnimation {
    private MessageModel message;

    public MovePassengerAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, MessageModel message) {
        super(animationController, eventNumber, simulationTime);
        this.message = message;
    }

    @Override
    public void refreshAnimation(AnimationPosition animationPosition) {
        BigDecimal simulationTime = animationPosition.getSimulationTime();
        BigDecimal sendingTime = message.getSendingTime();
        BigDecimal arrivalTime = message.getArrivalTime();
        double alpha = simulationTime.subtract(sendingTime).doubleValue() / arrivalTime.subtract(sendingTime).doubleValue();
        IFigure figure = animationController.getFigure(message, ImageFigure.class);

    }
}
