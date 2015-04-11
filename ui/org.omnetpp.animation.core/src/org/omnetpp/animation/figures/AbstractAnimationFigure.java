/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Figure;

/**
 * Abstract base class for animation figures.
 *
 * @author levy
 */
public abstract class AbstractAnimationFigure extends Figure implements IAnimationFigure {
    protected Object model;

    public AbstractAnimationFigure(Object model) {
        this.model = model;
    }

    public Object getModel() {
        return model;
    }
}
