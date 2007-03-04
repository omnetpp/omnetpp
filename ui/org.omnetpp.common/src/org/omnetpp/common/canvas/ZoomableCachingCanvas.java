package org.omnetpp.common.canvas;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.util.GeomUtils;

/**
 * Extends CachingCanvas with zoom handling capabilities. Dragging and
 * mouse wheel zooming behaviours also added.
 *
 * @author andras
 */
//FIXME add tooltip support
public abstract class ZoomableCachingCanvas extends CachingCanvas {

	private static final int MAXPIX = Integer.MAX_VALUE / 2;
	
	private double zoomX = 0; // pixels per coordinate unit
	private double zoomY = 0; // pixels per coordinate unit
	
	private double minX, maxX;
	private double minY, maxY;

	/**
     * Constructor.
     */
	public ZoomableCachingCanvas(Composite parent, int style) {
		super(parent, style);
	}

	public void setArea(double minX, double minY, double maxX, double maxY) {
		this.minX = minX;
		this.minY = minY;
		this.maxX = Math.max(minX, maxX);
		this.maxY = Math.max(minY, maxY);
		updateVirtualSize();
		System.out.printf("Area set: (%g, %g, %g, %g) - virtual size: (%d, %d)\n", this.minX, this.maxX, this.minY, this.maxY, getVirtualWidth(), getVirtualHeight());
	}

	public double getMaxX() {
		return maxX;
	}

	public double getMaxY() {
		return maxY;
	}

	public double getMinX() {
		return minX;
	}

	public double getMinY() {
		return minY;
	}

	protected int getLeftInset() {
		return getViewportRectangle().x - getClientArea().x;
	}

	protected int getTopInset() {
		return getViewportRectangle().y - getClientArea().y;
	}
	
	protected Insets getInsets() {
		org.eclipse.swt.graphics.Rectangle clientArea = getClientArea();
		org.eclipse.swt.graphics.Rectangle viewport = getViewportRectangle();
		return new Insets(
				viewport.y - clientArea.y, 
				viewport.x - clientArea.x,
				clientArea.y + clientArea.height - viewport.y - viewport.height,
				clientArea.x + clientArea.width - viewport.x - viewport.width);
	}

	public double fromCanvasX(int x) {
		return (x + getViewportLeft() - getLeftInset()) / zoomX + minX;
	}

	public double fromCanvasY(int y) {
		return maxY - (y + getViewportTop() - getTopInset()) / zoomY;
	}

	public double fromCanvasDistX(int x) {
		return x / zoomX;
	}

	public double fromCanvasDistY(int y) {
		return y / zoomY;
	}
	
	public int toCanvasX(double xCoord) {
		double x = (xCoord - minX)*zoomX - getViewportLeft() + getLeftInset();
		return x<-MAXPIX ? -MAXPIX : x>MAXPIX ? MAXPIX : (int)x;
	}

	public int toCanvasY(double yCoord) {
		double y = (maxY - yCoord)*zoomY - getViewportTop() + getTopInset();
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
	
	public double getViewportCenterCoordX() {
		int middleX = getViewportWidth() / 2;
		return fromCanvasX(middleX + getLeftInset());
	}

	public double getViewportCenterCoordY() {
		int middleY = getViewportHeight() / 2;
		return fromCanvasY(middleY + getTopInset());
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
		updateVirtualSize();
		centerXOn(x);
		redraw();
		System.out.println("zoomX set to "+zoomX);
	}

	public void setZoomY(double zoom) {
		double y = getViewportCenterCoordY();
		zoomY  = zoom;
		updateVirtualSize();
		centerYOn(y);
		redraw();
		System.out.println("zoomY set to "+zoomY);
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
		checkViewportSize();
		double w = maxX - minX;
		zoomX = getViewportWidth() / (w==0 ? 1.0 : w);
		updateVirtualSize();
		scrollHorizontalTo(0);
	}

	public void zoomToFitY() {
		checkViewportSize();
		double h = maxY - minY;
		zoomY = getViewportHeight() / (h==0 ? 1.0 : h);
		updateVirtualSize();
		scrollVerticalTo(0);
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
	
	private void checkSize() {
		if (getVirtualWidth() == 0 || getVirtualHeight() == 0)
			throw new IllegalStateException("virtual size is zero (not yet set?)");
		if (zoomX == 0 || zoomY == 0)
			throw new IllegalStateException("zoomX or zoomY is zero");
	}

	private void checkViewportSize() {
		if (getViewportWidth() == 0 || getViewportHeight() == 0)
			throw new IllegalStateException("viewport size is zero (not yet set?)");
	}

	protected void updateVirtualSize() {
		double w = (maxX - minX)*zoomX;
		double h = (maxY - minY)*zoomY;
		setVirtualSize((long)w, (long)h);
		clearCanvasCache();
	}

	public void clearCanvasCacheAndRedraw() {
		clearCanvasCache();
		redraw();
	}

}
