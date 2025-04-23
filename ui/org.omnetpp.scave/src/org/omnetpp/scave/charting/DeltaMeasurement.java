/*--------------------------------------------------------------*
  Copyright (C) 2006-2025 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.LinePlot.DataPoint;
import org.omnetpp.scave.charting.LinePlot.LineProperties;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.ILinePlotter;

/**
 * Handles delta measurement functionality for line charts.
 *
 * This class provides methods to find the nearest data point, set markers,
 * calculate and display delta values, and handle key events (A/S/D/X).
 */
public class DeltaMeasurement {

    private final LinePlot parent;

    // How close the mouse must be to a point or line segment to be
    // considered "near", in pixels.
    private static final double PICKING_THRESHOLD = 10.0;
    private static final int TEXT_MARGIN = 2;

    // Delta measurement endpoints
    private DeltaEndpoint endpointA = null;
    private DeltaEndpoint endpointB = null;

    // Marker properties
    private static final int MARKER_RADIUS = 5;
    private static final Color MARKER_COLOR = ColorFactory.RED;

    /**
     * Inner class to encapsulate a delta measurement endpoint.
     * Contains a DataPoint (which may be null for interpolated points)
     * and the exact coordinates of the point.
     */
    private static class DeltaEndpoint {
        private DataPoint dataPoint;  // Can be null for interpolated points (unused at the moment)
        private double x, y;          // Always present

        public DeltaEndpoint(DataPoint dataPoint, double x, double y) {
            this.dataPoint = dataPoint;
            this.x = x;
            this.y = y;
        }

        @Override
        public boolean equals(Object other) {
            if (other == null || !(other instanceof DeltaEndpoint)) {
                return false;
            }
            DeltaEndpoint otherEndpoint = (DeltaEndpoint)other;
            return this.x == otherEndpoint.x && this.y == otherEndpoint.y
                    && ObjectUtils.equals(this.dataPoint, otherEndpoint.dataPoint);
        }
    }

    /**
     * Creates a new delta measurement handler for the given line plot.
     */
    public DeltaMeasurement(LinePlot parent) {
        this.parent = parent;
    }

    /**
     * Handles key press events for delta measurement.
     * Returns true if the event was handled, false otherwise.
     */
    public boolean handleKeyPress(char key, int x, int y, ICoordsMapping coordsMapping) {

        // start state    event    condition    action    end state

        switch (key) {
        case 'a':
        case 'A': {
            DeltaEndpoint nearest = findNearestPoint(x, y, coordsMapping);

            if (nearest == null) {
                // begin    A    anywhere else    nothing    begin
                return false;
            }

            if (endpointA == null) {
                // no    A    near point    mark begin point    begin
                // end    A    near point    mark new begin point    segment
                endpointA = nearest;
                return true;
            }

            if (nearest.equals(endpointA)) {
                // begin    A    near begin point    clear    no
                endpointA = null; // !!! missing from spec: in segment state, clear begin point, goto end state
                return true;
            }

            if (nearest.equals(endpointB)) {
                // segment    A    near end point    mark new begin point, unmark end point    begin
                endpointB = null;
                endpointA = nearest;
                return true;
            }

            // begin    A    near non-begin point    mark new begin point    begin
            // segment    A    near non-begin point    mark new begin point    segment
            endpointA = nearest;
            return true;
        }
        case 's':
        case 'S': {
            DeltaEndpoint[] segment = findNearestSegment(x, y, coordsMapping);

            if (segment == null || segment.length < 2)
                return false;

            // segment    S    near marked segment    clear    no
            if (segment[0].equals(endpointA) && segment[1].equals(endpointB)) {
                endpointA = null;
                endpointB = null;
                return true;
            }

            // any    S    near unmarked segment    mark new begin/end points    segment
            endpointA = segment[0];
            endpointB = segment[1];

            return true;
        }
        case 'd':
        case 'D': {
            DeltaEndpoint nearest = findNearestPoint(x, y, coordsMapping);

            if (nearest == null) {
                // end    D    anywhere else    nothing    end
                return false;
            }

            if (endpointB == null) {
                // no    D    near point    mark end point    end
                // begin    D    near point    mark new end point    segment
                endpointB = nearest;
                return true;
            }

            if (nearest.equals(endpointB)) {
                // end    D    near end point    clear    no
                endpointB = null; // !!! missing from spec: in segment state, clear end point, goto begin state
                return true;
            }

            if (nearest.equals(endpointA)) {
                // segment    D    near begin point    mark new end point, unmark begin point    end
                endpointA = null;
                endpointB = nearest;
                return true;
            }

            // end    D    near non-end point    mark new end point    end
            // segment    D    near non-end point    mark new end point    segment
            endpointB = nearest;
            return true;
        }
        case 'x':
        case 'X':
            // any    X    always    clear    no
            clearMeasurement();
            return true;
        }

        return false;
    }

    /**
     * Finds the nearest data point to the mouse cursor, including interpolated points
     * for step plots.
     */
    private DeltaEndpoint findNearestPoint(int x, int y, ICoordsMapping coordsMapping) {
        final double maxDistSq = PICKING_THRESHOLD * PICKING_THRESHOLD;

        IXYDataset dataset = parent.getDataset();
        if (dataset == null) {
            return null;
        }

        double minDistSq = Double.POSITIVE_INFINITY;
        DeltaEndpoint closestEndpoint = null;

        for (int series : parent.getLegend().getEnabledItemIndices()) {
            LineProperties props = parent.getLineProperties(series);
            if (!props.getEffectiveDisplayLine()) {
                continue;
            }

            ILinePlotter linePlotter = props.getLinePlotter();
            int n = dataset.getItemCount(series);
            if (n == 0) {
                continue;
            }

            // Check the first point separately if n=1 or if the first segment is skipped
            if (n == 1 || Double.isNaN(dataset.getY(series, 0)) || Double.isNaN(dataset.getY(series, 1))) {
                double dataX1 = dataset.getX(series, 0);
                double dataY1 = dataset.getY(series, 0);
                if (!Double.isNaN(dataY1)) {
                    double screenX1 = coordsMapping.toCanvasX(parent.transformX(dataX1));
                    double screenY1 = coordsMapping.toCanvasY(parent.transformY(dataY1));
                    double distSq = distSq(x, y, screenX1, screenY1);
                    if (distSq < minDistSq && distSq <= maxDistSq) {
                        minDistSq = distSq;
                        closestEndpoint = new DeltaEndpoint(new DataPoint(series, 0), dataX1, dataY1);
                    }
                }
            }

            // Iterate through segments to find vertices
            for (int i = 0; i < n - 1; ++i) {
                double dataX1 = dataset.getX(series, i);
                double dataY1 = dataset.getY(series, i);
                double dataX2 = dataset.getX(series, i + 1);
                double dataY2 = dataset.getY(series, i + 1);

                // Skip if either point is NaN
                if (Double.isNaN(dataY1) || Double.isNaN(dataY2)) {
                    // If dataY2 is NaN, we still need to check the vertex at dataX1, dataY1
                    if (!Double.isNaN(dataY1)) {
                        double screenX1 = coordsMapping.toCanvasX(parent.transformX(dataX1));
                        double screenY1 = coordsMapping.toCanvasY(parent.transformY(dataY1));
                        double distSq = distSq(x, y, screenX1, screenY1);
                        if (distSq < minDistSq && distSq <= maxDistSq) {
                            minDistSq = distSq;
                            closestEndpoint = new DeltaEndpoint(new DataPoint(series, i), dataX1, dataY1);
                        }
                    }
                    continue;
                }

                // Get segment vertices in data coordinates using the centralized method
                double[] segmentDataPoints = linePlotter.getSegmentPoints(dataX1, dataY1, dataX2, dataY2);

                if (segmentDataPoints.length == 0) {
                    // Even if no segments, check the first point of the pair
                    double screenX1 = coordsMapping.toCanvasX(parent.transformX(dataX1));
                    double screenY1 = coordsMapping.toCanvasY(parent.transformY(dataY1));
                    double distSq = distSq(x, y, screenX1, screenY1);
                    if (distSq < minDistSq && distSq <= maxDistSq) {
                        minDistSq = distSq;
                        closestEndpoint = new DeltaEndpoint(new DataPoint(series, i), dataX1, dataY1);
                    }
                    continue; // No line segments for this style or pair
                }

                // Iterate through the vertices defined by the segment points
                for (int j = 0; j < segmentDataPoints.length; j += 2) {
                    double vx = segmentDataPoints[j];
                    double vy = segmentDataPoints[j + 1];

                    // Convert vertex to screen coordinates for distance check
                    double screenX = coordsMapping.toCanvasX(parent.transformX(vx));
                    double screenY = coordsMapping.toCanvasY(parent.transformY(vy));

                    // Calculate squared distance from mouse point (sx, sy) to the screen vertex
                    double distSq = distSq(x, y, screenX, screenY);

                    if (distSq < minDistSq && distSq <= maxDistSq) {
                        minDistSq = distSq;
                        // Determine if the vertex corresponds to an original data point
                        DataPoint dp = null;
                        if (vx == dataX1 && vy == dataY1) {
                            dp = new DataPoint(series, i);
                        } else if (vx == dataX2 && vy == dataY2) {
                            dp = new DataPoint(series, i + 1);
                        }
                        closestEndpoint = new DeltaEndpoint(dp, vx, vy);
                    }
                }
            }

            // Check the last point of the series explicitly, as it's not covered as a start point in the loop
            if (n > 0 && !Double.isNaN(dataset.getY(series, n - 1))) {
                double dataXLast = dataset.getX(series, n - 1);
                double dataYLast = dataset.getY(series, n - 1);
                double screenXLast = coordsMapping.toCanvasX(parent.transformX(dataXLast));
                double screenYLast = coordsMapping.toCanvasY(parent.transformY(dataYLast));
                double distSq = distSq(x, y, screenXLast, screenYLast);
                if (distSq < minDistSq && distSq <= maxDistSq) {
                    minDistSq = distSq;
                    closestEndpoint = new DeltaEndpoint(new DataPoint(series, n - 1), dataXLast, dataYLast);
                }
            }
        }

        return closestEndpoint;
    }


    /**
     * Finds the nearest line segment to the mouse cursor, considering draw style.
     * Returns a two-element array with the DeltaEndpoints of the nearest segment,
     * or null if no segment is found within PICKING_THRESHOLD.
     */
    private DeltaEndpoint[] findNearestSegment(int x, int y, ICoordsMapping coordsMapping) {
        IXYDataset dataset = parent.getDataset();
        if (dataset == null) {
            return null;
        }

        double minDistSq = PICKING_THRESHOLD * PICKING_THRESHOLD;
        DeltaEndpoint[] closestSegment = null;

        for (int series : parent.getLegend().getEnabledItemIndices()) {
            LineProperties props = parent.getLineProperties(series);
            if (!props.getEffectiveDisplayLine()) {
                continue;
            }

            int n = dataset.getItemCount(series);
            if (n < 2) {
                continue; // Need at least two points for a segment
            }

            ILinePlotter linePlotter = props.getLinePlotter();

            // TODO: optimize, only check the points within +/- PICKING_THRESHOLD pixels
            // (but at least the ones immediately to the left and right of the pointer),
            // and one additional pair to the left and right.
            for (int i = 0; i < n - 1; ++i) {
                double dataX1 = dataset.getX(series, i);
                double dataY1 = dataset.getY(series, i);
                double dataX2 = dataset.getX(series, i + 1);
                double dataY2 = dataset.getY(series, i + 1);

                // Skip if either point is NaN
                if (Double.isNaN(dataY1) || Double.isNaN(dataY2)) {
                    continue;
                }

                // Get segment vertices in data coordinates using the centralized method
                double[] segmentDataPoints = linePlotter.getSegmentPoints(dataX1, dataY1, dataX2, dataY2);

                if (segmentDataPoints.length == 0) {
                    continue; // No line segments for this style or pair
                }

                // Iterate through the segments defined by the vertices
                for (int j = 0; j < segmentDataPoints.length - 2; j += 2) {
                    double segX1 = segmentDataPoints[j];
                    double segY1 = segmentDataPoints[j + 1];
                    double segX2 = segmentDataPoints[j + 2];
                    double segY2 = segmentDataPoints[j + 3];

                    // Convert segment endpoints to screen coordinates for distance check
                    double screenX1 = coordsMapping.toCanvasX(parent.transformX(segX1));
                    double screenY1 = coordsMapping.toCanvasY(parent.transformY(segY1));
                    double screenX2 = coordsMapping.toCanvasX(parent.transformX(segX2));
                    double screenY2 = coordsMapping.toCanvasY(parent.transformY(segY2));

                    // Calculate distance from mouse point to the screen segment
                    double distSq = distSqPointSegment(x, y, screenX1, screenY1, screenX2, screenY2);

                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        // Store the endpoints of this *specific* segment in data coordinates
                        // Note: We lose the original DataPoint association here, but that wasn't
                        // consistently used for interpolated segment endpoints anyway.
                        closestSegment = new DeltaEndpoint[] {
                            new DeltaEndpoint(null, segX1, segY1), // Use null for DataPoint as it might be interpolated
                            new DeltaEndpoint(null, segX2, segY2)
                        };
                    }
                }
            }
        }

        return closestSegment;
    }

    /**
     * Calculates the squared distance between two points.
     */
    private static double distSq(double x1, double y1, double x2, double y2) {
        double dx = x1 - x2;
        double dy = y1 - y2;
        return dx * dx + dy * dy;
    }

    /**
     * Calculates the squared distance from a point (px, py) to a line segment defined by (x1, y1) and (x2, y2).
     */
    private static double distSqPointSegment(double px, double py, double x1, double y1, double x2, double y2) {
        double l2 = distSq(x1, y1, x2, y2);
        if (l2 == 0.0) return distSq(px, py, x1, y1); // Segment is a point

        // Project point P onto the line containing segment AB
        // Consider the line extending the segment, parameterized as A + t (B - A)
        // The projection of P onto the line is at the value of t which minimizes the distance:
        // t = [(P-A) . (B-A)] / |B-A|^2
        double t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;

        // Clamp t to the range [0, 1] to ensure the projection lies on the segment
        t = Math.max(0, Math.min(1, t));

        // Coordinates of the projection point on the segment
        double projX = x1 + t * (x2 - x1);
        double projY = y1 + t * (y2 - y1);

        // Return the squared distance from the original point to the projection point
        return distSq(px, py, projX, projY);
    }

    /**
     * Clears the current measurement and removes all markers.
     */
    public void clearMeasurement() {
        endpointA = null;
        endpointB = null;
    }

    /**
     * Draws the delta measurement markers and information.
     *
     * @param graphics The graphics context
     * @param coordsMapping The coordinate mapping
     */
    public void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        if (endpointA == null && endpointB == null)
            return;

        graphics.pushState();

        Rectangle clipping = GraphicsUtils.getClip(graphics);
        org.eclipse.draw2d.geometry.Rectangle plotArea = parent.getPlotRectangle();
        graphics.setClip(clipping.intersect(new Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height)));

        if (endpointA != null)
            drawMarker(graphics, endpointA, coordsMapping, SWT.LINE_SOLID);

        if (endpointB != null)
            drawMarker(graphics, endpointB, coordsMapping, SWT.LINE_DOT);

        if (endpointA != null && endpointB != null)
            drawDeltaInfo(graphics, coordsMapping);
        else if (endpointA != null)
            drawEndpointInfo(endpointA, graphics, coordsMapping);
        else if (endpointB != null)
            drawEndpointInfo(endpointB, graphics, coordsMapping);

        graphics.popState();
    }

    /**
     * Draws a marker around a data point.
     */
    private void drawMarker(Graphics graphics, DeltaEndpoint coords, ICoordsMapping coordsMapping, int markerLineStyle) {
        if (coords == null) {
            return;
        }

        double x = parent.transformX(coords.x);
        double y = parent.transformY(coords.y);

        int canvasX = (int)coordsMapping.toCanvasX(x);
        int canvasY = (int)coordsMapping.toCanvasY(y);

        graphics.pushState();

        graphics.setForegroundColor(MARKER_COLOR);
        graphics.setLineWidth(2);
        graphics.setLineStyle(markerLineStyle);

        graphics.drawOval(canvasX - MARKER_RADIUS, canvasY - MARKER_RADIUS, MARKER_RADIUS * 2+1, MARKER_RADIUS * 2+1);

        graphics.popState();
    }

    /**
     * Draws the delta information.
     */
    private void drawDeltaInfo(Graphics graphics, ICoordsMapping coordsMapping) {
        double xA = endpointA.x;
        double yA = endpointA.y;
        double xB = endpointB.x;
        double yB = endpointB.y;

        double deltaX = xB - xA;
        double deltaY = yB - yA;

        // Format the delta information
        String deltaInfo;

        if (deltaX != 0 && deltaY == 0) {
            deltaInfo = String.format("X1: %.6g\nX2: %.6g\nΔX: %.6g\nY: %.6g", xA, xB, deltaX, yA);
        }
        else if (deltaX == 0 && deltaY != 0) {
            deltaInfo = String.format("X: %.6g\nY1: %.6g\nY2: %.6g\nΔY: %.6g", xA, yA, yB, deltaY);
        }
        else {
            deltaInfo = String.format("X1: %.6g, Y1: %.6g\nX2: %.6g, Y2: %.6g\nΔX: %.6g, ΔY: %.6g", xA, yA, xB, yB, deltaX, deltaY);
        }

        graphics.pushState();

        // Draw the delta information
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.setBackgroundColor(CrossHair.TOOLTIP_COLOR);

        Point textExtent = GraphicsUtils.getTextExtent(graphics, deltaInfo);

        double screenXA = coordsMapping.toCanvasX(xA);
        double screenXB = coordsMapping.toCanvasX(xB);
        double screenYA = coordsMapping.toCanvasY(yA);
        double screenYB = coordsMapping.toCanvasY(yB);

        double xDist = Math.abs(screenXA - screenXB);
        double yDist = Math.abs(screenYA - screenYB);

        // Calculate position for the delta information
        double xMid = (screenXA + screenXB) / 2;
        double yMid = (screenYA + screenYB) / 2;

        int textX = (int)xMid - textExtent.x / 2;
        int textY = (int)yMid - textExtent.y / 2;

        // if the text doesn't fit between the two markers, move it out to the top or to the right
        if ((textExtent.x + TEXT_MARGIN * 2 >= xDist - MARKER_RADIUS * 2)
                && (textExtent.y + TEXT_MARGIN * 2 >= yDist - MARKER_RADIUS * 2)) {
            // moving in the dimension where the distance between markers is smaller
            if (xDist > yDist)
               textY = (int)Math.min(screenYA, screenYB) - textExtent.y - TEXT_MARGIN * 2 - MARKER_RADIUS * 2;
            else
               textX = (int)Math.max(screenXA, screenXB) + + TEXT_MARGIN * 2 + MARKER_RADIUS * 2;
        }

        // Draw background and text
        graphics.fillRectangle(textX - TEXT_MARGIN, textY - TEXT_MARGIN, textExtent.x + TEXT_MARGIN * 2, textExtent.y + TEXT_MARGIN * 2);
        graphics.drawRectangle(textX - TEXT_MARGIN, textY - TEXT_MARGIN, textExtent.x + TEXT_MARGIN * 2, textExtent.y + TEXT_MARGIN * 2);
        graphics.drawText(deltaInfo, textX, textY);

        graphics.popState();
    }


    private void drawEndpointInfo(DeltaEndpoint endpoint, Graphics graphics, ICoordsMapping coordsMapping) {
        double xA = endpoint.x;
        double yA = endpoint.y;

        // Format the delta information
        String deltaInfo = String.format("X: %.6g\nY: %.6g", xA, yA);

        // Calculate position for the delta information
        double xMid = parent.transformX(xA);
        double yMid = parent.transformY(yA);

        int canvasMidX = (int)coordsMapping.toCanvasX(xMid);
        int canvasMidY = (int)coordsMapping.toCanvasY(yMid);

        graphics.pushState();

        // Draw the delta information
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.setBackgroundColor(CrossHair.TOOLTIP_COLOR);

        Point textExtent = GraphicsUtils.getTextExtent(graphics, deltaInfo);
        int textX = canvasMidX - textExtent.x / 2;
        int textY = canvasMidY - textExtent.y - 5;

        Rectangle plotArea = parent.getPlotRectangle();

        // Ensure the text is within the plot area
        if (textX < plotArea.x) {
            textX = plotArea.x + 5;
        }
        else if (textX + textExtent.x > plotArea.x + plotArea.width) {
            textX = plotArea.x + plotArea.width - textExtent.x - 5;
        }

        if (textY < plotArea.y) {
            textY = canvasMidY + 5;
        }

        // Draw background and text
        graphics.fillRectangle(textX - 2, textY - 2, textExtent.x + 4, textExtent.y + 4);
        graphics.drawRectangle(textX - 2, textY - 2, textExtent.x + 4, textExtent.y + 4);
        graphics.drawText(deltaInfo, textX, textY);

        graphics.popState();
    }
}
