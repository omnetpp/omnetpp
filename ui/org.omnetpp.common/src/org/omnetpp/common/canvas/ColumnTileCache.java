package org.omnetpp.common.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.Debug;

/**
 * A tile cache that works with tiles that cover the full vertical range
 * of the "virtual canvas area". 
 *
 * Currently not used. We use XYTileCache instead, because ColumnTileCache
 * fails if canvas virtual height becomes too big (e.g as a result of 
 * zooming), as it runs out of memory to store a full column.
 *  
 * @author andras
 */
public class ColumnTileCache implements ITileCache {

	private static final boolean debug = false;
	private int memoryUsageLimit = 32*1024*1024; // 32Meg by default
	private int memoryUsage = 0;
	private ArrayList<Tile> cache = new ArrayList<Tile>();
	
	public int getMemoryUsage() {
		return memoryUsage;
	}

	public int getMemoryUsageLimit() {
		return memoryUsageLimit;
	}

	public void setMemoryUsageLimit(int limit) {
		this.memoryUsageLimit = limit;
		discardOldTiles();
	}

	public void add(LargeRect rect, Image image) {
		Tile tile = new Tile(rect, image);
		cache.add(tile);
		int imageSize = imageSize(image);
		Assert.isTrue(imageSize < memoryUsageLimit, "memory usage limit set too small, even a single column image won't fit");
		memoryUsage += imageSize;
		//Debug.printf("cache: added image %d x %d, size %dk\n", rect.width, rect.height, imageSize/1024);
		discardOldTiles();
	}

	public void printCache() {
		Debug.printf("Cache: %d tiles, memory usage %dk, limit %dk\n", cache.size(), memoryUsage/1024, memoryUsageLimit/1024);
		for (Tile tile : cache) {
			Debug.printf(" - tile %d x %d, size %dk\n", tile.rect.width, tile.rect.height, imageSize(tile.image)/1024);
		}
	}

	private void discardOldTiles() {
		//printCache();
		int count = 0;
		while (memoryUsage > memoryUsageLimit) {
			Assert.isTrue(cache.size()>0);
			Tile tile = cache.remove(0);
			memoryUsage -= imageSize(tile.image);
			tile.image.dispose();
			count++;
		}
		if (debug && count>0)
			Debug.printf("cache: thrown out %d tiles, currently holding %d\n", count, cache.size());
	}

	private int imageSize(Image image) {
		return image.getBounds().width * image.getBounds().height * 4; //XXX refine
	}

	public void clear() {
		for (Tile tile : cache)
			tile.image.dispose();
		cache.clear();
		memoryUsage = 0;
	}

	public void getTiles(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		// simple algorithm: find best match, and return diff to it as missing
		Tile bestTile = findBestMatch(rect);
		if (bestTile==null) {
			// no overlapping tile, request full rectangle
			outMissingAreas.add(tweakRect(new LargeRect(rect.x, 0, rect.width, virtualHeight)));
		}
		else if (bestTile.rect.contains(rect)) {
			// tile covers it all
			outCachedTiles.add(bestTile);
		}
		else {
			// calculate diff rect and see
			LargeRect diff = rect.minus(bestTile.rect); //TODO impl and use advancedMinus
			if (diff==null) {
				// best tile is no use (actually this cannot happen in ColumnTileCache as all tiles are full height)
				outMissingAreas.add(tweakRect(new LargeRect(rect.x, 0, rect.width, virtualHeight)));
			}
			else {
				outCachedTiles.add(bestTile);
				// instead of adding the diff to missingAreas right away, try to find more tiles by recursion
				//outMissingAreas.add(tweakRect(new LargeRect(diff.x, 0, diff.width, virtualHeight)));
				getTiles(diff, virtualWidth, virtualHeight, outCachedTiles, outMissingAreas);
			}
		}
	}

	private LargeRect tweakRect(LargeRect rect) {
		// make left & right edge a multiple of 8 pixels
		long dx = rect.x & 7L;
		rect.x -= dx;
		rect.width = (rect.width + dx + 7) & ~7L;
		return rect;
	}

	/**
	 * Returns the tile which has the largest overlapping area with rect,
	 * and can be substracted from rect (i.e. diff set is rectangular).
	 * Returns null if there is no such tile.
	 */
	private Tile findBestMatch(LargeRect rect) {
		Tile bestTile = null;
		long bestArea = 0;
		for (Tile tile : cache) {
			if (tile.rect.intersects(rect) && rect.minus(tile.rect)!=null) {
				LargeRect r = tile.rect.intersection(rect);
				long area = r.width * r.height;
				if (area > bestArea) {
					bestArea = area;
					bestTile = tile;
				}
			}
		}
		return bestTile;
	}
}
