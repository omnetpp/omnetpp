package org.omnetpp.common.virtualtable;

import org.eclipse.swt.graphics.GC;

/**
 * Virtual table cells are rendered by using this interface.
 */
public interface IVirtualTableLineRenderer<T> {
	/**
	 * The virtual table notifies its line renderer whenever the input changes.
	 */
	public void setInput(Object input);

	/**
	 * Returns the height in pixels of a virtual table line including the separator line.
	 */
	public int getLineHeight(GC gc);

	/**
	 * Draws the given cell using gc starting at 0, 0 coordinates.
	 * Only one text line may be drawn. Coordinate transformation and clipping will be set on GC to
	 * enforce drawing be made only in the available area.
	 */
	public void drawCell(GC gc, T element, int index);
}