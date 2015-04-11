/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;

import java.util.List;

import org.eclipse.swt.graphics.Image;

/**
 * Interface for tile cache classes for CachingCanvas.
 *
 * @author andras
 */
public interface ITileCache {

    /**
     * Represents a tile. Tiles should only be manipulated by the tile cache
     * and not by any other class.
     */
    public static class Tile {
        public LargeRect rect = null;
        public Image image = null;

        public Tile(LargeRect rect, Image image) {
            this.rect = new LargeRect(rect);
            this.image = image;
        }
    }

    public void setMemoryUsageLimit(int limit);

    public int getMemoryUsageLimit();

    public int getMemoryUsage();

    /**
     * Receives the rectangle to cover on screen and the dimensions
     * of the full "virtual" canvas, and returns a list of tiles
     * that need to be copied to the screen, and a list of the
     * areas still missing from the cache to cover the rectangle.
     *
     * @param rect area to cover
     * @param virtualWidth full canvas width
     * @param virtualHeight full canvas height
     * @param outCachedTiles gets filled with existing tiles that apply
     * @param outMissingAreas gets filled with missing areas
     */
    public void getTiles(
            LargeRect rect,
            long virtualWidth, long virtualHeight,
            List<Tile> outCachedTiles,
            List<LargeRect> outMissingAreas);

    /**
     * Puts the given image (or parts of it) into the cache.
     *
     * @param rect   virtual coordinates of the image (top left)
     * @param image  should be either stored or disposed of by this method
     */
    public void add(LargeRect rect, Image image);

    /**
     * Clears the cache.
     */
    public void clear();

}
