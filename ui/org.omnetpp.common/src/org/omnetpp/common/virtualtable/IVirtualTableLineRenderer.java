package org.omnetpp.common.virtualtable;

import org.eclipse.swt.graphics.GC;

public interface IVirtualTableLineRenderer<T> {
	public int getLineHeight(GC gc);

	/**
	 * Draws the given item using gc starting at 0, 0 coordinates.
	 * Only one text line may be drawn. Coordinate transformation and clipping will be set on GC to
	 * enforce drawing can be made only in the available area.
	 */
	public void drawCell(GC gc, T element, int index);
}