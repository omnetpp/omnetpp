/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a triangle symbol.
 *
 * @author andras
 */
public class TriangleSymbol extends PolygonSymbol {

    public static class UpTriangleSymbol extends TriangleSymbol {
        public UpTriangleSymbol(int size) {
            super(size, 0);
        }
    }
    public static class LeftTriangleSymbol extends TriangleSymbol {
        public LeftTriangleSymbol(int size) {
            super(size, 1);
        }
    }
    public static class DownTriangleSymbol extends TriangleSymbol {
        public DownTriangleSymbol(int size) {
            super(size, 2);
        }
    }
    public static class RightTriangleSymbol extends TriangleSymbol {
        public RightTriangleSymbol(int size) {
            super(size, 3);
        }
    }

    private int rotationCount;

    private TriangleSymbol(int size, int rotationCount) {
        super(size);
        this.rotationCount = rotationCount;
        points = getPoints(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        // equal-sized triangle whose area is sizeHint^2
        int height = (132*sizeHint+50)/100; // 1.32 = sqrt4(3)
        int halfside = (76*sizeHint+50)/100; // 0.76 = 1 / sqrt4(3)
        int off = (84*sizeHint+50)/100;
        int[] points = new int[] {
                -halfside, (height-off),
                0, -off,
                halfside, (height-off)};

        for (int i = 0; i < rotationCount; ++i)
            rotate90(points);
        return points;
    }
}
