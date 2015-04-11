/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.scave.charting.ChartCanvas;

import com.simulcraft.test.gui.access.CanvasAccess;
import com.simulcraft.test.gui.core.UIStep;

public class ChartCanvasAccess extends CanvasAccess {

    public ChartCanvasAccess(ChartCanvas canvas) {
        super(canvas);
    }

    @Override
    public ChartCanvas getControl() {
        return (ChartCanvas)widget;
    }

    @UIStep
    public Rectangle getViewportRectangle() {
        org.eclipse.draw2d.geometry.Rectangle rect = getControl().getViewportRectangle();
        return new Rectangle(rect.x, rect.y, rect.width, rect.height);
    }

    @UIStep
    public Rectangle getViewportRectangleAbsolute() {
        org.eclipse.draw2d.geometry.Rectangle rect = getControl().getViewportRectangle();
        Point tmp = getControl().toDisplay(rect.x, rect.y);
        return new Rectangle(tmp.x, tmp.y, rect.width, rect.height);
    }
}
