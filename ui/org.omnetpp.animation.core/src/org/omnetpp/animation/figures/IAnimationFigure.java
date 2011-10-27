package org.omnetpp.animation.figures;

/**
 * Interface for animation figures displayed by the animation canvas.
 */
public interface IAnimationFigure {
    /**
     * Returns the model object that this figure represents.
     */
    public Object getModel();

    /**
     * Refreshes the visual representation of the model according to the current state of the model object.
     */
    public void refesh();
}
