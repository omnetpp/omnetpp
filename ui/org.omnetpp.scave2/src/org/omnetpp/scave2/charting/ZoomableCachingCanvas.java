package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Cursor;
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
//FIXME insets should be taken into account by coord calculation functions and zooming functions
public abstract class ZoomableCachingCanvas extends CachingCanvas {

	private static final Cursor DRAG_CURSOR = new Cursor(null, SWT.CURSOR_SIZEALL);
	private static final int MAXPIX = Integer.MAX_VALUE / 2;
	
	private double zoomX = 100; // pixels per coordinate unit
	private double zoomY = 100; // pixels per coordinate unit
	
	private int dragStartX;
	private int dragStartY;

	protected double minX, maxX;  //XXX make private
	protected double minY, maxY;  //XXX make private

	private Insets insets;  // size of margins used for axis drawing
	
	/**
     * Constructor.
     */
	public ZoomableCachingCanvas(Composite parent, int style) {
		super(parent, style);
    	setUpMouseHandling();
	}

	public double fromCanvasX(int x) {
		return (x + getViewportLeft()) / zoomX + minX;
	}

	public double fromCanvasY(int y) {
		return (y + getViewportTop()) / zoomY + minY;
	}

	public double fromCanvasDistX(int x) {
		return x / zoomX + minX;
	}

	public double fromCanvasDistY(int y) {
		return y / zoomY + minY;
	}
	
	public int toCanvasX(double xCoord) {
		long x = (long)((xCoord - minX)*zoomX) - getViewportLeft();
		return x<-MAXPIX ? -MAXPIX : x>MAXPIX ? MAXPIX : (int)x;
	}

	public int toCanvasY(double yCoord) {
		long y = (long)((yCoord - minY)*zoomY) - getViewportTop();
		return y<-MAXPIX ? -MAXPIX : y>MAXPIX ? MAXPIX : (int)y;
	}

	public int toCanvasDistX(double xCoord) {
		long x = (long)((xCoord - minX)*zoomX);
		return x<-MAXPIX ? -MAXPIX : x>MAXPIX ? MAXPIX : (int)x;
	}

	public int toCanvasDistY(double yCoord) {
		long y = (long)((yCoord - minY)*zoomY) - getViewportTop();
		return y<-MAXPIX ? -MAXPIX : y>MAXPIX ? MAXPIX : (int)y;
	}
	
	public double getViewportCenterCoordX() {
		int middleX = getWidth() / 2;
		return fromCanvasX(middleX);
	}

	public double getViewportCenterCoordY() {
		int middleY = getHeight() / 2;
		return fromCanvasY(middleY);
	}
	
	public void centerXOn(double xCoord) {
		scrollHorizontalTo((long)((xCoord - minX)*zoomX) - getViewportLeft());
	}

	public void centerYOn(double yCoord) {
		scrollVerticalTo((long)((yCoord - minY)*zoomY) - getViewportTop());
	}
	
	public void setZoomX(double zoom) {
		double x = getViewportCenterCoordX();
		zoomX  = zoom;
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
		int width = getWidth()==0 ? 300 : getWidth(); // zero before canvas gets layouted
		setZoomX(width / (maxX - minX));
	}

	public void zoomToFitY() {
		int height = getHeight()==0 ? 200 : getHeight(); // zero before canvas gets layouted
		setZoomY(height / (maxY - minY));
	}
	
	public void zoomToFit() {
		zoomToFitX();
		zoomToFitY();
	}

	public void zoomToRectangle(Rectangle rectangle) {
		// TODO
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
					scrollHorizontalTo(getViewportLeft() - getWidth() * event.count / 20);
				}
			}
		});
		// dragging ("hand" cursor) and tooltip
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
				if (dragStartX != -1 && dragStartY != -1 && (e.stateMask & SWT.BUTTON_MASK) != 0 && (e.stateMask & SWT.MODIFIER_MASK) == 0)
					mouseDragged(e);
				else {
					setCursor(null); // restore cursor at end of drag (must do it here too, because we 
									 // don't get the "released" event if user releases mouse outside the canvas)
					redraw();
				}
			}

			private void mouseDragged(MouseEvent e) {
				// scroll by the amount moved since last drag call
				int dx = e.x - dragStartX;
				int dy = e.y - dragStartY;
				scrollHorizontalTo(getViewportLeft() - dx);
				scrollVerticalTo(getViewportTop() - dy);
				dragStartX = e.x;
				dragStartY = e.y;
			}
		});
	}
}
