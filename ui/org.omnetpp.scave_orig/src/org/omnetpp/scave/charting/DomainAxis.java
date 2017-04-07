/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_WRAP_LABELS;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Vector;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GeomUtils;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.dataset.IScalarDataset;

/**
 * Domain axis for bar chart.
 */
class DomainAxis {

    ScalarChart chart;
    Rectangle rect; // strip below the plotArea where the axis text etc goes
    Vector<LineData> lines = new Vector<LineData>();
    String title = DEFAULT_X_AXIS_TITLE;
    boolean drawLabels = true;
    boolean drawTitle = true;
    Font titleFont = DEFAULT_AXIS_TITLE_FONT;
    Font labelsFont = DEFAULT_LABELS_FONT;
    double rotation = DEFAULT_X_LABELS_ROTATED_BY;
    boolean wrapLabels = DEFAULT_WRAP_LABELS;
    int gap = 4;  // between chart and axis

    static class LabelData {
        TextLayout textLayout;
        int row;
        int centerY;
        Dimension size;
        Dimension rotatedSize;

        public void dispose() {
            if (textLayout != null)
                textLayout.dispose();
        }
    }

    final static Comparator<LineData> lineDataRightEdgeComparator = new Comparator<LineData>() {
        public int compare(LineData first, LineData second) {
            return first.right == second.right ? 0 :
                    first.right < second.right ? -1 : 1;
        }
    };

    static class LineData {
        int height;
        long right;
        List<LabelData> labels = new ArrayList<LabelData>();

        public LineData() {
            height = 0;
            right = Integer.MIN_VALUE;
        }
    }

    public DomainAxis(ScalarChart chart) {
        this.chart = chart;
    }

    public void setLabels(boolean drawLabels) {
        this.drawLabels = drawLabels;
    }

    public void setDrawTitle(boolean drawTitle) {
        this.drawTitle = drawTitle;
    }

    public void dispose() {
        for (LineData line : lines)
            for (LabelData label : line.labels)
                if (label != null)
                    label.dispose();
    }

    /**
     * Modifies insets to accomodate room for axis title, ticks, tick labels etc.
     * Also returns insets for convenience.
     */
    public Insets layout(Graphics graphics, Rectangle bounds, Insets insets, ICoordsMapping coordsMapping, int pass) {
        if (pass == 1) {
            // measure title height and labels height
            graphics.setFont(titleFont);
            int titleHeight = !drawTitle || title.equals("") ? 0 : GraphicsUtils.getTextExtent(graphics, title).y;
            graphics.setFont(labelsFont);
            lines.add(new LineData());
            IScalarDataset dataset = chart.getDataset();
            BarPlot plot = chart.getPlot();
            Dimension maxSize = new Dimension(-1, Math.max(bounds.height / 2, 10));
            if (dataset != null) {
                // dispose previous TextLayouts
                for (LineData line : lines)
                    for (LabelData label : line.labels)
                        label.dispose();
                lines.clear();
                lines.add(new LineData());

                int cColumns = dataset.getColumnCount();
                int cRows = dataset.getRowCount();
                for (int row = 0; row < cRows; ++row) {
                    long left = plot.getBarRectangle(row, 0, coordsMapping).x;
                    long right = plot.getBarRectangle(row, cColumns - 1, coordsMapping).right();

                    LineData line = Collections.min(lines, lineDataRightEdgeComparator);
                    LabelData label;

                    if (wrapLabels) {
                        maxSize.width = rotation == 0.0 ? (int)Math.max(right - left, 10) : -1;
                        label = layoutGroupLabel(row, labelsFont, rotation, graphics, maxSize);
                    }
                    else {
                        label = layoutGroupLabel(row, labelsFont, rotation, graphics, maxSize);

                        if ((left + right) / 2 - label.rotatedSize.width / 2 < line.right) {
                            line = new LineData();
                            lines.add(line);
                        }
                    }

                    line.labels.add(label);
                    line.height = Math.max(line.height, label.rotatedSize.height);
                    line.right = (left + right) / 2 + label.rotatedSize.width / 2;
                }
            }

            // position labels vertically
            int labelsHeight = 0;
            if (drawLabels) {
                for (LineData line : lines) {
                    for (LabelData label : line.labels)
                        label.centerY = labelsHeight + line.height / 2;
                    labelsHeight += line.height;
                }
                labelsHeight = Math.min(labelsHeight, maxSize.height);
            }

            // modify insets with space required
            insets.top = Math.max(insets.top, 10); // leave a few pixels at the top
            insets.bottom = Math.max(insets.bottom, gap + labelsHeight + titleHeight + 8);

            return insets;
        }
        else {
            rect = bounds.getCopy();
            int bottom = rect.bottom();
            rect.height = insets.bottom;
            rect.y = bottom - rect.height;
            rect.x += insets.left;
            rect.width -= insets.getWidth();
            return insets;
        }
    }

    private LabelData layoutGroupLabel(int row, Font font, double rotation , Graphics graphics, Dimension maxSize) {
        LabelData data = new LabelData();
        String label = chart.getDataset().getRowKey(row);
        data.row = row;
        data.textLayout = new TextLayout(Display.getDefault());
        data.textLayout.setText(label.replace(';', '\u00ad')); // hyphenate at ';'
        data.textLayout.setFont(font);
        data.textLayout.setAlignment(SWT.CENTER);
        if (maxSize.width > 0)
            data.textLayout.setWidth(maxSize.width);
        //Debug.format("width=%s%n", maxWidth);
        org.eclipse.swt.graphics.Rectangle bounds = data.textLayout.getBounds();
        data.size = new Dimension(bounds.width, Math.min(bounds.height, maxSize.height)); //new Dimension(graphics.textExtent(data.label));
        data.rotatedSize = GeomUtils.rotatedSize(data.size, rotation);
        return data;
    }

    public void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        graphics.pushState();

        graphics.setClip(rect);

        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setLineWidth(1);
        graphics.setForegroundColor(ColorFactory.BLACK);

        BarPlot plot = chart.getPlot();
        Rectangle plotRect = plot.getRectangle();

        // draw labels
        if (drawLabels) {
            IScalarDataset dataset = chart.getDataset();
            if (dataset != null) {
                int cColumns = dataset.getColumnCount();
                int cRows = dataset.getRowCount();

                // draw lines
                for (int row = 0; row < cRows; ++row) {
                    long left = plot.getBarRectangle(row, 0, coordsMapping).x;
                    long right = plot.getBarRectangle(row, cColumns - 1, coordsMapping).right();
                    LargeGraphics.drawLine(graphics, left, rect.y + gap, right, rect.y + gap);
                }

                // draw labels
                graphics.setFont(labelsFont);
                graphics.drawText("", 0, 0); // force Graphics push the font setting into graphics
                graphics.pushState(); // for restoring the transform
                for (LineData line : lines) {
                    for (LabelData label : line.labels) {
                        graphics.restoreState();
                        long left = plot.getBarRectangle(label.row, 0, coordsMapping).x;
                        long right = plot.getBarRectangle(label.row, cColumns - 1, coordsMapping).right();
                        Dimension size = label.size;
                        Dimension rotatedSize = label.rotatedSize;

                        if (isRectangularAngle(rotation)) // center into the cell
                            graphics.translate((int)(left + right) / 2, rect.y + gap + 1 + label.centerY);
                        else // left at the bottom of the bar
                            graphics.translate((int)(left + right) / 2 + rotatedSize.width / 2, rect.y + gap + 1 + label.centerY);
                        try {
                            graphics.rotate((float)-rotation);
                            graphics.drawTextLayout(label.textLayout, - size.width / 2, - size.height/2);
                        } catch (RuntimeException e) {
                            // Fallback for Graphics SVG (does not implement rotate and textLayout).
                            graphics.drawText(label.textLayout.getText(), - size.width / 2, - size.height/2);
                        }
                    }
                }
                graphics.popState();
            }
        }

        // draw axis title
        if (drawTitle) {
            graphics.setFont(titleFont);
            graphics.drawText("", 0, 0); // force Graphics push the font setting into graphics
            Point size = GraphicsUtils.getTextExtent(graphics, title);
            graphics.drawText(title, plotRect.x + (plotRect.width - size.x) / 2, rect.bottom() - size.y - 1);
        }

        graphics.popState();
    }

    private static boolean isRectangularAngle(double degree) {
        return Math.abs(Math.IEEEremainder(degree, 90.0)) < 10.0;
    }
}