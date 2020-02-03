/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a "cross" symbol.
 *
 * @author andras
 */
public class CrossSymbol extends MultilineSymbol {
    public CrossSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        int size = (120*sizeHint+50)/100;
        size |= 1; // make it an odd number
        int d = size/2;
        return new int[] {
            -d, -d, +d, +d,
            -d, +d, +d, -d
        };
    }
}

