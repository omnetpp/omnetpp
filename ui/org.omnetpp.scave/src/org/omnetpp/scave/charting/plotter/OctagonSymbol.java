/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws an octagon symbol.
 *
 * @author andras
 */
public class OctagonSymbol extends PolygonSymbol {
    public OctagonSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int d2 = sizeHint/2;
        int d4 = sizeHint/4;
        return new int[] {
            -d2, -d4,
            -d4, -d2,

            +d4, -d2,
            +d2, -d4,

            +d2, +d4,
            +d4, +d2,

            -d4, +d2,
            -d2, +d4,
        };
    }
}
