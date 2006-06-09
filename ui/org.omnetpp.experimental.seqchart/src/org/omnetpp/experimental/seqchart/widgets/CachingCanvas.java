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
//XXX Other utility functionality: dragging the area with the mouse ("hand cursor"); rubberbanding.
public abstract class CachingCanvas extends LargeScrollableCanvas {

	private boolean doCaching = false;
	ITileCache tileCache = new ColumnTileCache(); //XXX make settable

	public CachingCanvas(Composite parent, int style) {
		//super(parent, style | SWT.NO_BACKGROUND);
		//super(parent, style);
		super(parent, style | SWT.DOUBLE_BUFFERED);
	}

	public boolean getCaching() {
		return doCaching;
	}

	public void setCaching(boolean doCaching) {
		this.doCaching = doCaching;
	}

	@Override
	protected void paint(GC gc) {
		if (!doCaching) {
			// paint directly on the GC
			Graphics graphics = new SWTGraphics(gc);
			paintCachables(graphics);
			paintNoncachables(graphics);
			graphics.dispose();
		}
		else {
			Rectangle clip = gc.getClipping();
			LargeRect lclip = new LargeRect(clip.x+getViewportLeft(), clip.y+getViewportTop(), clip.width, clip.height);
			
			ArrayList<Tile> cachedTiles = new ArrayList<Tile>();
			ArrayList<LargeRect> missingAreas = new ArrayList<LargeRect>();

			tileCache.getTiles(lclip, getVirtualWidth(), getVirtualHeight(), cachedTiles, missingAreas);

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

				paintCachables(imgraphics);

				imgraphics.dispose();
				imgc.dispose();
				
				// draw the image on the screen, and also add it to the cache
				gc.drawImage(image, rect.x, rect.y);
				tileCache.add(lrect, image);
				debugDrawTile(gc, lrect, new Color(null,255,0,0));
			}

			// paint items that we don't want to cache
			Graphics graphics = new SWTGraphics(gc);
			paintNoncachables(graphics);
			graphics.dispose();
		}
		
	}

	private void debugDrawTile(GC gc, LargeRect rect, Color color) {
		gc.setForeground(color);
		gc.drawRoundRectangle(
				(int)(rect.x-getViewportLeft()), (int)(rect.y-getViewportTop()),
				(int)rect.width-1, (int)rect.height-1, 8, 8);
	}

	/**
	 * By redefining this method and modifying the passed paintRect in it, 
	 * subclasses can specify what rectangle they will actually redraw when 
	 * they get the given rectangle as clipping. This gives CachingCanvas
	 * a possibility to cache a larger area than the clipping if it's going to be 
	 * drawn anyway.
	 */
	protected void refinePaintRectangle(Rectangle paintRect) { }
	protected abstract void paintCachables(Graphics graphics);
	protected abstract void paintNoncachables(Graphics graphics);

	public void clearCanvasCache() {
		tileCache.clear();
	}
}
