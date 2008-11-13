package org.omnetpp.common.canvas;

import java.util.LinkedHashMap;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.Debug;

/**
 * A tile cache for CachingCanvas that works with tiles of fixed size (100x100 pixels).
 *
 * @author andras
 */
public class XYTileCache implements ITileCache {
	// we use narrow tall tiles to ensure smoother scrolling, as our canvases are typically drawn with a loop along the x axis
	private static final int TILE_WIDTH = 30;
	private static final int TILE_HEIGHT = 200;
	private static final int TILE_SIZE_BYTES = TILE_HEIGHT*TILE_WIDTH*4; // assumption: RGBA true color
	
	private static final boolean debug = false;
	
	private int memoryUsageLimit = 32*1024*1024; // 32Meg by default
	private int memoryUsage = 0;
	
	// use coords of tile's top-left corner as key into the hashmap; "linked" is used for LRU cache mgmt
	private LinkedHashMap<LargePoint,Tile> cache = new LinkedHashMap<LargePoint,Tile>();
	
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
		// find WHOLE tiles in the image, and extract them
		GC gc = new GC(image);
		long startX = modulo(rect.x,TILE_WIDTH)==0 ? rect.x : rect.x - modulo(rect.x,TILE_WIDTH) + TILE_WIDTH;
		long startY = modulo(rect.y,TILE_HEIGHT)==0 ? rect.y : rect.y - modulo(rect.y,TILE_HEIGHT) + TILE_HEIGHT;
		Assert.isTrue(startX>=rect.x);
		Assert.isTrue(startY>=rect.y);
		for (long x = startX; x+TILE_WIDTH<=rect.right(); x+=TILE_WIDTH) {
			for (long y = startY; y+TILE_HEIGHT<=rect.bottom(); y+=TILE_HEIGHT) {
				LargePoint key = new LargePoint(x,y);
				Assert.isTrue(!cache.containsKey(key)); // because CachingCanvas only calls us with tiles we reported missing
				if (!cache.containsKey(key)) {
					if (debug) {
						gc.drawLine((int)(x - rect.x), (int)(y - rect.y), (int)(x - rect.x)+10, (int)(y - rect.y)+10);
						gc.drawText("tile ("+x+","+y+")", (int)(x - rect.x)+10, (int)(y - rect.y)+5);
					}
					Image tileImage = new Image(null, TILE_WIDTH, TILE_HEIGHT);
					gc.copyArea(tileImage, (int)(x - rect.x), (int)(y - rect.y));
					Tile tile = new Tile(new LargeRect(x, y, TILE_WIDTH, TILE_HEIGHT), tileImage);
					cache.put(key, tile);
					memoryUsage += TILE_SIZE_BYTES;
					if (memoryUsage > memoryUsageLimit)
						break;
				} 
			}
			if (memoryUsage > memoryUsageLimit)
				break;
		}
		gc.dispose();
		image.dispose();
		discardOldTiles();
	}

	public void printCache() {
		Debug.printf("Cache: %d tiles, memory usage %dk, limit %dk\n", cache.size(), memoryUsage/1024, memoryUsageLimit/1024);
		for (LargePoint p : cache.keySet()) {
			Debug.printf(" - tile (%d,%d) at (%d,%d), size %dk\n", p.x/TILE_WIDTH, p.y/TILE_HEIGHT, p.x, p.y, TILE_SIZE_BYTES/1024);
		}
	}

	private void discardOldTiles() {
		//printCache();
		int count = 0;
		while (memoryUsage > memoryUsageLimit) {
			Assert.isTrue(cache.size()>0);
			LargePoint key = cache.keySet().iterator().next(); // get first element's key
			Tile tile = cache.remove(key);
			tile.image.dispose();
			memoryUsage -= TILE_SIZE_BYTES;
			count++;
		}
		if (debug && count>0)
			Debug.printf("cache: thrown out %d tiles, currently holding %d\n", count, cache.size());
	}

	public void clear() {
		for (Tile tile : cache.values())
			tile.image.dispose();
		cache.clear();
		memoryUsage = 0;
	}

	public void getTiles(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		// Return tiles that overlap with "rect"; missing areas are rounded up to form
		// whole tiles for better caching, and possibly also merged (so that fewer paint() calls
		// are needed)
		long startX = rect.x - modulo(rect.x,TILE_WIDTH);
		long startY = rect.y - modulo(rect.y,TILE_HEIGHT);
		LargePoint lookupKey = new LargePoint();
		for (long x = startX; x<rect.right(); x+=TILE_WIDTH) {
			for (long y = startY; y<rect.bottom(); y+=TILE_HEIGHT) {
				lookupKey.set(x,y);
				if (cache.containsKey(lookupKey)) {
					Assert.isTrue(cache.get(lookupKey).rect.x==lookupKey.x && cache.get(lookupKey).rect.y==lookupKey.y);
					outCachedTiles.add(cache.get(lookupKey));
				}
				else {
					mergeOrAdd(outMissingAreas, new LargeRect(x, y, TILE_WIDTH, TILE_HEIGHT));
				}
			}
		}
		
		if (debug) {
			Debug.printf("For rect %s, returning %d tiles", rect.toString(), outCachedTiles.size());
			if (outMissingAreas.size()>0) {
				Debug.printf(" and %d missing areas:\n", outMissingAreas.size());
				for (LargeRect r : outMissingAreas) {
					Debug.println(" - "+r);
				}
			}
			Debug.println();
		}
	}

	/**
	 * Merges the given rectangle to the rectangles in areas[], or adds it if cannot be merged.
	 */
	private void mergeOrAdd(List<LargeRect> areas, LargeRect r) {
		// search for a rectangle to which r could be merged. The way XYTileCache works,
		// we have better chance starting with the most recently added rectangles, 
		// i.e. iterate backwards
		for (int i=areas.size()-1; i>=0; i--) {
			LargeRect area = areas.get(i);
			if (area.y == r.y && area.height == r.height && area.right()==r.x) {
				area.width += r.width; // merge horizontally (on the right)
				mergeOrAdd(areas, areas.remove(i)); // try our luck recursively with the new rectangle as well
				return;
			}
			if (area.x == r.x && area.width == r.width && area.bottom()==r.y) {
				area.height += r.height; // merge vertically (below) 
				mergeOrAdd(areas, areas.remove(i)); // try our luck recursively with the new rectangle as well
				return;
			}
		}
		// could not be merged: just add
		areas.add(r);
	}
	
	private long modulo(long a, long b) {
		return a < 0 ? a % b + b : a % b;
	}
}
