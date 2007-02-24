package org.omnetpp.common.canvas;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Composite;

/**
 * Extends CachingCanvas with zoom handling capabilities. Dragging and
 * mouse wheel zooming behaviours also added.
 *
 * @author andras
 */
//FIXME while zooming with Ctrl+wheel too fast, one can get "event loop exception: SWTError: No more handles"
//FIXME add tooltip support
public abstract class ZoomableCachingCanvas extends CachingCanvas {

	private static final int MAXPIX = Integer.MAX_VALUE / 2;
	
	private double zoomX = 0; // pixels per coordinate unit
	private double zoomY = 0; // pixels per coordinate unit
	

	protected double minX, maxX;  //XXX make private and add getArea()
	protected double minY, maxY;  //XXX make private and add getArea()

	private Insets insets;  // size of margins used for axis drawing
	
	/**
     * Constructor.
     */
	public ZoomableCachingCanvas(Composite parent, int style) {
		super(parent, style);
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

	public void zoomBy(double zoomFactor, int aroundCanvasX, int aroundCanvasY) {
		System.out.println("zoom around point not implemented. x="+aroundCanvasX+", y="+aroundCanvasY); //FIXME
		zoomBy(zoomFactor);
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
}
