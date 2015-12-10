/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.displaymodel;

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
        return new Dimension(width == -1 ? -1 : (int)(width*scale), height == -1 ? -1 : (int)(height*scale)); //TODO use NaN -> -1 instead...
    }

    public static DimensionF fromPixels(Dimension dim, float scale) {
        return dim == null ? null : new DimensionF(dim.width / scale, dim.height / scale);
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
        return width == other.width && height == other.height;
    }

}
