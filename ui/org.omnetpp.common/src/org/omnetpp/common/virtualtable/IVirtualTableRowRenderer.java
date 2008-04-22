package org.omnetpp.common.virtualtable;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.ui.SizeConstraint;

/**
 * Virtual table cells are rendered by using this interface.
 */
public interface IVirtualTableRowRenderer<T> {
	/**
	 * The virtual table notifies its row renderer whenever the input changes.
	 */
	public void setInput(Object input);

	/**
	 * Returns the height in pixels of a virtual table row including the separator line.
	 */
	public int getRowHeight(GC gc);

	/**
	 * Draws the given cell using gc starting at 0, 0 coordinates.
	 * Only one text row may be drawn. Coordinate transformation and clipping will be set on GC to
	 * enforce drawing be made only in the available area.
	 */
	public void drawCell(GC gc, T element, int index);

	/**
	 * A tooltip text for the given element or null.
	 * @param outSizeConstraint TODO
	 */
	public String getTooltipText(T element, SizeConstraint outSizeConstraint);
}