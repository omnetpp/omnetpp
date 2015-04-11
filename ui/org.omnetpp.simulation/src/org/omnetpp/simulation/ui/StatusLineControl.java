/*--------------------------------------------------------------*
   Copyright (C) 2006-2015 OpenSim Ltd.

   This file is distributed WITHOUT ANY WARRANTY. See the file
   'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.simulation.ui;

import org.eclipse.draw2d.Cursors;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;

/**
 * Custom status line control. Essentially acts as a row of labels, but has
 * special layout behavior, graphical separators between items, and abbreviates
 * texts that don't fit using ellipses.
 *
 * <p>
 * Layout behavior:
 *
 * <ol>
 * <li>Item widths are kept in pixels, not as weights.
 * <li>Initially, labels are packed left to right, each item occupying
 * exactly as much space as it needs.
 * <li>Items can only grow, so when an item's text is updated, the item
 * either stays the same width or becomes wider. This is done to avoid jumping
 * of items when the status line is updated frequently.
 * <li>Users can drag the separators (sash-like behavior). User-modified item
 * widths always stay the same, never shrink or grow as a result of updating the
 * item text.
 * </ol>
 *
 * @author Andras
 */
public class StatusLineControl extends Canvas {
    private static final int DEFAULT_WIDTH = 500;  // whatever; not really used
    private static final int DEFAULT_HEIGHT = 20;

    // drawing constants
    private final int marginWidth = 5;
    private final int marginHeight = 1;
    private int padding = 10;
    private Color separatorColor = Display.getCurrent().getSystemColor(SWT.COLOR_WIDGET_LIGHT_SHADOW);

    static class Item {
        String text = "";
        int width = 2;
        boolean strict = false; // width is allowed to grow or not (strict == not allowed)
    };

    private Item items[] = new Item[0];
    private int sashDragged = -1;
    private int dragStartMouseX;
    private int dragStartWidth;

    /**
     * Constructs a new control.
     */
    public StatusLineControl(Composite parent, int style) {
        super(parent, style | SWT.DOUBLE_BUFFERED);

        addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                internalRepaint(e.gc);
            }
        });

        addMouseMoveListener(new MouseMoveListener() {
            @Override
            public void mouseMove(MouseEvent e) {
                if (sashDragged == -1) {
                    int sashIndex = findSashNear(e.x);
                    Cursor cursor = (sashIndex==-1) ? null : Cursors.SIZEW;
                    if (getCursor() != cursor)
                        setCursor(cursor);
                }
                else {
                    items[sashDragged].strict = true;
                    int dx = e.x - dragStartMouseX;
                    items[sashDragged].width = Math.max(0, dragStartWidth + dx);
                    redraw();
                }
            }
        });

        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                sashDragged = findSashNear(e.x);
                if (sashDragged != -1) {
                    dragStartMouseX = e.x;
                    dragStartWidth = items[sashDragged].width;
                }
            }
            @Override
            public void mouseUp(MouseEvent e) {
                sashDragged = -1;
            }
        });
    }

    /**
     * Sets the number of items. As a side effect, existing items will be deleted.
     */
    public void setItemCount(int n) {
        items = new Item[n];
        for (int i = 0; i < n; i++)
            items[i] = new Item();
    }

    public int getItemCount() {
        return items.length;
    }

    public void setItemText(int index, String text) {
        items[index].text = text;
        redraw();
    }

    public String getItemText(int index) {
        return items[index].text;
    }

    public void setItemWidth(int index, int width, boolean strict) {
        items[index].width = width;
        items[index].strict = strict;
        redraw();
    }

    public int getItemWidth(int index) {
        return items[index].width;
    }

    public boolean isItemWidthStrict(int index) {
        return items[index].strict;
    }

    public int getPadding() {
        return padding;
    }

    public void setPadding(int padding) {
        this.padding = padding;
        redraw();
    }

    public void clearItemTexts() {
        for (Item item : items)
            item.text = "";
    }

    public void resetWidths() {
        for (Item item : items) {
            item.width = 0;
            item.strict = false;
        }
    }

    public void resetNonStrictWidths() {
        for (Item item : items)
            if (!item.strict)
                item.width = 0;
    }

    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        if (wHint == SWT.DEFAULT) wHint = DEFAULT_WIDTH;
        if (hHint == SWT.DEFAULT) hHint = DEFAULT_HEIGHT;
        return super.computeSize(wHint, hHint, changed);
    }

    protected void internalRepaint(GC gc) {
        Rectangle r = getClientArea();
        Color fg = gc.getForeground();

        int x = marginWidth;
        for (int i = 0; i < items.length; i++) {
            // determine item width and abbreviate text if needed
            int spaceLeft = r.width - 2*marginWidth - x;
            if (spaceLeft <= 0)
                break;
            Item item = items[i];
            String text = item.text;
            int textWidth = gc.textExtent(text).x;
            if (textWidth > item.width || textWidth > spaceLeft) {
                if (item.strict)
                    text = abbreviate(text, gc, Math.min(item.width, spaceLeft));
                else if (textWidth <= spaceLeft)
                    item.width = textWidth; // enlarge item
                else {
                    text = abbreviate(text, gc, spaceLeft);
                    item.width = spaceLeft;
                }
            }

            // draw
            gc.setForeground(fg);
            gc.drawText(text, x, marginHeight);
            x += item.width + padding;
            if (i != items.length - 1) {
                gc.setForeground(separatorColor);
                gc.drawLine(x, 0, x, r.height);
                x += padding;
            }
        }

    }

    protected String abbreviate(String text, GC gc, int space) {
        while (text.length() > 1) {
            text = text.substring(0, text.length()-1); // chop last char
            String text2 = text + "...";
            if (gc.textExtent(text2).x <= space)
                return text2;
        }
        return "";
    }

    protected int findSashNear(int mouseX) {
        int x = marginWidth;
        for (int i = 0; i < items.length; i++) {
            x += items[i].width + padding;
            if (Math.abs(mouseX - x) < 2)
                return i;
            x += padding;
        }
        return -1;
    }
}
