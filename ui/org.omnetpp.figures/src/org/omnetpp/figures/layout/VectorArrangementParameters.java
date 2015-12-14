/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;


/**
 * Contains the vector arrangement parameters, that is, the 3rd, 4th and 5th args
 * of the "p" tag in a submodule display string, in an organized form.
 *
 * See LAYOUT_PAR1, LAYOUT_PAR2, LAYOUT_PAR3 in IDisplayString.
 *
 * @author andras
 */
public class VectorArrangementParameters {
    public float x = Float.NaN, y = Float.NaN;  // coordinates for the "x" (exact) arrangement
    public float dx = Float.NaN, dy = Float.NaN; // spacing for the matrix, row and column layouts; radii for the ring layout
    public int n = -1; // number of icons per row in the matrix layout

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + Float.floatToIntBits(x);
        result = prime * result + Float.floatToIntBits(y);
        result = prime * result + Float.floatToIntBits(dx);
        result = prime * result + Float.floatToIntBits(dy);
        result = prime * result + n;
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        VectorArrangementParameters other = (VectorArrangementParameters) obj;
        if (Float.floatToIntBits(dx) != Float.floatToIntBits(other.dx))
            return false;
        if (Float.floatToIntBits(dy) != Float.floatToIntBits(other.dy))
            return false;
        if (n != other.n)
            return false;
        if (Float.floatToIntBits(x) != Float.floatToIntBits(other.x))
            return false;
        if (Float.floatToIntBits(y) != Float.floatToIntBits(other.y))
            return false;
        return true;
    }
}
