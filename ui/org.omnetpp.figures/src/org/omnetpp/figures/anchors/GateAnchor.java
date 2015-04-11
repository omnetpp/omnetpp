/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.anchors;

import org.eclipse.draw2d.ChopboxAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Provides support for Ned Model specific behaviors ie: gatename
 *
 * @author rhornig
 */
public class GateAnchor extends ChopboxAnchor {
    public GateAnchor(IFigure owner) {
        super(owner);
    }

    @Override
    protected Rectangle getBox() {
        return getOwner() instanceof IAnchorBounds ? ((IAnchorBounds)getOwner()).getAnchorBounds() : super.getBox();
    }
}
