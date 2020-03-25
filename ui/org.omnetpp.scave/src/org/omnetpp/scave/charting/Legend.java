/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.properties.PlotProperty.LegendAnchor;
import org.omnetpp.scave.charting.properties.PlotProperty.LegendPosition;

/**
 * This class layouts and draws the legend on a vector chart.
 *
 * @author tomi
 */
public class Legend implements ILegend {

    class Item {
        String text;
        Color color;
        IPlotSymbol symbol;
        boolean drawLine;
        int x, y;   // location relative to the legend top-left
        int width, height;

        public Item(Color color, String text, IPlotSymbol symbol, boolean drawLine) {
            this.color = color;
            this.text = text;
            this.symbol = symbol;
            this.drawLine = drawLine;
        }

        public void calculateSize(Graphics graphics) {
            Point size = GraphicsUtils.getTextExtent(graphics, text);
            width = size.x + 17; // place for symbol
            height = size.y;
        }

        public void draw(Graphics graphics, int x, int y) {
            graphics.setAntialias(SWT.ON);
            graphics.setForegroundColor(color);
            graphics.setBackgroundColor(color);

            // draw line
            if (drawLine) {
                graphics.setLineWidth(2);
                graphics.setLineStyle(SWT.LINE_SOLID);
                graphics.drawLine(x+1, y+height/2, x+15, y+height/2);
            }
            // draw symbol
            if (symbol != null) {
                int size = symbol.getSizeHint();
                try {
                    symbol.setSizeHint(6);
                    symbol.drawSymbol(graphics, x+8, y+height/2);
                }
                finally {
                    symbol.setSizeHint(size);
                }
            }

            // draw text
            if (font != null) graphics.setFont(font);
            graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText(text, x + 17, y);
        }
    }

    private boolean visible;
    private boolean drawBorder;
    private Font font;
    private LegendPosition position;
    private LegendAnchor anchor;
    private int horizontalSpacing = 10; // horizontal spacing between legend items
    private int verticalSpacing = 5;   // vertical spacing between legend items
    private int horizontalPadding = 5; // horizontal space to the left and right of the legend, inside the border
    private int verticalPadding = 5;   // vertical space above and below the the legend, inside the border
    private int horizontalMargin = 5;  // horizontal space to the left and right of the legend, outside the border
    private int verticalMargin = 5;    // vertical space above and below the legend, outside the border
    private List<Item> items = new ArrayList<Item>();
    private Rectangle bounds; // rectangle of the legend in canvas coordinates
    int visibleItemCount;

    public Legend() {
    }

    public Legend(boolean visible, boolean drawBorder, Font font, LegendPosition position, LegendAnchor anchor) {
        this.visible = visible;
        this.drawBorder = drawBorder;
        this.font = font;
        this.position = position;
        this.anchor = anchor;
    }

    public void clearItems() {
        items.clear();
    }

    public void addItem(Color color, String text, IPlotSymbol symbol, boolean drawLine) {
        items.add(new Item(color, text, symbol, drawLine));
    }

    public boolean isVisible() {
        return visible;
    }

    public void setVisible(boolean flag) {
        visible = flag;
    }

    public boolean getDrawBorder() {
        return drawBorder;
    }

    public void setDrawBorder(boolean flag) {
        drawBorder = flag;
    }

    public Font getFont() {
        return font;
    }

    public void setFont(Font font) {
        this.font = font;
    }

    public LegendPosition getPosition() {
        return position;
    }

    public void setPosition(LegendPosition position) {
        this.position = position;
    }

    public LegendAnchor getAnchor() {
        return anchor;
    }

    public void setAnchor(LegendAnchor anchor) {
        this.anchor = anchor;
    }

    public Rectangle getBounds() {
        return bounds;
    }

    /**
     * Calculates the position and size of the legend and its items.
     * Returns the remaining space.
     * @param pass TODO
     */
    public Rectangle layout(Graphics graphics, Rectangle parent, int pass) {
        if (pass == 1) {
            if (!visible)
                return parent;

            //Debug.println("Parent rect: " + parent);

            graphics.setFont(font);

            // measure items
            for (int i = 0; i < items.size(); ++i)
                items.get(i).calculateSize(graphics);

            // position items and calculate size
            boolean horizontal = position == LegendPosition.Above || position == LegendPosition.Below ||
                                    position == LegendPosition.Inside &&
                                    (anchor == LegendAnchor.North || anchor == LegendAnchor.South);
            Point maxSize = new Point(parent.width, parent.height);
            if (horizontal)
                maxSize.y = parent.height / 2;
            else
                maxSize.x = parent.width / 2;
            positionItems(maxSize, horizontal);

            //Debug.println("Bounds: "+bounds+", Max: "+maxSize);

            bounds.width = Math.min(bounds.width, maxSize.x);
            bounds.height = Math.min(bounds.height, maxSize.y);

            // calculate legend position
            int dx, dy;
            switch (anchor) {
            case North:     dx = 0; dy = -1; break;
            case NorthEast: dx = 1; dy = -1; break;
            case East:      dx = 1; dy = 0; break;
            case SouthEast: dx = 1; dy = 1; break;
            case South:     dx = 0; dy = 1; break;
            case SouthWest: dx = -1; dy = 1; break;
            case West:      dx = -1; dy = 0; break;
            case NorthWest: dx = -1; dy = -1; break;
            default: throw new IllegalStateException();
            }

            int top = parent.y;
            int left = parent.x;
            int bottom = parent.y + parent.height;
            int right = parent.x + parent.width;

            switch (position) {
            case Above:
                bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
                bounds.y = top + verticalMargin;
                top = Math.min(top + bounds.height + 2 * verticalMargin, bottom);
                break;
            case Below:
                bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
                bounds.y = bottom - verticalMargin - bounds.height;
                bottom = Math.max(bottom - bounds.height - 2 * verticalMargin, top);
                break;
            case Left:
                bounds.x = left + horizontalMargin;
                bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
                left = Math.min(left + bounds.width + 2 * horizontalMargin, right);
                break;
            case Right:
                bounds.x = right - bounds.width - horizontalMargin;
                bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
                right = Math.max(right - bounds.width - 2 * horizontalMargin, left);
                break;
            case Inside:
                bounds.x = left + (parent.width - bounds.width) * (dx + 1) / 2;
                bounds.y = top + (parent.height - bounds.height) * (dy + 1) / 2;
                break;
            default:
                throw new IllegalStateException();
            }

            return new Rectangle(left, top, right - left, bottom - top);
        }
        else {
            if (!visible || position != LegendPosition.Inside)
                return parent;

            int dx, dy;
            switch (anchor) {
            case North:     dx = 0; dy = -1; break;
            case NorthEast: dx = 1; dy = -1; break;
            case East:      dx = 1; dy = 0; break;
            case SouthEast: dx = 1; dy = 1; break;
            case South:     dx = 0; dy = 1; break;
            case SouthWest: dx = -1; dy = 1; break;
            case West:      dx = -1; dy = 0; break;
            case NorthWest: dx = -1; dy = -1; break;
            default: throw new IllegalStateException();
            }

            bounds.width = Math.min(bounds.width, Math.max(0, parent.width - 2 * horizontalMargin));
            bounds.height = Math.min(bounds.height, Math.max(0, parent.height - 2 * verticalMargin));
            bounds.x = parent.x + horizontalMargin + (parent.width - bounds.width - 2 * horizontalMargin) * (dx + 1) / 2;
            bounds.y = parent.y + verticalMargin + (parent.height - bounds.height - 2 * verticalMargin) * (dy + 1) / 2;
            updateVisibleItemCount();

            return parent;
        }
    }

    private void positionItems(Point maxSize, boolean horizontal) {
        if (horizontal) {
            // calculate initial number of columns
            int x = 0, columns = 0;
            for (Item item : items) {
                if (x + item.width > maxSize.x)
                    break;
                x += item.width;
                ++columns;
            }
            if (columns == 0)
                columns = 1;
            // layout items in a table in with the given columns,
            // decrease the number of columns until the table width is smaller than the max width
            for (; columns >= 1; --columns) {
                // calculate cell sizes
                int rows = (items.size() - 1)/ columns + 1;
                int[] columnWidths = new int[columns];
                int[] rowHeights = new int[rows];
                for (int i = 0; i < items.size(); ++i) {
                    Item item = items.get(i);
                    int col = i % columns;
                    int row = i / columns;
                    columnWidths[col] = Math.max(columnWidths[col], item.width);
                    rowHeights[row] = Math.max(rowHeights[row], item.height);
                }
                // calculate table size
                bounds = new Rectangle(0, 0, 0, 0);
                bounds.width = (columns - 1) * horizontalSpacing + 2 * horizontalMargin;
                bounds.height = (rows - 1) * verticalSpacing + 2 * verticalMargin;
                for (int i = 0; i < columns; ++i)
                    bounds.width += columnWidths[i];
                for (int i = 0; i < rows; ++i)
                    bounds.height += rowHeights[i];


                // retry with fewer columns if too wide
                if (columns > 1 && bounds.width > maxSize.x)
                    continue;

                // position children
                visibleItemCount = 0;
                x = horizontalPadding;
                int y = verticalPadding;
                for (int i = 0; i < items.size(); ++i) {
                    Item item = items.get(i);
                    int row = i / columns;
                    int col = i % columns;

                    if (col == 0) {
                        if(y + rowHeights[row] + verticalPadding > maxSize.y) {
                            bounds.height = y + verticalPadding;
                            break;
                        }
                        if (row > 0)
                            y += verticalSpacing;
                    }
                    else { // col > 0
                        x += horizontalSpacing;
                    }

                    item.x = x;
                    item.y = y;

                    if (col == columns - 1) {
                        x = horizontalPadding;
                        y += rowHeights[row];
                    }
                    else {
                        x += columnWidths[col];
                    }

                    ++visibleItemCount;
                }

                break;
            }
        }
        else { // vertical
            bounds = new Rectangle(0, 0, 0, 0);
            visibleItemCount = 0;
            int x = horizontalPadding;
            int y = verticalPadding;
            for (int i = 0; i < items.size(); ++i) {
                Item item = items.get(i);

                if (y + item.height + verticalPadding > maxSize.y) {
                    break;
                }

                item.x = x;
                item.y = y;

                bounds.width = Math.max(bounds.width, item.width + 2 * horizontalPadding);
                bounds.height = Math.max(bounds.height, y + item.height + verticalPadding);

                y += item.height + verticalSpacing;
                ++visibleItemCount;
            }
        }
    }

    public void updateVisibleItemCount() {
        int count = 0;
        Item lastVisibleItem = null;
        for (Item item : items) {
            if (count == visibleItemCount || item.y + item.height + verticalPadding > bounds.height)
                break;
            count++;
            lastVisibleItem = item;
        }

        if (count != visibleItemCount) {
            visibleItemCount = count;
            if (lastVisibleItem == null)
                bounds.height = 2 * verticalPadding;
            else
                bounds.height = Math.max(0, lastVisibleItem.y + lastVisibleItem.height + verticalPadding);
        }
    }

    /**
     * Draws the legend to the canvas.
     */
    public void draw(Graphics graphics) {
        if (!visible)
            return;
        graphics.pushState();
        try {
            // draw background and border
            graphics.setBackgroundColor(ColorFactory.WHITE);
            Rectangle clip = graphics.getClip(new Rectangle());
            graphics.setClip(new Rectangle(bounds.x, bounds.y, bounds.width + 1, bounds.height + 1).intersect(clip));
            graphics.fillRectangle(bounds);
            if (drawBorder) {
                graphics.setForegroundColor(ColorFactory.BLACK);
                graphics.drawRectangle(bounds);
            }
            // draw items
            int left = bounds.x;
            int top = bounds.y;
            int count = visibleItemCount == items.size() ? visibleItemCount : visibleItemCount - 1;
            for (int i = 0; i < count; i++) {
                Item item = items.get(i);
                item.draw(graphics, left+item.x, top + item.y);
            }
            // draw "... and X more" text in place of the last visible item
            if (visibleItemCount < items.size()) {
                int x=left, y = top;
                if (visibleItemCount > 0) {
                    Item item = items.get(visibleItemCount-1);
                    x += item.x;
                    y += item.y;
                }
                graphics.drawText(String.format("... and %d more", items.size() - visibleItemCount + 1), x, y);
            }
        }
        finally {
            graphics.popState();
        }
    }
}
