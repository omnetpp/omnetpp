package org.omnetpp.common.image;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.swt.graphics.ImageData;

/**
 * Immutable class representing the scaling of an image.
 *
 * <p>Horizontal and vertical scaling can be:
 * <ul>
 * <li> n > 0 : the absolute width/height
 * <li> n < 0 : the negate of the relative width/height in percentage
 * <li> n == 0: the width/height is computed from the scaling of the other dimension keeping the aspect ratio
 * </ul>
 *
 * <p>Scaling has the following string representation:
 * <ul>
 * <li> one of the predefined scalings: "vs","s","l","vl"
 * <li> one number (e.g. "16", "-120"): specifies only the width, aspect ratio is kept
 * <li> two numbers separated by an 'x' (e.g. "16x-120"): specifies width and height scaling;
 *                                                        0 value can be omitted (e.g. "16x", "x24")
 * </ul>
 *
 * @author tomi
 */
public class Scaling {

    // predefined scalings
    public static final Scaling NORMAL = new Scaling(-100, -100);
    public static final Scaling VS = new Scaling(-40, -40);
    public static final Scaling S = new Scaling(-60, -60);
    public static final Scaling L = new Scaling(-150, -150);
    public static final Scaling VL = new Scaling(-250, -250);

    // at most one of them can be 0
    public final int width;
    public final int height;

    public Scaling(int width, int height) {
        if (width == 0 && height == 0 ||
            width == 0 && height == -100 ||
            width == -100 && height == 0) {
            this.width = -100;
            this.height = -100;
        }
        else {
            this.width = width;
            this.height = height;
        }
    }

    /**
     * Returns the new size of the image, or null if its size is not changed
     * by applying this scaling.
     */
    public Dimension computeImageSize(ImageData imgData) {
        if (imgData.width == 0 || imgData.height == 0)
            return null;

        if ((width == 0 || width == -100 || width == imgData.width) &&
            (height == 0 || height == -100 || height == imgData.height))
            return null;

        double widthRatio = width > 0 ? (double)width / imgData.width : // absolute width
                                        -(double)width / 100;           // percentage
        double heightRatio = height > 0 ? (double)height / imgData.height : // absolute height
                                          -(double)height / 100;        // percentage
        if (width == 0)
            widthRatio = heightRatio;
        else if (height == 0)
            heightRatio = widthRatio;

        return new Dimension((int)(imgData.width*widthRatio),
                             (int)(imgData.height*heightRatio));
    }

    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (!(o instanceof Scaling))
            return false;

        Scaling other = (Scaling)o;
        return width == other.width && height == other.height;
    }

    public int hashCode() {
        return width + height;
    }

    public String toString() {
        return "Scaling("+width+","+height+")";
    }

    // translate the textual size to pixel
    public static Scaling parse(String imageScaling) {
        if (imageScaling == null || "".equals(imageScaling) ) {
            return null;
        }
        else if ("vs".equals(imageScaling.toLowerCase())) {
            return VS;
        }
        else if ("s".equals(imageScaling.toLowerCase())) {
            return S;
        }
        else if ("l".equals(imageScaling.toLowerCase())) {
            return L;
        }
        else if ("vl".equals(imageScaling.toLowerCase())) {
            return VL;
        }
        else {
            try {
                int i = imageScaling.indexOf('x');
                if (i >= 0) {
                    String widthStr = imageScaling.substring(0, i);
                    String heightStr = imageScaling.substring(i+1);
                    int width = widthStr.length() > 0 ? Integer.parseInt(widthStr) : 0;
                    int height = heightStr.length() > 0 ? Integer.parseInt(heightStr) : 0;
                    return new Scaling(width, height);
                }
                else {
                    return new Scaling(Integer.valueOf(imageScaling).intValue(), 0);
                }
            } catch (NumberFormatException e) {}
        }

        return null;
    }

    String getFilenameSuffix() {
        if (this.equals(VS))
            return "_vs";
        else if (this.equals(S))
            return "_s";
        else if (this.equals(L))
            return "_l";
        else if (this.equals(VL))
            return "_vl";
        return "";
    }

    static String findFilenameSuffix(String imageId) {
        if (imageId.endsWith("_s") || imageId.endsWith("_l"))
            return imageId.substring(imageId.length()-1);
        if (imageId.endsWith("_vs") || imageId.endsWith("_vl"))
            return imageId.substring(imageId.length()-2);
        return "";
    }
}
