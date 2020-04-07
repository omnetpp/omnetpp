package org.omnetpp.common.largetable;

import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextLayout;

/**
 * Base class for LargeTable row renderers.
 *
 * @author andras
 */
public abstract class AbstractLargeTableRowRenderer implements ILargeTableRowRenderer {
    protected static final int CELL_LEFT_MARGIN = 4;

    /**
     * Returns the styled string to be used by drawCell() to paint the contents of the given cell.
     * GC and alignment are those of the drawCell() call, passed here for extracting color,
     * font, or size information if necessary.
     */
    public StyledString getStyledText(int rowIndex, int columnIndex, GC gc, int alignment) {
        String text = getText(rowIndex, columnIndex);
        return new StyledString(text);
    }

    /**
     * Returns the indentation (in pixels) of the icon/text of the given cell.
     */
    public abstract int getIndentation(int rowIndex, int columnIndex);

    /**
     * Returns the icon of the given cell, or null. The icon is drawn to the left of the cell.
     */
    public abstract Image getImage(int rowIndex, int columnIndex);

    public int getRowHeight(GC gc) {
        int fontHeight = gc.getFontMetrics().getHeight();
        return fontHeight + 3;
    }

    @Override
    public void drawCell(GC gc, int rowIndex, int columnIndex, int alignment) {
        //TODO draw icon
        StyledString styledString = getStyledText(rowIndex, columnIndex, gc, alignment);
        int indent = getIndentation(rowIndex, columnIndex);
        //TODO observe alignment
        drawStyledString(gc, styledString, CELL_LEFT_MARGIN + indent, 0);
    }

    protected void drawStyledString(GC gc, StyledString styledString, int x, int y) {
        TextLayout textLayout = new TextLayout(gc.getDevice());
        textLayout.setText(styledString.getString());
        for (StyleRange styleRange : styledString.getStyleRanges())
            textLayout.setStyle(styleRange, styleRange.start, styleRange.start + styleRange.length);
        textLayout.draw(gc, x, y);
        textLayout.dispose();
    }
}
