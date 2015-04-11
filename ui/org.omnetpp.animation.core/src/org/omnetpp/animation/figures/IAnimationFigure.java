/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.figures;

/**
 * Interface for animation figures displayed by the animation canvas.
 *
 * @author levy
 */
public interface IAnimationFigure {
    /**
     * Returns the model that this figure represents.
     */
    public Object getModel();

    /**
     * Refreshes the visual appearance according to the current model.
     */
    public void refeshAppearance();
}
