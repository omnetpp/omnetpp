package org.omnetpp.common.canvas;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Sash;
import org.eclipse.swt.widgets.ScrollBar;
import org.omnetpp.common.canvas.ITileCache.Tile;

/**
 * A scrollable canvas that supports caching of (part of) the drawing 
 * in off-screen image buffers for performance improvement.
 */
public abstract class CachingCanvas extends LargeScrollableCanvas {

	private boolean doCaching = true;
	private ITileCache tileCache = new XYTileCache();
	private boolean debug = false;
	

	/**
	 * Constructor. 
	 */
	public CachingCanvas(Composite parent, int style) {
		super(parent, style);

		addPaintListener(new PaintListener() {
			public void paintControl(final PaintEvent e) {
				// show the busy mouse cursor during drawing, except if the user is resizing the
				// chart (resize cursor <-->) or is dragging the scrollbar. Solution: only show
				// busy cursor if display.getCursorControl() is the canvas, and the cursor
				// location is inside the *client area* of the canvas (ie not over a scrollbar).
				// Display.getCurrent().getCursorControl()==CachingCanvas.this -- not needed after all
				if (CachingCanvas.this.getClientArea().contains(Display.getCurrent().getCursorLocation())) {
					// show busy cursor during painting
					BusyIndicator.showWhile(null, new Runnable() {
						public void run() {
							paint(e.gc);
						}
					});
				}
				else {
					paint(e.gc);
				}
			}
		});
	}

	/**
	 * Returns whether caching is on.
	 */
	public boolean getCaching() {
		return doCaching;
	}

	/**
	 * Turns on/off caching.
	 */
	public void setCaching(boolean doCaching) {
		this.doCaching = doCaching;
		clearCanvasCache();
	}

	/**
	 * Paints the canvas, making use of the cache.
	 */
	protected void paint(GC gc) {
		// call any code subclass wants to run before painting
		beforePaint(gc);
		
		if (!doCaching) {
			// paint directly on the GC
			gc.setClipping(gc.getClipping().intersection(getViewportRectangle()));
			paintCachableLayer(gc);
			gc.setClipping(getClientArea());
			paintNoncachableLayer(gc);
		}
		else {
			Rectangle clip = gc.getClipping();
			Rectangle viewportRect = getViewportRectangle();
			clip = clip.intersection(viewportRect);
			gc.setClipping(clip);
			LargeRect lclip = canvasToVirtualRect(clip);
			
			ArrayList<Tile> cachedTiles = new ArrayList<Tile>();
			ArrayList<LargeRect> missingAreas = new ArrayList<LargeRect>();

			tileCache.getTiles(lclip, getVirtualWidth(), getVirtualHeight(), cachedTiles, missingAreas);
			//System.out.println("cache: found "+cachedTiles.size()+" tiles, missing "+missingAreas.size()+" areas");

			// display cached tiles
			for (Tile tile : cachedTiles) {
				gc.drawImage(tile.image, virtualToCanvasX(tile.rect.x), virtualToCanvasY(tile.rect.y));
				debugDrawTile(gc, tile.rect, new Color(null,0,255,0));
			}

			// draw missing tiles
			for (LargeRect lrect : missingAreas) {
				Rectangle rect = virtualToCanvasRect(lrect);
				Assert.isTrue(!rect.isEmpty()); // tile cache should not return empty rectangles

				Image image = new Image(getDisplay(), rect);
				GC imgc = new GC(image);
				Transform transform = new Transform(imgc.getDevice());
				transform.translate(-rect.x, -rect.y);
				imgc.setTransform(transform);
				imgc.setClipping(rect);

				paintCachableLayer(imgc);

				transform.dispose();
				imgc.dispose();

				// draw the image on the screen, and also add it to the cache
				gc.drawImage(image, rect.x, rect.y);
				tileCache.add(lrect, image);
				debugDrawTile(gc, lrect, new Color(null,255,0,0));
			}

			// paint items that we don't want to cache
			gc.setClipping(getClientArea());
			paintNoncachableLayer(gc);
		}
	}


	/**
	 * Marks the tiles on the screen by drawing a border for them.
	 */
	private void debugDrawTile(GC gc, LargeRect rect, Color color) {
		if (debug) {
			Rectangle viewportRect = getViewportRectangle();
			gc.setForeground(color);
			gc.setLineStyle(SWT.LINE_DOT);
			gc.drawRoundRectangle(
					(int)(rect.x - getViewportLeft() + viewportRect.x),
					(int)(rect.y - getViewportTop() + viewportRect.y),
					(int)rect.width-1, (int)rect.height-1, 30, 30);
		}
	}

	/**
	 * Called in every repaint request, before any call to paintCachables() and
	 * paintNoncachables(). This is a good place for pre-paint calculations
	 * whose result is needed by both paintCachables() and paintNoncachables().
	 */
	protected void beforePaint(GC gc) {}

	/**
	 * Paint everything in this method that can be cached. This may be called several
	 * times during a repaint, with different clip rectangles.
	 * 
	 * IMPORTANT: A transform (translate) is set on the gc, DO NOT OVERWRITE IT,
	 * or caching will be messed up. 
	 */
	protected abstract void paintCachableLayer(GC gc);

	/**
	 * Paint in this method anything that you don't want to be cached 
	 * (selection marks, etc). It will paint over the cachable layer.
	 */
	protected abstract void paintNoncachableLayer(GC gc);

	/**
	 * Clears the tile cache. To be called any time the drawing changes.
	 */
	public void clearCanvasCache() {
		tileCache.clear();
		System.out.println("canvas cache cleared");
	}
}
