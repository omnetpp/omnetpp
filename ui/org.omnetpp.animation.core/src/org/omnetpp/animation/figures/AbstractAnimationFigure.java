package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Figure;

public abstract class AbstractAnimationFigure extends Figure implements IAnimationFigure {
    protected Object model;

    public AbstractAnimationFigure(Object model) {
        this.model = model;
    }

    public Object getModel() {
        return model;
    }
}
