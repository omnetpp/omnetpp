/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

/**
 * Draws a "tri" symbol.
 *
 * @author andras
 */
public class TriSymbol extends MultilineSymbol {

    public TriSymbol(int size, int rotationCount) {
        super(size);
        for (int i = 0; i < rotationCount; ++i)
            rotate90(points);
    }

    @Override
    protected int[] getLines(int sizeHint) {
        // equal-sized triangle whose area is sizeHint^2
        int height = (132*sizeHint+50)/100; // 1.32 = sqrt4(3)
        int halfside = (76*sizeHint+50)/100; // 0.76 = 1 / sqrt4(3)
        int off = (84*sizeHint+50)/100;
        return new int[] {
            0, 0, -halfside, (height-off),
            0, 0, 0, -off,
            0, 0, halfside, (height-off)
        };
    }

}
