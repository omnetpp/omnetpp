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
		discardOldTiles();
		System.out.printf("image %d x %d added to cache, memory usage %dk\n", rect.width, rect.height, memoryUsage/1024);
		return tile;
	}

	private void discardOldTiles() {
		while (memoryUsage > memoryUsageLimit) {
			Assert.isTrue(cache.size()>0);
			Tile tile = cache.remove(0);
			memoryUsage -= imageSize(tile.image);
			tile.image.dispose();
		}
	}

	private int imageSize(Image image) {
		return image.getBounds().width * image.getBounds().height * 4; //XXX refine
	}

	public void clear() {
		for (Tile tile : cache)
				tile.image.dispose();
		cache.clear();
	}

	public void getTiles(LargeRect rect, long virtualWidth, long virtualHeight, List<Tile> outCachedTiles, List<LargeRect> outMissingAreas) {
		// dummy impl: if we find an exact match, return it, otherwise return full area as missing
		//XXX refine
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
}
