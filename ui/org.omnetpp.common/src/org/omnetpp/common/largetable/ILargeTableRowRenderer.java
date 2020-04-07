/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.largetable;

import org.eclipse.swt.graphics.GC;

/**
 * LargeTable's cells are rendered by using this interface. For practical purposes,
 * you'll probably want to subclass AbstractLargeTableRowRenderer instead of
 * this interface.
 *
 * @andras
 */
public interface ILargeTableRowRenderer {
    /**
     * Returns the height in pixels of a virtual table row including the separator line.
     */
    public int getRowHeight(GC gc);

    /**
     * Draws the given cell using gc starting at the (0, 0) coordinates. Only one text
     * row may be drawn. Coordinate transformation and clipping will be set on
     * GC to enforce drawing be made only in the available area. The font, and
     * the correct background and foreground colors for the current selection and
     * the table's focused state will be also set on the GC. The alignment
     * is the table column's alignment, one of SWT.LEFT, SWT.CENTERED and SWT.RIGHT.
     */
    public void drawCell(GC gc, int rowIndex, int columnIndex, int alignment);

    /**
     * Returns the text of the given cell. This will be used by the table for the
     * "Copy" function.
     */
    public String getText(int rowIndex, int columnIndex);

    /**
     * A tooltip text for the given element, or null if none.
     */
    public String getTooltipText(int rowIndex, int columnIndex);
}
