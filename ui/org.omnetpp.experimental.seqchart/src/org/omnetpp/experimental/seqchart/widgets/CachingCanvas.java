package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;

/**
 * A scrollable canvas that supports caching of (part of) the drawing 
 * in off-screen image buffers for performance improvement.
 */
//XXX Other utility functionality: dragging the area with the mouse ("hand cursor"); rubberbanding.
public abstract class CachingCanvas extends LargeScrollableCanvas {

	private static class CacheItem {
		public Rectangle rect = null; //XXX Bad, bad, bad!!! store virtual (long) coords!!!
		public Image image = null;
		public CacheItem() {}
		public CacheItem(Rectangle rect, Image image) {
			this.rect = rect; 
			this.image = image;
		}
	}

	private boolean doCaching = true;
	private long memoryUsageLimit = 32*1024*1024; // 32Meg by default
	private long memoryUsage = 0;
	private ArrayList<CacheItem> cache = new ArrayList<CacheItem>();
	
	public CachingCanvas(Composite parent, int style) {
		super(parent, style | SWT.NO_BACKGROUND);
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
			// perform caching
			Rectangle r = gc.getClipping();
			
			CacheItem cacheItem = findImageFor(r);  //XXX use partial matches as well!!!
			if (cacheItem==null) {
			//if (true) {
				r = new Rectangle(r.x, r.y, r.width, r.height);
				refinePaintRectangle(r);
				
				Image image = new Image(getDisplay(), r);
				GC imgc = new GC(image);
				Graphics imgraphics = new SWTGraphics(imgc);  // we need to use Graphics because GC doesn't have translate() support
				imgraphics.translate(-r.x, -r.y);
				imgraphics.setClip(new org.eclipse.draw2d.geometry.Rectangle(r));

				paintCachables(imgraphics);

				imgraphics.dispose();
				imgc.dispose();
				
				cacheItem = addToCache(r, image);
			}

			// copy cached image to screen
			gc.drawImage(cacheItem.image, cacheItem.rect.x, cacheItem.rect.y);

			// paint items that we don't want to appear on the cached image
			Graphics graphics = new SWTGraphics(gc);
			paintNoncachables(graphics);
			graphics.dispose();
		}
		
	}

	/**
	 * By redefining this method and modifying the passed paintRect in it, 
	 * subclasses can specify what rectangle they will actually redraw when 
	 * they get the given rectangle as clipping. This gives CachingCanvas
	 * a possibility to cache a larger area than the clipping if it's going to be 
	 * drawn anyway.
	 */
	protected void refinePaintRectangle(Rectangle paintRect) {
	}
	protected abstract void paintCachables(Graphics graphics);
	protected abstract void paintNoncachables(Graphics graphics);

	private CacheItem findImageFor(Rectangle r) {
		for (CacheItem item : cache)
			if (item.rect.contains(r.x, r.y) && item.rect.contains(r.x+r.width-1, r.y+r.height-1))
				return item;
		return null;
	}

	private CacheItem addToCache(Rectangle r, Image image) {
		CacheItem cacheItem;
		cacheItem = new CacheItem(new Rectangle(r.x, r.y, r.width, r.height), image);
		cache.add(cacheItem);
		memoryUsage += image.getImageData().data.length;
		if (memoryUsage > memoryUsageLimit) {
			//TODO throw out something
		}
		System.out.printf("image %d x %d added to cache, memory usage %dk\n", r.width, r.height, memoryUsage/1024);
		return cacheItem;
	}

	public void flushCanvasCache() {
		cache.clear();
		memoryUsage = 0;
	}
}
