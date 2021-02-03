/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a "line" symbol.
 *
 * @author andras
 */
public class LineSymbol extends MultilineSymbol {

    public static class HorizontalLineSymbol extends LineSymbol {
        public HorizontalLineSymbol(int size) {
            super(size, false);
        }
    }

    public static class VerticalLineSymbol extends LineSymbol {
        public VerticalLineSymbol(int size) {
            super(size, true);
        }
    }

    private boolean vertical;

    private LineSymbol(int size, boolean vertical) {
        super(size);
        this.vertical = vertical;
        points = getLines(sizeHint);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        int d = sizeHint/2;
        int[] lines = new int[] {
            -d, 0, d, 0
        };
        if (vertical)
            rotate90(lines);
        return lines;
    }
}
