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

    public LineSymbol(int size, boolean vertical) {
        super(size);
        if (vertical)
            rotate90(points);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        int d = sizeHint/2;
        return new int[] {
            0,  -d, 0,  d,
        };
    }
}
