package org.omnetpp.experimental.seqchart.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.graphics.Image;

/**
 * A tile cache that works with tiles that cover the full vertical range
 * of the "virtual canvas area". 
 *
 * @author andras
 */
public class ColumnTileCache implements ITileCache {

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

	public Tile add(LargeRect rect, Image image) {
		Tile tile = new Tile(rect, image);
		cache.add(tile);
		int imageSize = imageSize(image);
		Assert.isTrue(imageSize < memoryUsageLimit, "memory usage limit set too small");
		memoryUsage += imageSize;
		System.out.printf("cache: added image %d x %d, size %dk\n", rect.width, rect.height, imageSize/1024);
		discardOldTiles();
		return tile;
	}

	public void printCache() {
		System.out.printf("Cache: %d tiles, memory usage %dk, limit %dk\n", cache.size(), memoryUsage/1024, memoryUsageLimit/1024);
		for (Tile tile : cache) {
			System.out.printf(" - tile %d x %d, size %dk\n", tile.rect.width, tile.rect.height, imageSize(tile.image)/1024);
		}
	}

	private void discardOldTiles() {
		printCache();
		int count = 0;
		while (memoryUsage > memoryUsageLimit) {
			Assert.isTrue(cache.size()>0);
			Tile tile = cache.remove(0);
			memoryUsage -= imageSize(tile.image);
			tile.image.dispose();
			count++;
		}
		if (count>0)
			System.out.printf("cache: thrown out %d tiles, currently holding %d\n", count, cache.size());
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

	public void getTiles_dummyImplementation1(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		// if we find an exact match, return it, otherwise return full area as missing
		// not very efficient
		for (Tile tile : cache) {
			if (tile.rect.contains(rect)) {
				System.out.println("tile cache HIT!");
				outCachedTiles.add(tile);
				return;
			}
		}

		System.out.println("tile cache MISS!");
		outMissingAreas.add(rect);
	}
	
	public void getTiles(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		// simple algorithm: find best match, and return diff to it as missing
		Tile bestTile = findBestMatch(rect);
		if (bestTile==null) {
			// no overlapping tile, request full rectangle
			outMissingAreas.add(new LargeRect(rect.x, 0, rect.width, virtualHeight));
		}
		else if (bestTile.rect.contains(rect)) {
			// tile covers it all
			outCachedTiles.add(bestTile);
		}
		else {
			// calculate diff rect and see
			LargeRect diff = rect.minus(bestTile.rect);
			if (diff==null) {
				// best tile is no use (actually this cannot happen in ColumnTileCache as all tiles are full height)
				//XXX look for ones where diff!=null already in findBestMatch()!!
				outMissingAreas.add(new LargeRect(rect.x, 0, rect.width, virtualHeight));
			}
			else {
				outCachedTiles.add(bestTile);
				// instead of adding the diff to missingAreas right away, try to find more tiles by recursion
				//outMissingAreas.add(new LargeRect(diff.x, 0, diff.width, virtualHeight));
				getTiles(diff, virtualWidth, virtualHeight, outCachedTiles, outMissingAreas);
			}
		}
	}

	/**
	 * Returns the tile which has the largest overlapping area with rect.
	 * Returns null if no tiles overlap with it.
	 */
	private Tile findBestMatch(LargeRect rect) {
		Tile bestTile = null;
		long bestArea = 0;
		for (Tile tile : cache) {
			if (tile.rect.intersects(rect)) {
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
