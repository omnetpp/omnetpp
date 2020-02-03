/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a pentagon symbol.
 *
 * @author andras
 */
public class PentagonSymbol extends PolygonSymbol {
    public PentagonSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int[] pts = new int[] {
              0, -100,
            -95,  -31,
            -59,   81,
             59,   81,
             95,  -31,
        };

        for (int i = 0; i < pts.length; ++i) {
            pts[i] *= sizeHint;
            pts[i] /= 100;
        }
        return pts;
    }
}
