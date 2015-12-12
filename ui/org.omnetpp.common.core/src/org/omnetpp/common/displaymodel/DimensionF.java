/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.displaymodel;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Dimension;

/**
 * Dimension with float coordinates.
 *
 * @author andras
 */
public class DimensionF {
    public float width = Float.NaN;
    public float height = Float.NaN;

    public DimensionF(float width, float height) {
        this.width = width;
        this.height = height;
    }

    public Dimension toPixels(float scale) {
        return new Dimension(Float.isNaN(width) ? -1 : (int)(width*scale), Float.isNaN(height) ? -1 : (int)(height*scale));
    }

    public static DimensionF fromPixels(Dimension dim, float scale) {
        return dim == null ? null : new DimensionF(fromPixels(dim.width, scale), fromPixels(dim.height, scale));
    }

    private static float fromPixels(int size, float scale) {
        Assert.isTrue(size > 0 || size == -1); 
        return size == -1 ? Float.NaN : size / scale;
    }

    @Override
    public int hashCode() {
        return Float.floatToIntBits(width) * 31 + Float.floatToIntBits(height);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        DimensionF other = (DimensionF) obj;
        return eq(width, other.width) && eq(height, other.height);
    }

    private static boolean eq(float a, float b) {
        return Float.floatToIntBits(a) == Float.floatToIntBits(b); // this works for a=NaN, b=NaN too 
    }

}
