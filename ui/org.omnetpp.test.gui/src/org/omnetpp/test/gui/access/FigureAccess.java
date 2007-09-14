package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.test.gui.core.InUIThread;

public class FigureAccess
	extends ClickableAccess
{
	protected IFigure figure;
	
	public FigureAccess(IFigure figure) {
		this.figure = figure;
	}
	
	public IFigure getFigure() {
        return figure;
    }

	public IFigure getRootFigure() {
		IFigure currentFigure = figure;

		while (currentFigure.getParent() != null)
			currentFigure = currentFigure.getParent();

		return currentFigure;
	}

	public Canvas getCanvas() {
		IFigure rootFigure = getRootFigure();
		LightweightSystem lightweightSystem = (LightweightSystem)ReflectionUtils.getFieldValue(rootFigure, "this$0");
		return (Canvas)ReflectionUtils.getFieldValue(lightweightSystem, "canvas");
	}

	@InUIThread
	public void click(int button) {
	    click(button, getCenter(getAbsoluteBounds()));
	}
	
    @InUIThread
    public void click(int button, org.eclipse.swt.graphics.Point point) {
        click(button, point.x, point.y);
    }

    @InUIThread
    public void click(int button, int x, int y) {
        clickAbsolute(button, getCanvas().toDisplay(x, y));
    }	

    public Rectangle getAbsoluteBounds() {
        Rectangle r = figure.getBounds().getCopy();
		figure.translateToAbsolute(r);
        return r;
    }

	protected org.eclipse.swt.graphics.Point toDisplay(Point point) {
		return toDisplay(point.x, point.y);
	}
	
	protected org.eclipse.swt.graphics.Point toDisplay(org.eclipse.swt.graphics.Point point) {
		return toDisplay(point.x, point.y);
	}

	protected org.eclipse.swt.graphics.Point toDisplay(int x, int y) {
		Point point = new Point(x, y);
		figure.translateToAbsolute(point);
		return getCanvas().toDisplay(point.x, point.y);
	}
}
