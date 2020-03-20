/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.apache.batik.svggen.SVGGraphics2D;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.Path;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gmf.runtime.draw2d.ui.render.awt.internal.svg.export.GraphicsSVG;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ITileCache.Tile;
import org.omnetpp.common.image.ImageUtils;
import org.omnetpp.common.util.GraphicsUtils;

/**
 * A scrollable canvas that supports caching of (part of) the drawing
 * in off-screen image buffers for performance improvement.
 *
 * @author andras
 */
@SuppressWarnings("restriction")
public abstract class CachingCanvas extends LargeScrollableCanvas {

    private boolean doCaching = true;
    private ITileCache tileCache = new XYTileCache();
    private boolean debug = Debug.isChannelEnabled("cachingcanvas");

    /**
     * Constructor.
     */
    public CachingCanvas(Composite parent, int style) {
        super(parent, style);

        addPaintListener(new PaintListener() {
            public void paintControl(final PaintEvent e) {
                if (debug)
                    Debug.println("actually painting chart");
                paint(e.gc);
                if (debug)
                    Debug.println("actual painting done");
            }
        });
    }

    /**
     * Override base method to limit the size of the virtual canvas.
     * This is necessary because of arithmetic overflows could
     * happen while generating the tiles.
     */
    @Override
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
    public void copyImageToClipboard() {
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
        cp.setContents(new ImageTransferable(ImageUtils.convertToAWT(image)), owner);
    }

    public void saveImage(String fileName) {
        String ext = new Path(fileName).getFileExtension().toLowerCase();
        int format = -1;
        switch(ext) {
            case "svg": format = SWT.IMAGE_SVG; break;
            case "png": format = SWT.IMAGE_PNG; break;
            case "jpg": format = SWT.IMAGE_JPEG; break;
            case "bmp": format = SWT.IMAGE_BMP; break;
            case "ico": format = SWT.IMAGE_ICO; break;
            // Note: GIF is supposed to work too, but currently creates an empty file (Eclipse 4.14)
            default: throw new RuntimeException("Unsupported file format '" + ext + "' (supported ones are: svg, png, jpg, bmp, ico)");
        }

        if (format == SWT.IMAGE_SVG)
            saveSVG(fileName);
        else {
            int width = getClientArea().width, height = getClientArea().height;
            Image image = getImage(width, height);
            ImageLoader loader = new ImageLoader();
            loader.data = new ImageData[] { image.getImageData() };
            loader.save(fileName, format);
            image.dispose();
        }
    }

    protected void saveSVG(String fileName) {
        int width = getClientArea().width, height = getClientArea().height;
        GraphicsSVG graphics = GraphicsSVG.getInstance(new Rectangle(0, -1, width, height));
        try {
            SVGGraphics2D g = graphics.getSVGGraphics2D();
            g.setClip(0, 0, width, height);
            paintWithoutCaching(graphics);
            writeXML(graphics, fileName);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        finally {
            graphics.dispose();
        }
    }

    private void writeXML(GraphicsSVG graphics, String fileName)
        throws TransformerConfigurationException, TransformerFactoryConfigurationError, TransformerException
    {
        Source source = new DOMSource(graphics.getRoot());
        StreamResult streamResult = new StreamResult(new File(fileName));
        Transformer transformer = TransformerFactory.newInstance().newTransformer();
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");
        transformer.transform(source, streamResult);
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

    protected Graphics createGraphics(GC gc) {
        return new SWTGraphics(gc);
    }

    protected void paint(GC gc) {
        Graphics graphics = createGraphics(gc);
        try {
            paint(graphics);
        }
        finally {
            graphics.dispose();
        }
    }

    /**
     * Paints the canvas, making use of the cache.
     */
    protected void paint(Graphics graphics) {
        if (!doCaching)
            paintWithoutCaching(graphics);
        else {
            Rectangle viewportRect = new Rectangle(getViewportRectangle());
            Rectangle oldClip = GraphicsUtils.getClip(graphics);
            Rectangle clip = oldClip.getIntersection(viewportRect);
            graphics.setClip(clip);
            LargeRect lclip = canvasToVirtualRect(clip);

            ArrayList<Tile> cachedTiles = new ArrayList<Tile>();
            ArrayList<LargeRect> missingAreas = new ArrayList<LargeRect>();

            tileCache.getTiles(lclip, getVirtualWidth(), getVirtualHeight(), cachedTiles, missingAreas);
            //Debug.println("cache: found "+cachedTiles.size()+" tiles, missing "+missingAreas.size()+" areas");

            // display cached tiles
            for (Tile tile : cachedTiles) {
                graphics.drawImage(tile.image, virtualToCanvasX(tile.rect.x), virtualToCanvasY(tile.rect.y));
                debugDrawTile(graphics, tile.rect, new Color(null,0,255,0));
            }

            // draw missing tiles
            for (LargeRect lrect : missingAreas) {
                Rectangle rect = virtualToCanvasRect(lrect);
                Assert.isTrue(!rect.isEmpty()); // tile cache should not return empty rectangles

                Graphics imageGraphics = null;
                GC imageGC = null;
                Transform transform = null;
                Image image = new Image(getDisplay(), rect.width, rect.height);
                try {
                    imageGC = new GC(image);
                    imageGraphics = createGraphics(imageGC);
                    imageGraphics.translate(-rect.x, -rect.y);
                    imageGraphics.setClip(new Rectangle(rect.x, rect.y, rect.width, rect.height));
                    paintCachableLayer(imageGraphics);
                }
                finally {
                    if (imageGraphics != null)
                        imageGraphics.dispose();
                    if (transform != null)
                        transform.dispose();
                    if (imageGC != null)
                        imageGC.dispose();
                }

                // draw the image on the screen, and also add it to the cache
                graphics.drawImage(image, rect.x, rect.y);
                tileCache.add(lrect, image);
                debugDrawTile(graphics, lrect, new Color(null,255,0,0));
            }

            // paint items that we don't want to cache
            graphics.setClip(oldClip);
            paintNoncachableLayer(graphics);
        }
    }

    /**
     * Paint directly on the graphics
     */
    private void paintWithoutCaching(Graphics graphics) {
        Rectangle clip = GraphicsUtils.getClip(graphics);
        graphics.setClip(clip.intersect(new Rectangle(getViewportRectangle())));
        paintCachableLayer(graphics);
        graphics.setClip(new Rectangle(getClientArea()));
        paintNoncachableLayer(graphics);
    }

    /**
     * Marks the tiles on the screen by drawing a border for them.
     */
    private void debugDrawTile(Graphics graphics, LargeRect rect, Color color) {
        if (debug) {
            Rectangle viewportRect = new Rectangle(getViewportRectangle());
            graphics.setForegroundColor(color);
            graphics.setLineStyle(SWT.LINE_DOT);
            Rectangle r = new Rectangle(
                    (int)(rect.x - getViewportLeft() + viewportRect.x),
                    (int)(rect.y - getViewportTop() + viewportRect.y),
                    (int)rect.width-1, (int)rect.height-1);
            graphics.drawRoundRectangle(r, 30, 30);
        }
    }

    /**
     * Paint everything in this method that can be cached. This may be called several
     * times during a repaint, with different clip rectangles.
     *
     * IMPORTANT: A transform (translate) is set on the graphics, DO NOT OVERWRITE IT,
     * or caching will be messed up.
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
        if (debug)
            Debug.println("canvas cache cleared");
    }
}
