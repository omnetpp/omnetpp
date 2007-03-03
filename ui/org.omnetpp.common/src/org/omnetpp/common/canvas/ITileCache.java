package org.omnetpp.common.canvas;

import java.util.List;

import org.eclipse.swt.graphics.Image;

/**
 * Interface for tile cache classes for CachingCanvas.
 * 
 * @author andras
 */
public interface ITileCache {
	
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

	public void add(LargeRect rect, Image image);

	public void clear();
	
}
