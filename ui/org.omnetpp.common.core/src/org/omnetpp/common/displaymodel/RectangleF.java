/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.displaymodel;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Rectangle with float coordinates.
 *
 * @author andras
 */
public class RectangleF {
    public float x = Float.NaN;
    public float y = Float.NaN;
    public float width = Float.NaN;
    public float height = Float.NaN;

    public RectangleF(float x, float y, float width, float height) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    public PointF getLocation() {
        return new PointF(x, y);
    }

    public DimensionF getSize() {
        return new DimensionF(width, height);
    }

    public Rectangle toPixels(float scale) {
        Assert.isTrue(!Float.isNaN(x) && !Float.isNaN(y));
        Assert.isTrue(Float.isNaN(width) || width > 0);
        Assert.isTrue(Float.isNaN(height) || height > 0);
        return new Rectangle((int)(x*scale), (int)(y*scale), Float.isNaN(width) ? -1 : (int)(width*scale), Float.isNaN(height) ? -1 : (int)(height*scale));
    }

    public static RectangleF fromPixels(Rectangle rect, float scale) {
        return rect == null ? null : new RectangleF(rect.x / scale, rect.y / scale, fromPixels(rect.width, scale), fromPixels(rect.height, scale));
    }

    private static float fromPixels(int size, float scale) {
        Assert.isTrue(size > 0 || size == -1); 
        return size == -1 ? Float.NaN : size / scale;
    }

    @Override
    public int hashCode() {
        return 511 * Float.floatToIntBits(x) * 127 + Float.floatToIntBits(y) + Float.floatToIntBits(width) * 31 + Float.floatToIntBits(height);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        RectangleF other = (RectangleF) obj;
        return eq(x, other.x) && eq(y, other.y) && eq(width, other.width) && eq(height, other.height);
    }

    private static boolean eq(float a, float b) {
        return Float.floatToIntBits(a) == Float.floatToIntBits(b); // this works for a=NaN, b=NaN too 
    }

}
