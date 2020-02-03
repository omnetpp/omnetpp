package org.omnetpp.common.canvas;

import java.util.Vector;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;

/**
 * This utility class provides 'long' coordinate support for graphics. Despite the fact that SWT graphics takes 'int' coordinates
 * it seems to have a limit around 2 million on Windows platforms. Besides we also want to be able to draw arbitrary stuff in the
 * 'long' coordinate range. Unfortunately simply casting the coordinates/sizes to 'int' is not right, because it just throws away
 * the upper bits and the result might accidentally fall into the clipping region.
 *
 * The idea is to clip all necessary drawing operations to the coordinate range that SWT clearly supports before calling the
 * underlying graphics.
 *
 * @author levy
 */
public class LargeGraphics {
    /**
     * This must be a coordinate that SWT can always handle safely on any platform.
     * Drawing operations outside this coordinate will not be propagated to the underlying graphics.
     */
    private static long MIN_COORDINATE = -10000;  // 10000 because of Cairo

    /**
     * This must be a coordinate that SWT can always handle safely on any platform.
     * Drawing operations outside this coordinate will not be propagated to the underlying graphics.
     */
    private static long MAX_COORDINATE = 10000;

    private static long MAX_SIZE = MAX_COORDINATE - MIN_COORDINATE;

    private final static boolean validCoordinate(long value) {
        return MIN_COORDINATE <= value && value <= MAX_COORDINATE;
    }

    private final static boolean validSize(long value) {
        return value <= MAX_SIZE;
    }

    private final static int toIntCoordinate(long value) {
        Assert.isTrue(validCoordinate(value));
        return (int)value;
    }

    private static int[] toIntCoordinates(long[] longValues) {
        int[] intValues = new int[longValues.length];
        for (int i = 0; i < longValues.length; i++)
            intValues[i] = toIntCoordinate(longValues[i]);
        return intValues;
    }

    private final static int toIntSize(long value) {
        Assert.isTrue(validSize(value));
        return (int)value;
    }

    public static void drawPoint(Graphics graphics, long x, long y) {
        // just casting to 'int' and passing the result in would be wrong, resulting in random points popping up in the clipping region
        if (validCoordinate(x) && validCoordinate(y))
            graphics.drawPoint(toIntCoordinate(x), toIntCoordinate(y));
    }

    private static long[] buffer = new long[4]; // to eliminate allocations in potentially performance-critical clipping methods (consider plotting of output vectors in Scave)

    public static void drawLine(Graphics graphics, long x1, long y1, long x2, long y2) {
        // calculate the intersection with supported coordinate range, there's no need to get the actual clipping
        long[] points = CohenSutherland.clipLine(x1, y1, x2, y2, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE, buffer);
        if (points != null)
            graphics.drawLine(toIntCoordinate(points[0]), toIntCoordinate(points[1]), toIntCoordinate(points[2]), toIntCoordinate(points[3]));
    }

    // only checking if the first point is within the technical bounds,
    // without clipping the actual line
    public static void drawSmallLine(Graphics graphics, long x1, long y1, long x2, long y2) {
        if (validCoordinate(x1) && validCoordinate(y1))
            graphics.drawLine(toIntCoordinate(x1), toIntCoordinate(y1), toIntCoordinate(x2), toIntCoordinate(y2));
    }

    public static void drawRectangle(Graphics graphics, long x, long y, long width, long height) {
        long[] points = clipRectangle(x, y, width, height, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE, buffer);
        if (points != null)
            graphics.drawRectangle(toIntCoordinate(points[0]), toIntCoordinate(points[1]), toIntSize(points[2]), toIntSize(points[3]));
    }

    public static void fillRectangle(Graphics graphics, long x, long y, long width, long height) {
        long[] points = clipRectangle(x, y, width, height, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE, buffer);
        if (points != null)
            graphics.fillRectangle(toIntCoordinate(points[0]), toIntCoordinate(points[1]), toIntSize(points[2]), toIntSize(points[3]));
    }

    public static void drawOval(Graphics graphics, long x, long y, long width, long height) {
        long[] points = clipRectangle(x, y, width, height, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE, buffer);
        if (points != null)
            // KLUDGE: draw with the original coordinates and cross our fingers and hope that
            // nobody wants to draw big ovals hanging into the clipping area from far away
            graphics.drawOval(toIntCoordinate(x), toIntCoordinate(y), toIntCoordinate(width), toIntCoordinate(height));
    }

    public static void fillOval(Graphics graphics, long x, long y, long width, long height) {
        long[] points = clipRectangle(x, y, width, height, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE, buffer);
        if (points != null)
            // KLUDGE: draw with the original coordinates and cross our fingers and hope that
            // nobody wants to draw big ovals hanging into the clipping area from far away
            graphics.fillOval(toIntCoordinate(x), toIntCoordinate(y), toIntCoordinate(width), toIntCoordinate(height));
    }

    public static void drawPolyline(Graphics graphics, long points[]) {
        //System.out.println("Before: " + Arrays.toString(points));
        points = SutherlandHodgman.clipPolygon(points, false, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE);
        //System.out.println("After: " + Arrays.toString(points));
        if (points != null)
            graphics.drawPolyline(toIntCoordinates(points));
    }

    public static void fillPolygon(Graphics graphics, long[] points) {
        points = SutherlandHodgman.clipPolygon(points, true, MIN_COORDINATE, MIN_COORDINATE, MAX_COORDINATE, MAX_COORDINATE);
        if (points != null)
            graphics.fillPolygon(toIntCoordinates(points));
    }

    // only checking if the first point is within the technical bounds,
    // without clipping the actual shape
    public static void fillSmallPolygon(Graphics graphics, long[] points) {
        if (validCoordinate(points[0]) && validCoordinate(points[1]))
            graphics.fillPolygon(toIntCoordinates(points));
    }

    public static void drawText(Graphics graphics, String text, long x, long y) {
        // KLUDGE: we are ignoring the size of the text here, because it is very unlikely that
        // the text would be so big that it would flow into the clipping region which is much
        // smaller than the valid coordinate range
        if (validCoordinate(x) && validCoordinate(y))
            graphics.drawText(text, toIntCoordinate(x), toIntCoordinate(y));
    }

    /**
     * Copied from http://en.wikipedia.org/wiki/Cohen�Sutherland and edited somewhat.
     *
     * @author levy
     */
    private static class CohenSutherland
    {
        private static int REGION_LEFT = 1;
        private static int REGION_RIGHT = 2;
        private static int REGION_BOTTOM = 4;
        private static int REGION_TOP = 8;

        private final static int regionCode(long x, long y, long xmin, long ymin, long xmax, long ymax) {
            int code = 0;
            if (y > ymax)
                code |= REGION_TOP;
            else if (y < ymin)
                code |= REGION_BOTTOM;
            if (x > xmax)
                code |= REGION_RIGHT;
            else if (x < xmin)
                code |= REGION_LEFT;
            return code;
        }

        public final static long[] clipLine(long x1, long y1, long x2, long y2, long xmin, long ymin, long xmax, long ymax, long[] buffer) {
            // regions for P0, P1, and whatever point lies outside the clip rectangle
            int region0, region1, regionOut, count = 0;
            region0 = regionCode(x1, y1, xmin, ymin, xmax, ymax);
            region1 = regionCode(x2, y2, xmin, ymin, xmax, ymax);
            do {
                if ((region0 | region1) == 0) {
                    buffer[0] = x1; buffer[1] = y1; buffer[2] = x2; buffer[3] = y2;
                    return buffer;
                }
                else if ((region0 & region1) > 0)
                    return null;
                else {
                    // Failed both tests, so calculate the line segment to clip
                    // from an outside point to an intersection with clip edge
                    long x = 0, y = 0;
                    // At least one endpoint is outside the clip rectangle; pick it.
                    regionOut = region0 != 0 ? region0: region1;
                    // Now find the intersection point;
                    // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
                    if ((regionOut & REGION_TOP) > 0) {
                        x = x1 + (long)((double)(x2 - x1) * (ymax - y1) / (y2 - y1));
                        y = ymax;
                    }
                    else if ((regionOut & REGION_BOTTOM) > 0) {
                        x = x1 + (long)((double)(x2 - x1) * (ymin - y1) / (y2 - y1));
                        y = ymin;
                    }
                    else if ((regionOut & REGION_RIGHT)> 0) {
                        y = y1 + (long)((double)(y2 - y1) * (xmax - x1) / (x2 - x1));
                        x = xmax;
                    }
                    else if ((regionOut & REGION_LEFT) > 0) {
                        y = y1 + (long)((double)(y2 - y1) * (xmin - x1) / (x2 - x1));
                        x = xmin;
                    }
                    // Now we move outside point to intersection point to clip
                    // and get ready for next pass.
                    if (regionOut == region0) {
                        x1 = x;
                        y1 = y;
                        region0 = regionCode(x1, y1, xmin, ymin, xmax, ymax);
                    }
                    else {
                        x2 = x;
                        y2 = y;
                        region1 = regionCode(x2, y2, xmin, ymin, xmax, ymax);
                    }
                }
                count++;
            }
            // KLUDGE: this condition is the same as the one on wikipedia, but WTF is it??? I must admit I don't have the faintest idea!
            // I would like to think that count could not be more than a few...
            while (count < 5000);
            return null;
        }
    }

    private final static long[] clipRectangle(long x, long y, long width, long height, long xmin, long ymin, long xmax, long ymax, long[] buffer) {
        long x1 = Math.max(x, xmin);
        long x2 = Math.min(x + width, xmax);
        long y1 = Math.max(y, ymin);
        long y2 = Math.min(y + height, ymax);
        if (((x2 - x1) < 0) || ((y2 - y1) < 0))
            return null;
        else {
            buffer[0] = x1; buffer[1] = y1; buffer[2] = x2 - x1; buffer[3] = y2 - y1;
            return buffer;
        }
    }

    /**
     * Copied from http://en.wikipedia.org/wiki/Sutherland-Hodgman and edited somewhat.
     *
     * @author levy
     */
    private static class SutherlandHodgman {
        public final static byte TOP_EDGE = 0;
        public final static byte RIGHT_EDGE = 1;
        public final static byte BOTTOM_EDGE = 2;
        public final static byte LEFT_EDGE = 3;

        private final static boolean isInsideClipHalfPane(LargePoint p, byte EdgeID, long xmin, long ymin, long xmax, long ymax)
        {
            if (EdgeID == TOP_EDGE)
                return (p.y >= ymin);
            else if (EdgeID == BOTTOM_EDGE)
                return (p.y <= ymax);
            else if (EdgeID == LEFT_EDGE)
                return (p.x >= xmin);
            else if (EdgeID == RIGHT_EDGE)
                return (p.x <= xmax);
            else
                return false;
        }

        /**
         * Calculate the intersection with the given edge of the window
         */
        private final static LargePoint intersectionPoint(byte EdgeID, LargePoint p1, LargePoint p2, long xmin, long ymin, long xmax, long ymax)
        {
            if (EdgeID == TOP_EDGE)
                return new LargePoint(p1.x + (ymin - p1.y) * (p2.x - p1.x) / (p2.y - p1.y), ymin);
            else if (EdgeID == BOTTOM_EDGE)
                return new LargePoint(p1.x + (ymax - p1.y) * (p2.x - p1.x) / (p2.y - p1.y), ymax);
            else if (EdgeID == RIGHT_EDGE)
                return new LargePoint(xmax, p1.y + (xmax - p1.x) * (p2.y - p1.y) / (p2.x - p1.x));
            else if (EdgeID == LEFT_EDGE)
                return new LargePoint(xmin, p1.y + (xmin - p1.x) * (p2.y - p1.y) / (p2.x - p1.x));
            else
                return null;
        }

        /**
         * Clip the polygon against the rectangle
         */
        @SuppressWarnings("unchecked")
        private final static long[] clipPolygon(long[] points, boolean closed, long xmin, long ymin, long xmax, long ymax) {
            LargePoint currentPoint;
            LargePoint nextPoint;
            // make a copy of original polygon to work with
            Vector<LargePoint> clippedPolygon = new Vector<LargePoint>();
            Vector<LargePoint> polygon = new Vector<LargePoint>(points.length / 2);
            for (int i = 0; i < points.length / 2; i++)
                polygon.add(new LargePoint(points[2 * i], points[2 * i + 1]));
            if (closed && !polygon.firstElement().equals(polygon.lastElement()))
                polygon.add(polygon.firstElement());
            // loop through all for clipping edges
            for (byte edgeID = 0; edgeID < 4; edgeID++) {
                clippedPolygon.clear();
                for (int i = 0; i < polygon.size() - 1; i++) {
                    currentPoint = polygon.get(i);
                    nextPoint = polygon.get(i+1);
                    if (isInsideClipHalfPane(currentPoint, edgeID, xmin, ymin, xmax, ymax)) {
                        if (isInsideClipHalfPane(nextPoint, edgeID, xmin, ymin, xmax, ymax))
                            // here both points are inside the clipping window so add the second one
                            clippedPolygon.add(new LargePoint(nextPoint.x, nextPoint.y));
                        else
                            // the following point is outside so add the intersection point
                            clippedPolygon.add(intersectionPoint(edgeID, currentPoint, nextPoint, xmin, ymin, xmax, ymax));
                    }
                    else {
                        // so first point is outside the window here
                        if (isInsideClipHalfPane(nextPoint, edgeID, xmin, ymin, xmax, ymax)) {
                            // the following point is inside so add the insection point and also p2
                            clippedPolygon.add(intersectionPoint(edgeID, currentPoint, nextPoint, xmin, ymin, xmax, ymax));
                            clippedPolygon.add(new LargePoint(nextPoint.x, nextPoint.y));
                        }
                    }
                }
                // make sure that first and last element are the same, we want a closed polygon
                // KLUDGE !clippedPolygon.isEmpty() added to preven NoSuchElementException (is this right?)
                if (closed && !clippedPolygon.isEmpty() && clippedPolygon.firstElement() != clippedPolygon.lastElement())
                    clippedPolygon.add(clippedPolygon.firstElement());
                // we have to keep on working with our new clipped polygon
                polygon = (Vector<LargePoint>)clippedPolygon.clone();
            }
            // convert back to a long array
            if (polygon.size() == 0)
                return null;
            else {
                points = new long[polygon.size() * 2];
                for (int i = 0; i < polygon.size(); i++) {
                    LargePoint point = polygon.get(i);
                    points[2 * i] = point.x;
                    points[2 * i + 1] = point.y;
                }
                return points;
            }
        }
    }
}
