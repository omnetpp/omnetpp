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
        if (dim == null)
            return null;
        DimensionF result = new DimensionF(fromPixels(dim.width, scale), fromPixels(dim.height, scale));
        result.adjustForZoom(scale);
        return result;
    }

    private static float fromPixels(int size, float scale) {
        Assert.isTrue(size > 0 || size == -1);
        return size == -1 ? Float.NaN : size / scale;
    }

    /**
     * Rounds the size based on the specified zoom level.
     * The rounding is done to ensure that the dimensions have no more digits than makes sense
     * considering the granularity allowed by the zoom level.
     *
     * E.g. for a zoom of 1x..9x, round to integers; for a zoom of 10x..99x, round to 1 decimal place, etc.
     *
     * @param scave the zoom level
     */
    public void adjustForZoom(float scale) {
        float pow10 = 1.0f;
        while (pow10 < scale)
            pow10 *= 10.0;
        width = roundFloat(width, pow10);
        height = roundFloat(height, pow10);
    }

    private static float roundFloat(float x, float pow10) {
        return Math.round(x * pow10) / pow10;
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
