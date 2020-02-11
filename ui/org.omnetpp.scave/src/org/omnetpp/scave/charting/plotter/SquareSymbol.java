/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a square symbol.
 *
 * @author andras
 */
public class SquareSymbol extends PolygonSymbol {
    public SquareSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int d = sizeHint * 141 / 100 / 2;
        return new int[] {
            -d, -d,
            -d, +d,
            +d, +d,
            +d, -d,
        };
    }
}
