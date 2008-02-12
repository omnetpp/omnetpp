package org.omnetpp.common.canvas;


import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.widgets.Composite;

/**
 * Extends CachingCanvas with zoom handling capabilities. Dragging and mouse wheel 
 * zooming behaviours can be added via ZoomableCanvasMouseSupport.
 *
 * @author andras
 */
//FIXME zooming in repeatedly eventually causes BigDecimal Underflow in tick painting. Set reasonable limit for zooming!
public abstract class ZoomableCachingCanvas extends CachingCanvas implements ICoordsMapping {

	public static final String PROP_ZOOM_X = "zoomX";
	public static final String PROP_ZOOM_Y = "zoomY";
	
	private double zoomX = 0; // pixels per coordinate unit
	private double zoomY = 0; // pixels per coordinate unit
	
	private double minX = 0, maxX = 1;
	private double minY = 0, maxY = 1;

	private int numCoordinateOverflows;
	
	/**
     * Constructor.
     */
	public ZoomableCachingCanvas(Composite parent, int style) {
		super(parent, style);
	}
	
	public void setArea(RectangularArea area) {
		setArea(area.minX, area.minY, area.maxX, area.maxY);
	}

	public void setArea(double minX, double minY, double maxX, double maxY) {
		this.minX = minX;
		this.minY = minY;
		this.maxX = Math.max(minX, maxX);
		this.maxY = Math.max(minY, maxY);
		
		// don't allow zero width/height (as it will cause division by zero)
		if (this.minX == this.maxX)  this.maxX = this.minX + 1;
		if (this.minY == this.maxY)  this.maxY = this.minY + 1;
		
		zoomToFit(); // includes updateVirtualSize(), clearCanvasCache(), redraw() etc.
		// System.out.printf("Area set: (%g, %g, %g, %g) - virtual size: (%d, %d)\n", this.minX, this.maxX, this.minY, this.maxY, getVirtualWidth(), getVirtualHeight());
	}
	
	public RectangularArea getArea() {
		return new RectangularArea(minX, minY, maxX, maxY);
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
		Assert.isTrue(-MAXPIX<x && x<MAXPIX);
		return (x + getViewportLeft() - getLeftInset()) / zoomX + minX;
	}

	public double fromCanvasY(int y) {
		Assert.isTrue(-MAXPIX<y && y<MAXPIX);
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
		return toInt(x);
	}

	public int toCanvasY(double yCoord) {
		double y = (maxY - yCoord)*zoomY - getViewportTop() + getTopInset();
		return toInt(y);
	}

	public int toCanvasDistX(double xCoord) {
		double x = xCoord * zoomX;
		return toInt(x);
	}

	public int toCanvasDistY(double yCoord) {
		double y = yCoord * zoomY;
		return toInt(y);
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
	
	public int getNumCoordinateOverflows() {
		return numCoordinateOverflows;
	}

	public void resetCoordinateOverflowCount() {
		numCoordinateOverflows = 0;				
	}

	protected int toInt(double c) {
		return c<-MAXPIX ? -largeValue(c) : c>MAXPIX ? largeValue(c) : Double.isNaN(c) ? NAN_PIX : (int)c;
	}
	
	private int largeValue(double c) {
		if (!Double.isInfinite(c)) // infinite is OK and does not count as coordinate overflow
			numCoordinateOverflows++;
		return MAXPIX; 
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
	
	public double getMinZoomX() {
		checkAreaAndViewPort();
		return getViewportWidth() / (maxX - minX);
	}
	
	public void setZoomX(double zoomX) {
		double minZoomX = getMinZoomX();
		double newZoomX = Math.max(zoomX, minZoomX);
		if (newZoomX != this.zoomX) {
			double oldX = getViewportCenterCoordX();
			double oldZoomX = this.zoomX;
			this.zoomX = newZoomX;
			updateVirtualSize(); // includes clearCache + redraw
			centerXOn(oldX);
			firePropertyChangeEvent(PROP_ZOOM_X, oldZoomX, newZoomX);
			System.out.println("zoomX set to "+zoomX);
		}
	}
	
	public double getMinZoomY() {
		checkAreaAndViewPort();
		return getViewportHeight() / (maxY - minY);
	}

	public void setZoomY(double zoomY) {
		double minZoomY = getMinZoomY();
		double newZoomY = Math.max(zoomY, minZoomY);
		if (newZoomY != this.zoomY) {
			double oldY = getViewportCenterCoordY();
			double oldZoomY = this.zoomY;
			this.zoomY = newZoomY;
			updateVirtualSize(); // includes clearCache + redraw
			centerYOn(oldY);
			firePropertyChangeEvent(PROP_ZOOM_Y, oldZoomY, newZoomY);
			System.out.println("zoomY set to "+zoomY);
		}
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

	public void zoomXBy(double zoomFactor, int canvasX) {
		zoomXBy(zoomFactor);
		centerXOn(fromCanvasX(canvasX));
	}

	public void zoomYBy(double zoomFactor, int canvasY) {
		zoomYBy(zoomFactor);
		centerYOn(fromCanvasY(canvasY));
	}
	
	public void zoomBy(double zoomFactor, int canvasX, int canvasY) {
		zoomXBy(zoomFactor, canvasX);
		zoomYBy(zoomFactor, canvasY);
	}
	
	public void zoomToFitX() {
		setZoomX(getViewportWidth() / (maxX - minX));
	}

	public void zoomToFitY() {
		setZoomY(getViewportHeight() / (maxY - minY));
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
		zoomXBy(((double)getViewportWidth()) / r.width);
		zoomYBy(((double)getViewportHeight()) / r.height);
		
		// position to original top-left corner
		scrollHorizontalTo(toVirtualX(x));
		scrollVerticalTo(toVirtualY(y));
	}
	
	public boolean isZoomedOutX() {
		return zoomX == getViewportWidth() / (maxX - minX);
	}

	public boolean isZoomedOutY() {
		return zoomY == getViewportHeight() / (maxY - minY);
	}

	/**
	 * Ensure canvas is not zoomed out more than possible (area must fill viewport).  
	 */
	public void validateZoom() {
		setZoomX(getZoomX());
		setZoomY(getZoomY());
	}
	
	/**
	 * Called internally whenever zoom or the area changes.
	 */
	private void updateVirtualSize() {
		double w = (maxX - minX)*zoomX;
		double h = (maxY - minY)*zoomY;
		setVirtualSize((long)w, (long)h);
		clearCanvasCache();
		redraw();
	}

	public void clearCanvasCacheAndRedraw() {
		clearCanvasCache();
		redraw();
	}

	private void checkAreaAndViewPort() {
		if (minX == maxX || minY == maxY)
			throw new IllegalStateException("area width/height is zero (setArea() not called yet?)");
		if (getViewportWidth() == 0 || getViewportHeight() == 0)
			throw new IllegalStateException("viewport size is zero (not yet set?)");
	}

	/**
	 * Returns an object for efficient plot-to-canvas coordinate mapping. The returned 
	 * object is intended for *one-time* plotting the chart: it SHOULD BE DISCARDED
	 * at the end of the paint() method, because it captures chart geometry in "final" 
	 * variables which become obsolete once the user scrolls/resizes the chart.
	 */
	protected ICoordsMapping getOptimizedCoordinateMapper() {
		// Unoptimized version (for testing): 
		// return this;
		
		// how this method was created (also hints for maintenance): copy the corresponding
		// methods from ZoomableCachingCanvas, and create "final" variables for all 
		// member accesses and method calls in it.
		final double zoomX = getZoomX();
		final double zoomY = getZoomY();
		final double minX = this.getMinX();
		final double maxX = this.getMaxX();
		final double minY = this.getMinY();
		final double maxY = this.getMaxY();
		final long viewportLeftMinusLeftInset = getViewportLeft() - getLeftInset();
		final long viewportTopMinusTopInset = getViewportTop() - getTopInset();
	
		ICoordsMapping mapping = new ICoordsMapping() {
			private int numOverflows; // counts pixel coordinate overflows
			
			public double fromCanvasX(int x) {
				Assert.isTrue(-MAXPIX<x && x<MAXPIX);
				return (x + viewportLeftMinusLeftInset) / zoomX + minX;
			}
	
			public double fromCanvasY(int y) {
				Assert.isTrue(-MAXPIX<y && y<MAXPIX);
				return maxY - (y + viewportTopMinusTopInset) / zoomY;
			}
	
			public double fromCanvasDistX(int x) {
				return x / zoomX;
			}
	
			public double fromCanvasDistY(int y) {
				return y / zoomY;
			}
			
			public int toCanvasX(double xCoord) {
				double x = (xCoord - minX)*zoomX - viewportLeftMinusLeftInset;
				return toInt(x);
			}
	
			public int toCanvasY(double yCoord) {
				double y = (maxY - yCoord)*zoomY - viewportTopMinusTopInset;
				return toInt(y);
			}
	
			public int toCanvasDistX(double xCoord) {
				double x = xCoord * zoomX;
				return toInt(x);
			}
	
			public int toCanvasDistY(double yCoord) {
				double y = yCoord * zoomY;
				return toInt(y);
			}
			
			private int toInt(double c) {
				return c<-MAXPIX ? -largeValue(c) : c>MAXPIX ? largeValue(c) : Double.isNaN(c) ? NAN_PIX : (int)c;
			}
			
			private int largeValue(double c) {
				if (!Double.isInfinite(c)) // infinite is OK and does not count as coordinate overflow
					numOverflows++;
				return MAXPIX; 
			}
	
			public int getNumCoordinateOverflows() {
				return numOverflows;
			}
	
			public void resetCoordinateOverflowCount() {
				numOverflows = 0;				
			}
		};
		
		// run a mini regression test before we return it
		Assert.isTrue(toCanvasX(minX)==mapping.toCanvasX(minX) && toCanvasX(maxX)==mapping.toCanvasX(maxX));
		Assert.isTrue(toCanvasY(minY)==mapping.toCanvasY(minY) && toCanvasY(maxY)==mapping.toCanvasY(maxY));
		Assert.isTrue(toCanvasDistX(maxX-minX)==mapping.toCanvasDistX(maxX-minX));
		Assert.isTrue(toCanvasDistY(maxY-minY)==mapping.toCanvasDistY(maxY-minY));
		org.eclipse.swt.graphics.Rectangle r = getViewportRectangle();
		Assert.isTrue(zoomX == 0.0 || (fromCanvasX(r.x)==mapping.fromCanvasX(r.x) && fromCanvasX(r.x+r.width)==mapping.fromCanvasX(r.x+r.width)));
		Assert.isTrue(zoomY == 0.0 || (fromCanvasY(r.y)==mapping.fromCanvasY(r.y) && fromCanvasY(r.y+r.height)==mapping.fromCanvasY(r.y+r.height)));
		Assert.isTrue(zoomX == 0.0 || fromCanvasDistX(r.width)==mapping.fromCanvasDistX(r.width));
		Assert.isTrue(zoomY == 0.0 || fromCanvasDistY(r.height)==mapping.fromCanvasDistY(r.height));
	
		return mapping;
	}
	
	public RectangularArea getZoomedArea() {
		return new RectangularArea(
				fromVirtualX(getViewportLeft()),
				fromVirtualY(getViewportBottom()),
				fromVirtualX(getViewportRight()),
				fromVirtualY(getViewportTop()));
	}
	
	public void zoomToArea(RectangularArea area) {
		double minX = Math.max(area.minX, this.minX);
		double minY = Math.max(area.minY, this.minY);
		double maxX = Math.min(area.maxX, this.maxX);
		double maxY = Math.min(area.maxY, this.maxY);
		
		if (minX < maxX && minY < maxY) {
			setZoomX(getViewportWidth() / (maxX - minX));
			setZoomY(getViewportHeight() / (maxY - minY));
			scrollHorizontalTo(toVirtualX(minX));
			scrollVerticalTo(toVirtualY(maxY));
		}
	}
}
