package org.omnetpp.common.largetable;

import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.SWT;
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
    protected static final int CELL_HORIZONTAL_MARGIN = 4;
    protected static final int VERTICAL_SPACING = 0;
    protected static final int HORIZONTAL_SPACING = 0;

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
    public void drawCell(GC gc, int rowIndex, int columnIndex, int columnWidth, int alignment) {
        int x = CELL_HORIZONTAL_MARGIN + getIndentation(rowIndex, columnIndex);

        Image image = getImage(rowIndex, columnIndex);
        if (image != null) {
            gc.drawImage(image, x, (getRowHeight(gc) - image.getBounds().height) / 2);
            x += image.getBounds().width + HORIZONTAL_SPACING;
        }

        StyledString styledString = getStyledText(rowIndex, columnIndex, gc, alignment);
        drawStyledString(gc, styledString, x, VERTICAL_SPACING / 2, columnWidth, alignment);
    }

    protected void drawStyledString(GC gc, StyledString styledString, int x, int y, int cellWidth, int alignment) {
        TextLayout textLayout = new TextLayout(gc.getDevice());
        textLayout.setText(styledString.getString());
        for (StyleRange styleRange : styledString.getStyleRanges())
            textLayout.setStyle(styleRange, styleRange.start, styleRange.start + styleRange.length);

        // not using textLayout.setWidth and textLayout.setAlignment because those would
        // unconditionally wrap (and hyphenate) the text, instead of clipping (or eliding)
        int alignmentOffset = 0;
        if (alignment != SWT.LEFT) {
            int textWidth = textLayout.getBounds().width; // may be costly
            int availableWidth  = cellWidth - CELL_HORIZONTAL_MARGIN - x;
            switch (alignment) {
            case SWT.RIGHT: alignmentOffset = availableWidth - textWidth; break;
            case SWT.CENTER: alignmentOffset = (availableWidth - textWidth) / 2; break;
            }
        }

        textLayout.draw(gc, x + Math.max(0, alignmentOffset), y);
        textLayout.dispose();
    }
}


