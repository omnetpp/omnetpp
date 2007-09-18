package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.Viewport;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalEditPart;
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
        reveal();
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
	
	
	@InUIThread
	public void reveal() {
	    // copied from ScrollingGraphicalViewer.reveal(EditPart)
	    IFigure target = getFigure();
	    Viewport port = ((FigureCanvas)getCanvas()).getViewport();
	    Rectangle exposeRegion = target.getBounds().getCopy();
	    target = target.getParent();
	    while (target != null && target != port) {
	        target.translateToParent(exposeRegion);
	        target = target.getParent();
	    }
	    exposeRegion.expand(5, 5);
	    
	    Dimension viewportSize = port.getClientArea().getSize();

	    Point topLeft = exposeRegion.getTopLeft();
	    Point bottomRight = exposeRegion.getBottomRight().translate(viewportSize.getNegated());
	    Point finalLocation = new Point();
	    if (viewportSize.width < exposeRegion.width)
	        finalLocation.x = Math.min(bottomRight.x, Math.max(topLeft.x, port.getViewLocation().x));
	    else
	        finalLocation.x = Math.min(topLeft.x, Math.max(bottomRight.x, port.getViewLocation().x));

	    if (viewportSize.height < exposeRegion.height)
	        finalLocation.y = Math.min(bottomRight.y, Math.max(topLeft.y, port.getViewLocation().y));
	    else
	        finalLocation.y = Math.min(topLeft.y, Math.max(bottomRight.y, port.getViewLocation().y));

	    ((FigureCanvas)getCanvas()).scrollTo(finalLocation.x, finalLocation.y); 
	}
		
}
