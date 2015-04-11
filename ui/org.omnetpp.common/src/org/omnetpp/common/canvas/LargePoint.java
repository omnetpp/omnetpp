/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.canvas;


/**
 * A variant of SWT's Point that stores coordinates in long.
 *
 * @author andras
 */
public final class LargePoint {
    public long x;
    public long y;

    /**
     * Construct a new instance of this class.
     */
    public LargePoint() {
    }

    /**
     * Construct a new instance of this class given the x, y values.
     */
    public LargePoint(long x, long y) {
        this.x = x;
        this.y = y;
    }

    /**
     * Construct a copy of the given point.
     */
    public LargePoint(LargePoint p) {
        this.x = p.x;
        this.y = p.y;
    }

    public void set(long x, long y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public int hashCode() {
        final int PRIME = 31;
        return (int) (x ^ (x >>> 32)) + PRIME * (int) (y ^ (y >>> 32));
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        final LargePoint other = (LargePoint) obj;
        return (x == other.x) && (y == other.y);
    }
}
