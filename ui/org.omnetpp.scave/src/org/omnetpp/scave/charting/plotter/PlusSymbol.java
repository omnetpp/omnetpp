/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a "plus" symbol.
 *
 * @author andras
 */
public class PlusSymbol extends MultilineSymbol {

    public PlusSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        int size = (134*sizeHint+50)/100; // 3/sqrt(5) ~ 1.34
        size |= 1; // make it an odd number
        int d = size/2;
        return new int[] {
            -d,  0, +d,  0,
             0, -d,  0, +d
        };
    }
}
