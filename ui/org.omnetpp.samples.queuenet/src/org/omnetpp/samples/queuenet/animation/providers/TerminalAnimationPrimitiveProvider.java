package org.omnetpp.samples.queuenet.animation.providers;

import java.util.ArrayList;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;

public class TerminalAnimationPrimitiveProvider implements IAnimationPrimitiveProvider {
    private AnimationController animationController;

    public TerminalAnimationPrimitiveProvider() {
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    @Override
    public ArrayList<IAnimationPrimitive> loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        // TODO:
        return animationPrimitives;
    }
}
