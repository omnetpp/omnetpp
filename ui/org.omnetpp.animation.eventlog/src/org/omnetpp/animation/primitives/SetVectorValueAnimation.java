package org.omnetpp.animation.primitives;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.figures.NameValue;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.VectorResult;

public class SetVectorValueAnimation extends AbstractAnimationPrimitive {
    private String name;

    private double value;

    public SetVectorValueAnimation(AnimationController animationController, long beginEventNumber, long endEventNumber, BigDecimal beginSimulationTime, BigDecimal endSimulationTime, String name, double value) {
        super(animationController);
        setSourceEventNumber(endEventNumber);
        setBeginEventNumber(beginEventNumber);
        setEndEventNumber(endEventNumber);
        setBeginSimulationTime(beginSimulationTime);
        setEndSimulationTime(endSimulationTime);
        setFrameRelativeBeginAnimationTime(0);
        setFrameRelativeEndAnimationTime(0);
        this.name = name;
        this.value = value;
    }

    @Override
    public void refreshAnimation(AnimationPosition animationPosition) {
        NameValue nameValueFigure = (NameValue)animationController.getFigure(name, VectorResult.class);
        nameValueFigure.setValue(value);
    }
}
