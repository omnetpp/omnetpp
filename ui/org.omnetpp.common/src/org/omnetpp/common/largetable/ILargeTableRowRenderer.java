/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.largetable;

import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;

/**
 * LargeTable's cells are rendered by using this interface.
 */
public interface ILargeTableRowRenderer {
    /**
     * Returns the height in pixels of a virtual table row including the separator line.
     */
    public int getRowHeight(GC gc);

    /**
     * Draws the given cell using gc starting at 0, 0 coordinates. Only one text
     * row may be drawn. Coordinate transformation and clipping will be set on
     * GC to enforce drawing be made only in the available area.
     */
    public void drawCell(GC gc, int rowIndex, int columnIndex, boolean isSelected);

    /**
     * Returns the indentation (in pixels) of the icon/text of the given cell.
     */
    public int getIndentation(int rowIndex, int columnIndex);

    /**
     * Returns the icon of the given cell.
     */
    public Image getImage(int rowIndex, int columnIndex);

    /**
     * Returns the text of the given cell.
     */
    public StyledString getStyledText(int rowIndex, int columnIndex, boolean isSelected);

    /**
     * A tooltip text for the given element or null.
     */
    public String getTooltipText(int rowIndex, int columnIndex);
}
