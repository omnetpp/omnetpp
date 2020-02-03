/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a star symbol.
 *
 * @author andras
 */
public class StarSymbol extends PolygonSymbol {
    public StarSymbol(int size) {
        super(size);
    }

    @Override
    protected int[] getPoints(int sizeHint) {
        int[] pts = new int[] {
              0, -100, // p1
                59 / -2,   81 / -2, // s4
            -95,  -31, // p2
                95 / -2,  -31 / -2, // s5
            -59,   81, // p3
                0 / -2, -100 / -2, // s1
             59,   81, // p4
               -95 / -2,  -31 / -2, // s2
             95,  -31, // p5
              -59 / -2,   81 / -2, // s3
        };

        for (int i = 0; i < pts.length; ++i) {
            pts[i] *= sizeHint;
            pts[i] /= 100;
        }
        return pts;
    }
}
