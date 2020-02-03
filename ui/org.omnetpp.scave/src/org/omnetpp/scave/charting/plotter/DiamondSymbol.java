/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a diamond symbol.
 *
 * @author andras
 */
public class DiamondSymbol extends PolygonSymbol {

    public DiamondSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int d = (sizeHint*71+50)/100; // make same area as square; 1.41=sqrt(2) ; /2 = 0.71
        return new int[] {
                -d,  0,
                 0, -d,
                 d,  0,
                 0,  d};
    }
}
