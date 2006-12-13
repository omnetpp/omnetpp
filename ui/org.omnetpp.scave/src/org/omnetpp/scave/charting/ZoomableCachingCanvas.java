package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.canvas.CachingCanvas;

/**
 * Extends CachingCanvas with zoom handling capabilities. Dragging and
 * mouse wheel zooming behaviours also added.
 *
 * @author andras
 */
//FIXME while zooming with Ctrl+wheel too fast, one can get "event loop exception: SWTError: No more handles"
//FIXME add tooltip support
public abstract class ZoomableCachingCanvas extends CachingCanvas {

	private static final Cursor DRAG_CURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
	private static final int MAXPIX = Integer.MAX_VALUE / 2;
	
	private double zoomX = 0; // pixels per coordinate unit
	private double zoomY = 0; // pixels per coordinate unit
	
	private int dragStartX;
	private int dragStartY;

	protected double minX, maxX;  //XXX make private and add getArea()
	protected double minY, maxY;  //XXX make private and add getArea()

	private Insets insets;  // size of margins used for axis drawing
	
	/**
     * Constructor.
     */
	public ZoomableCachingCanvas(Composite parent, int style) {
		super(parent, style);
    	setUpMouseHandling();
	}

	public double fromCanvasX(int x) {
		return (x + getViewportLeft() - insets.left) / zoomX + minX;
	}

	public double fromCanvasY(int y) {
		return maxY - (y + getViewportTop() - insets.top) / zoomY;
	}

	public double fromCanvasDistX(int x) {
		return x / zoomX;
	}

	public double fromCanvasDistY(int y) {
		return y / zoomY;
	}
	
	public int toCanvasX(double xCoord) {
		double x = (xCoord - minX)*zoomX - getViewportLeft() + insets.left;
		return x<-MAXPIX ? -MAXPIX : x>MAXPIX ? MAXPIX : (int)x;
	}

	public int toCanvasY(double yCoord) {
		double y = (maxY - yCoord)*zoomY - getViewportTop() + insets.top;
		return y<-MAXPIX ? -MAXPIX : y>MAXPIX ? MAXPIX : (int)y;
	}

	public int toCanvasDistX(double xCoord) {
		double x = xCoord * zoomX;
		return x<-MAXPIX ? -MAXPIX : x>MAXPIX ? MAXPIX : (int)x;
	}

	public int toCanvasDistY(double yCoord) {
		double y = yCoord * zoomY;
		return y<-MAXPIX ? -MAXPIX : y>MAXPIX ? MAXPIX : (int)y;
	}

	public long toVirtualX(double xCoord) {
		double x = (xCoord - minX)*zoomX;
		return (long)x;
	}

	public long toVirtualY(double yCoord) {
		double y = (maxY - yCoord)*zoomY;
		return (long)y;
	}

	public double fromVirtualX(long x) {
		return x / zoomX + minX;
	}

	public double fromVirtualY(long y) {
		return maxY - y / zoomY;
	}
	
	
	
	@Override
	public org.eclipse.swt.graphics.Rectangle getViewportRectangle() {
		Rectangle rect = new Rectangle(getClientArea()).crop(insets);
		return new org.eclipse.swt.graphics.Rectangle(
				rect.x, rect.y, rect.width, rect.height);
	}

	public int getViewportWidth() {
		return getClientArea().width - insets.getWidth();
	}
	
	public int getViewportHeight() {
		return getClientArea().height - insets.getHeight();
	}
	
	public double getViewportCenterCoordX() {
		int middleX = getViewportWidth() / 2;
		return fromCanvasX(middleX + insets.left);
	}

	public double getViewportCenterCoordY() {
		int middleY = getViewportHeight() / 2;
		return fromCanvasY(middleY + insets.top);
	}
	
	public void centerXOn(double xCoord) {
		scrollHorizontalTo(toVirtualX(xCoord) - getViewportWidth()/2);
	}

	public void centerYOn(double yCoord) {
		scrollVerticalTo(toVirtualY(yCoord) - getViewportHeight()/2);
	}
	
	public void setZoomX(double zoom) {
		double x = getViewportCenterCoordX();
		zoomX = zoom;
		calculateVirtualSize();
		centerXOn(x);
		clearCanvasCacheAndRedraw();
	}

	public void setZoomY(double zoom) {
		double y = getViewportCenterCoordY();
		zoomY  = zoom;
		calculateVirtualSize();
		centerYOn(y);
		clearCanvasCacheAndRedraw();
	}
	
	public double getZoomX() {
		return zoomX;
	}

	public double getZoomY() {
		return zoomY;
	}
	
	public void zoomXBy(double zoomFactor) {
		setZoomX(zoomX * zoomFactor);	
	}

	public void zoomYBy(double zoomFactor) {
		setZoomY(zoomY * zoomFactor);	
	}

	public void zoomBy(double zoomFactor) {
		zoomXBy(zoomFactor);
		zoomYBy(zoomFactor);
	}
	
	public void zoomToFitX() {
		zoomX = 0;  // means "zoom out completely"; will be done by zoom validation in beforePaint()
	}

	public void zoomToFitY() {
		zoomY = 0;  // means "zoom out completely"; will be done by zoom validation in beforePaint()
	}
	
	public void zoomToFit() {
		zoomToFitX();
		zoomToFitY();
	}

	public void zoomToRectangle(Rectangle r) {
		// remember top-left corner
		double x = fromCanvasX(r.x);
		double y = fromCanvasY(r.y);

		// adjust zoom
		zoomXBy((getViewportWidth()) / r.width);
		zoomYBy((getViewportHeight()) / r.height);
		
		// position to original top-left corner
		scrollHorizontalTo(toVirtualX(x));
		scrollVerticalTo(toVirtualY(y));
	}
	
	public void setArea(double minX, double minY, double maxX, double maxY) {
		this.minX = minX;
		this.minY = minY;
		this.maxX = Math.max(minX, maxX);
		this.maxY = Math.max(minY, maxY);
		calculateVirtualSize();
	}

	protected void calculateVirtualSize() {
		double w = (maxX - minX)*zoomX;
		double h = (maxY - minY)*zoomY;
		setVirtualSize((long)w, (long)h);
	}

	public Insets getInsets() {
		return insets;
	}

	public void setInsets(Insets insets) {
		this.insets = insets;
	}

	public void clearCanvasCacheAndRedraw() {
		clearCanvasCache();
		redraw();
	}

	@Override
	protected void beforePaint(GC gc) {
		// validate zoom, so that one cannot zoom out too much (the content (getArea()) must cover full canvas)
		double w = maxX - minX;
		double minZoomX = (getViewportWidth()) / (w==0 ? 1.0 : w); 
		if (zoomX < minZoomX) {
			zoomX = minZoomX;
			calculateVirtualSize();
		}

		double h = maxY - minY;
		double minZoomY = (getViewportHeight()) / (h==0 ? 1.0 : h); 
		if (zoomY < minZoomY){
			zoomY = minZoomY;
			calculateVirtualSize();
		}
	}

	/**
	 * Sets up default mouse handling.
	 */
	protected void setUpMouseHandling() {
		addListener(SWT.MouseWheel, new Listener() {
			public void handleEvent(Event event) {
				if ((event.stateMask & SWT.CTRL)!=0) {
					for (int i = 0; i < event.count; i++)
						zoomBy(1.1);
	
					for (int i = 0; i < -event.count; i++)
						zoomBy(1.0 / 1.1);
				}
				else if ((event.stateMask & SWT.SHIFT)!=0) {
					scrollHorizontalTo(getViewportLeft() - getViewportWidth() * event.count / 20);
				}
			}
		});
		// dragging ("hand" cursor)
		addMouseListener(new MouseListener() {
			public void mouseDoubleClick(MouseEvent e) {}
			public void mouseDown(MouseEvent e) {
				setFocus();

				if (e.button == 1) {
					setCursor(DRAG_CURSOR);
					dragStartX = e.x;
					dragStartY = e.y;
				}
			}
			public void mouseUp(MouseEvent e) {
				setCursor(null); // restore cursor at end of drag
				dragStartX = dragStartY = -1;
			}
    	});
		addMouseMoveListener(new MouseMoveListener() {
			public void mouseMove(MouseEvent e) {
				if ((e.stateMask & SWT.BUTTON_MASK)!=0) { // drag with any mouse button being held down
					mouseDragged(e);
				} 
				else { // plain move
					setCursor(null); // restore cursor at end of drag (must do it here too, because we 
									 // don't get the "released" event if user releases mouse outside the canvas)
				}
			}

			private void mouseDragged(MouseEvent e) {
				// if mouse button is pressed with no modifier key: drag the chart
				if ((e.stateMask & SWT.MODIFIER_MASK)==0 && dragStartX!=-1 && dragStartY!=-1) {
					// scroll by the amount moved since last drag call
					int dx = e.x - dragStartX;
					int dy = e.y - dragStartY;
					scrollHorizontalTo(getViewportLeft() - dx);
					scrollVerticalTo(getViewportTop() - dy);
					dragStartX = e.x;
					dragStartY = e.y;
				}
			}
		});
	}
}
