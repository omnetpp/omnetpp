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
        return getControl().getViewportRectangle();
    }

    @UIStep
    public Rectangle getViewportRectangleAbsolute() {
        Rectangle rect = getControl().getViewportRectangle();
        Point tmp = getControl().toDisplay(rect.x, rect.y);
        return new Rectangle(tmp.x, tmp.y, rect.width, rect.height);
    }
}
