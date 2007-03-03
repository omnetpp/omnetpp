package org.omnetpp.common.canvas;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;

/**
 * A tile cache for CachingCanvas that works with tiles of fixed size (100x100 pixels).
 *
 * @author andras
 */
public class XYTileCache implements ITileCache {
	private static final int TILE_WIDTH = 100;
	private static final int TILE_HEIGHT = 100;
	private static final int TILESIZE_BYTES = TILE_HEIGHT*TILE_WIDTH*4; //XXX assumption - refine
	
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
		// extract tiles (whole tiles only!) from the image
		
		GC gc = new GC(image);
		long startX = rect.x%TILE_WIDTH==0 ? rect.x : rect.x - rect.x%TILE_WIDTH + TILE_WIDTH;
		long startY = rect.y%TILE_HEIGHT==0 ? rect.y : rect.y - rect.y%TILE_HEIGHT + TILE_HEIGHT;
		for (long x = startX; x+TILE_WIDTH<=rect.right(); x+=TILE_WIDTH) {
			for (long y = startY; y+TILE_HEIGHT<=rect.bottom(); y+=TILE_HEIGHT) {
				LargePoint key = new LargePoint(x,y);
				if (cache.containsKey(key))
					System.out.println("INCONSISTENCY in add()! cache already contains cache "+x+","+y);
				Image tileImage = new Image(null, TILE_WIDTH, TILE_HEIGHT);
				gc.copyArea(tileImage, (int)(x - rect.x), (int)(y - rect.y));
				Tile tile = new Tile(new LargeRect(x, y, TILE_WIDTH, TILE_HEIGHT), tileImage);
				cache.put(key, tile);
				memoryUsage += TILESIZE_BYTES;
				if (memoryUsage > memoryUsageLimit)
					break;
			}
			if (memoryUsage > memoryUsageLimit)
				break;
		}
		gc.dispose();
		discardOldTiles();
	}

	public void printCache() {
		System.out.printf("Cache: %d tiles, memory usage %dk, limit %dk\n", cache.size(), memoryUsage/1024, memoryUsageLimit/1024);
		for (LargePoint p : cache.keySet()) {
			System.out.printf(" - tile (%d,%d) at (%d,%d), size %dk\n", p.x/TILE_WIDTH, p.y/TILE_HEIGHT, p.x, p.y, TILESIZE_BYTES/1024);
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
			memoryUsage -= TILESIZE_BYTES;
			count++;
		}
		if (count>0)
			System.out.printf("cache: thrown out %d tiles, currently holding %d\n", count, cache.size());
	}

	public void clear() {
		for (Tile tile : cache.values())
			tile.image.dispose();
		cache.clear();
		memoryUsage = 0;
	}

	public void getTiles(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		long startX = rect.x%TILE_WIDTH==0 ? rect.x : rect.x - rect.x%TILE_WIDTH + TILE_WIDTH;
		long startY = rect.y%TILE_HEIGHT==0 ? rect.y : rect.y - rect.y%TILE_HEIGHT + TILE_HEIGHT;
		LargePoint key = new LargePoint();
		for (long x = startX; x+TILE_WIDTH<=rect.right(); x+=TILE_WIDTH) {
			LargeRect missingColumn = null; // will try to merge missing tiles vertically
			for (long y = startY; y+TILE_HEIGHT<=rect.bottom(); y+=TILE_HEIGHT) {
				key.x = x;
				key.y = y;
				if (cache.containsKey(key)) {
					outCachedTiles.add(cache.get(key));
					if (missingColumn!=null) {
						outMissingAreas.add(missingColumn);
						missingColumn = null;
					}
				}
				else {
					if (missingColumn==null) {
						missingColumn = new LargeRect(x, y, TILE_WIDTH, TILE_HEIGHT);
					}
					else {
						missingColumn.height += TILE_HEIGHT;
					}
				}
			}
			if (missingColumn!=null) {
				LargeRect lastMissingColumn = outMissingAreas.size()>0 ? outMissingAreas.get(outMissingAreas.size()-1) : null;
				if (lastMissingColumn!=null && lastMissingColumn.y == missingColumn.y && lastMissingColumn.height == missingColumn.height && lastMissingColumn.right()==missingColumn.x) {
					// merge
					lastMissingColumn.width += missingColumn.width;
					missingColumn = null;
				}
				else {
					// cannot be merged, add
					outMissingAreas.add(missingColumn);
					missingColumn = null;
				}
			}
		}
		
		// add border
		//XXX
		
		System.out.println("***** GETTILES *****");
		printCache();
		System.out.printf("For rect %s, returning %d tiles and %d missing rectangles\n\n", rect.toString(), outCachedTiles.size(), outMissingAreas.size());
	}
}
