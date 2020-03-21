package org.omnetpp.common.largetable;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.StyledString;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;

/**
 * Base class for LargeTable row renderers.
 *
 * @author andras
 */
public abstract class AbstractLargeTableRowRenderer implements ILargeTableRowRenderer {
    protected static final int CELL_LEFT_MARGIN = 4;
    protected Font font = JFaceResources.getDefaultFont();
    protected int fontHeight = 0;
    protected Color foregroundColor = Display.getCurrent().getSystemColor(SWT.COLOR_LIST_FOREGROUND);
    protected Color selectionForegroundColor = Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);
    protected Color selectionBackgroundColor = Display.getCurrent().getSystemColor(SWT.COLOR_LIST_SELECTION);

    public AbstractLargeTableRowRenderer() {
        super();
    }

    public Font getFont() {
        return font;
    }

    public void setFont(Font font) {
        this.font = font;
    }

    public Color getForegroundColor() {
        return foregroundColor;
    }

    public void setForegroundColor(Color foregroundColor) {
        this.foregroundColor = foregroundColor;
    }

    public Color getSelectionForegroundColor() {
        return selectionForegroundColor;
    }

    public void setSelectionForegroundColor(Color selectionForegroundColor) {
        this.selectionForegroundColor = selectionForegroundColor;
    }

    public Color getSelectionBackgroundColor() {
        return selectionBackgroundColor;
    }

    public void setSelectionBackgroundColor(Color selectionBackgroundColor) {
        this.selectionBackgroundColor = selectionBackgroundColor;
    }

    public int getRowHeight(GC gc) {
        if (fontHeight == 0) {
            Font oldFont = gc.getFont();
            gc.setFont(font);
            fontHeight = gc.getFontMetrics().getHeight();
            gc.setFont(oldFont);
        }

        return fontHeight + 3;
    }

    @Override
    public void drawCell(GC gc, int rowIndex, int columnIndex, boolean isSelected) {
        if (isSelected) {
            gc.setForeground(selectionForegroundColor);
            gc.setBackground(selectionBackgroundColor);
        }
        else
            gc.setForeground(foregroundColor);

        StyledString styledString = getStyledText(rowIndex, columnIndex, isSelected);
        int indent = getIndentation(rowIndex, columnIndex);
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