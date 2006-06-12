package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.experimental.seqchart.widgets.ITileCache.Tile;

/**
 * A scrollable canvas that supports caching of (part of) the drawing 
 * in off-screen image buffers for performance improvement.
 */
//TODO redraw chart with antialias while user is idle? hints: new SafeRunnable(); or:
//getDisplay().asyncExec(new Runnable() {
//	public void run() { ... }
//};
public abstract class CachingCanvas extends LargeScrollableCanvas {

	private boolean doCaching = true;
	private ITileCache tileCache = new ColumnTileCache();
	private boolean debug = true;
	

	/**
	 * Constructor. 
	 */
	public CachingCanvas(Composite parent, int style) {
		super(parent, style);
	}

	/**
	 * Returns whether caching is on or off.
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
	@Override
	protected void paint(GC gc) {
		// call any code subclass wants to run before painting
		beforePaint();
		
		if (!doCaching) {
			// paint directly on the GC
			Graphics graphics = new SWTGraphics(gc);
			paintCachableLayer(graphics);
			paintNoncachableLayer(graphics);
			graphics.dispose();
		}
		else {
			Rectangle clip = gc.getClipping();
			LargeRect lclip = new LargeRect(clip.x+getViewportLeft(), clip.y+getViewportTop(), 
											Math.min(clip.width, getVirtualWidth()), 
											Math.min(clip.height, getVirtualHeight()));
			
			ArrayList<Tile> cachedTiles = new ArrayList<Tile>();
			ArrayList<LargeRect> missingAreas = new ArrayList<LargeRect>();

			tileCache.getTiles(lclip, getVirtualWidth(), getVirtualHeight(), cachedTiles, missingAreas);
			//System.out.println("cache: found "+cachedTiles.size()+" tiles, missing "+missingAreas.size()+" areas");

			// display cached tiles
			for (Tile tile : cachedTiles) {
				gc.drawImage(tile.image, (int)(tile.rect.x-getViewportLeft()), (int)(tile.rect.y-getViewportTop()));
				debugDrawTile(gc, tile.rect, new Color(null,0,255,0));
			}

			// draw missing tiles
			for (LargeRect lrect : missingAreas) {
				Rectangle rect = new Rectangle((int)(lrect.x-getViewportLeft()), (int)(lrect.y-getViewportTop()), (int)lrect.width, (int)lrect.height);
				Image image = new Image(getDisplay(), rect);
				GC imgc = new GC(image);
				Graphics imgraphics = new SWTGraphics(imgc);  // we need to use Graphics because GC doesn't have translate() support
				imgraphics.translate(-rect.x, -rect.y);
				imgraphics.setClip(new org.eclipse.draw2d.geometry.Rectangle(rect));

				paintCachableLayer(imgraphics);

				imgraphics.dispose();
				imgc.dispose();
				
				// draw the image on the screen, and also add it to the cache
				gc.drawImage(image, rect.x, rect.y);
				tileCache.add(lrect, image);
				debugDrawTile(gc, lrect, new Color(null,255,0,0));
			}

			// paint items that we don't want to cache
			gc.setClipping(getClientArea());
			Graphics graphics = new SWTGraphics(gc);
			paintNoncachableLayer(graphics);
			graphics.dispose();
		}
		
	}


	/**
	 * Marks the tiles on the screen by drawing a border for them.
	 */
	private void debugDrawTile(GC gc, LargeRect rect, Color color) {
		if (debug) {
			gc.setForeground(color);
			gc.setLineStyle(SWT.LINE_DOT);
			gc.drawRoundRectangle(
					(int)(rect.x-getViewportLeft()), (int)(rect.y-getViewportTop()),
					(int)rect.width-1, (int)rect.height-1, 30, 30);
		}
	}

	/**
	 * Called in every repaint request, before any call to paintCachables() and
	 * paintNoncachables(). This is a good place for pre-paint calculations
	 * whose result is needed by both paintCachables() and paintNoncachables().
	 */
	protected abstract void beforePaint();

	/**
	 * Paint everything in this method that can be cached. This may be called several
	 * times during a repaint, with different clip rectangles.
	 */
	protected abstract void paintCachableLayer(Graphics graphics);

	/**
	 * Paint in this method anything that you don't want to be cached 
	 * (selection marks, etc). It will paint over the cachable layer.
	 */
	protected abstract void paintNoncachableLayer(Graphics graphics);

	/**
	 * Clears the tile cache. To be called any time the drawing changes.
	 */
	public void clearCanvasCache() {
		tileCache.clear();
		System.out.println("Cache cleared!");
	}
}
