package org.omnetpp.animation.editors;

import org.omnetpp.animation.widgets.AnimationCanvas;

/**
 * Interface for editor and view classes that show an animation canvas.
 */
public interface IAnimationCanvasProvider {
    /**
     * Returns the associated animation canvas of the editor or view.
     */
    public AnimationCanvas getAnimationCanvas();
}
