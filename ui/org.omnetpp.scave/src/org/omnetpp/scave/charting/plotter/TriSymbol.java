/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a "tri" symbol.
 *
 * @author andras
 */
public class TriSymbol extends MultilineSymbol {

    public static class UpTriSymbol extends TriSymbol {
        public UpTriSymbol(int size) {
            super(size, 0);
        }
    }
    public static class LeftTriSymbol extends TriSymbol {
        public LeftTriSymbol(int size) {
            super(size, 1);
        }
    }
    public static class DownTriSymbol extends TriSymbol {
        public DownTriSymbol(int size) {
            super(size, 2);
        }
    }
    public static class RightTriSymbol extends TriSymbol {
        public RightTriSymbol(int size) {
            super(size, 3);
        }
    }

    private int rotationCount;

    private TriSymbol(int size, int rotationCount) {
        super(size);
        this.rotationCount = rotationCount;
        points = getLines(sizeHint);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        // equal-sized triangle whose area is sizeHint^2
        int height = (132*sizeHint+50)/100; // 1.32 = sqrt4(3)
        int halfside = (76*sizeHint+50)/100; // 0.76 = 1 / sqrt4(3)
        int off = (84*sizeHint+50)/100;
        int[] lines = new int[] {
            0, 0, -halfside, (height-off),
            0, 0, 0, -off,
            0, 0, halfside, (height-off)
        };
        for (int i = 0; i < rotationCount; ++i)
            rotate90(lines);
        return lines;
    }
}
