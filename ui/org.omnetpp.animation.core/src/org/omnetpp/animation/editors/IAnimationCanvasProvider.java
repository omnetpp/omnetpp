/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.animation.editors;

import org.omnetpp.animation.widgets.AnimationCanvas;

/**
 * Interface for editor and view classes that show an animation canvas.
 *
 * @author levy
 */
public interface IAnimationCanvasProvider {
    /**
     * Returns the associated animation canvas of the editor or view.
     */
    public AnimationCanvas getAnimationCanvas();
}
