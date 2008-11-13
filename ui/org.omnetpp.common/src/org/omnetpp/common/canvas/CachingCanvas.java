package org.omnetpp.common.canvas;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ITileCache.Tile;
import org.omnetpp.common.image.ImageConverter;

/**
 * A scrollable canvas that supports caching of (part of) the drawing 
 * in off-screen image buffers for performance improvement.
 * 
 * @author andras
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
				paint(e.gc);
			}
		});
	}
	
	/**
	 * Override base method to limit the size of the virtual canvas.
	 * This is necessary because of arithmetic overflows could
	 * happen while generating the tiles.
	 */
	public void setVirtualSize(long width, long height) {
		width = Math.min(width, Long.MAX_VALUE - 10000);
		height = Math.min(height, Long.MAX_VALUE - 10000);
		super.setVirtualSize(width, height);
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
     * Copies the image of the chart to the clipboard.
     * Uses AWT functionality, because SWT does not support ImageTransfer yet.
     * See: https://bugs.eclipse.org/bugs/show_bug.cgi?id=78856.
     */
    public void copyToClipboard() {
        Clipboard cp = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
        ClipboardOwner owner = new java.awt.datatransfer.ClipboardOwner() {
            public void lostOwnership(Clipboard clipboard, Transferable contents) {
            }
        };
        
        class ImageTransferable implements Transferable {
            public java.awt.Image image;

            public ImageTransferable(java.awt.Image image) {
                this.image = image;
            }
            
            public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
                if (flavor == DataFlavor.imageFlavor)
                    return image;
                else
                    throw new UnsupportedFlavorException(flavor);
            }

            public DataFlavor[] getTransferDataFlavors() {
                return new DataFlavor[] {DataFlavor.imageFlavor};
            }

            public boolean isDataFlavorSupported(DataFlavor flavor) {
                return flavor == DataFlavor.imageFlavor;
            }
        };
        
        int width = getClientArea().width, height = getClientArea().height;
        Image image = getImage(width, height);
        cp.setContents(new ImageTransferable(ImageConverter.convertToAWT(image)), owner);
    }
    
    /**
     * Returns the image of the chart.
     */
    public Image getImage(int width, int height) {
        Image image = new Image(getDisplay(), width, height);
        GC gc = new GC(image);
        paint(gc);
        gc.dispose();
        return image;
    }

	/**
	 * Paints the canvas, making use of the cache.
	 */
	protected void paint(GC gc) {
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
			//Debug.println("cache: found "+cachedTiles.size()+" tiles, missing "+missingAreas.size()+" areas");

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
				// KLUDGE: FIXME: XXX: See Eclipse bug database: 245523
				// remove the next 3 lines when that bug is fixed, because it is an incorrect workaround
				// putting -1.01f does not help either, because the closer you get to -1.0 is the worse
				if (rect.y == 1)
                    transform.translate(-rect.x, -2f);
				else
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
		if (debug)
			Debug.println("canvas cache cleared");
	}
}
