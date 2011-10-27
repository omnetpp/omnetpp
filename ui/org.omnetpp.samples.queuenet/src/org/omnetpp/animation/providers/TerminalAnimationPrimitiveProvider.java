package org.omnetpp.animation.providers;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.eventlog.engine.IEvent;

public class TerminalAnimationPrimitiveProvider implements IAnimationPrimitiveProvider {
    private AnimationController animationController;

    public TerminalAnimationPrimitiveProvider() {
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    @Override
    public List<IAnimationPrimitive> collectAnimationPrimitivesForEvents(IEvent beginEvent, IEvent endEvent) {
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        // TODO:
        return animationPrimitives;
    }
}
