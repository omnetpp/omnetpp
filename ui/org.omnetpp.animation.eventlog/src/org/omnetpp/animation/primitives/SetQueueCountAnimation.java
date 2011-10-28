package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.figures.QueueFigure;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.QueueModel;

public class SetQueueCountAnimation extends AbstractAnimationPrimitive {
    private QueueModel queue;

    private int count;

    public SetQueueCountAnimation(AnimationController animationController, long beginEventNumber, long endEventNumber, BigDecimal beginSimulationTime, BigDecimal endSimulationTime, QueueModel name, int count) {
        super(animationController);
        setSourceEventNumber(endEventNumber);
        setBeginEventNumber(beginEventNumber);
        setEndEventNumber(endEventNumber);
        setBeginSimulationTime(beginSimulationTime);
        setEndSimulationTime(endSimulationTime);
        setFrameRelativeBeginAnimationTime(0);
        setFrameRelativeEndAnimationTime(0);
        this.queue = name;
        this.count = count;
    }

    @Override
    public void refreshAnimation(AnimationPosition animationPosition) {
        queue.setCount(count);
        QueueFigure figure = (QueueFigure)animationController.getFigure(queue, QueueFigure.class);
        figure.refeshAppearance();
    }
}
