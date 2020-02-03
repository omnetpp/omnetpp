/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a thin diamond symbol.
 *
 * @author andras
 */
public class ThinDiamondSymbol extends PolygonSymbol {

    public ThinDiamondSymbol(int size) {
        // matplotlib only supports "vertical" thin diamond, with no rotation
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int dy = (sizeHint*71+50)/100; // make same area as square; 1.41=sqrt(2) ; /2 = 0.71
        int dx = sizeHint/2;
        return new int[] {
                -dx,   0,
                  0, -dy,
                 dx,   0,
                  0,  dy};
    }
}
