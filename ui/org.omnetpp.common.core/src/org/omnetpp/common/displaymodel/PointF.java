/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.displaymodel;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Point;


/**
 * Point with float coordinates.
 *
 * @author andras
 */
public class PointF {
    public float x = Float.NaN;
    public float y = Float.NaN;

    public PointF(float x, float y) {
        this.x = x;
        this.y = y;
    }

    public static PointF fromPixels(Point p, float scale) {
        return p == null ? null : new PointF(p.x / scale, p.y / scale);
    }

    public Point toPixels(float scale) {
        Assert.isTrue(!Float.isNaN(x) && !Float.isNaN(y));
        return new Point((int)(x*scale), (int)(y*scale));
    }

    @Override
    public int hashCode() {
        return Float.floatToIntBits(x) * 31 + Float.floatToIntBits(y);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        PointF other = (PointF) obj;
        return eq(x, other.x) && eq(y, other.y);
    }

    private static boolean eq(float a, float b) {
        return Float.floatToIntBits(a) == Float.floatToIntBits(b); // this works for a=NaN, b=NaN too 
    }

}
