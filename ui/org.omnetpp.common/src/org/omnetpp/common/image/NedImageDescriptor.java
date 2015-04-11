/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.image;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.RGB;


/**
 * An ImageDescriptor used in NED editor. Supports coloring and scaling.
 *
 * @author rhornig
 */
public class NedImageDescriptor extends ImageDescriptor {

    /**
     * The class whose resource directory contain the file,
     * or <code>null</code> if none.
     */
    private final Class<?> location;

    /**
     * The name of the file.
     */
    private final String name;

    /**
     * The version of the image.
     *
     * <p>This makes possible to load different version of the same file,
     * see {@link ImageFactory.currentVersion}.
     */
    private final int version;

    // requested image properties
    private Scaling scaling = Scaling.NORMAL;
    private Padding padding = Padding.NULL;
    private RGB colorization = null;
    private int colorizationWeight = 0;

    /**
     * Creates a new file image descriptor.
     * The file has the given file name and is located
     * in the given class's resource directory. If the given
     * class is <code>null</code>, the file name must be absolute.
     * <p>
     * Note that the file is not accessed until its
     * <code>getImageData</code> method is called.
     *
     * @param clazz class for resource directory, or <code>null</code>
     * @param filename the name of the file or resource
     * @param version the version number of the resource
     */
    NedImageDescriptor(Class<?> clazz, String filename, int version) {
        Assert.isNotNull(filename);
        this.location = clazz;
        this.name = filename;
        this.version = version;
    }

    /**
     * Creates a new file image descriptor.
     * The file has the given file name and is located
     * in the given class's resource directory. If the given
     * class is <code>null</code>, the file name must be absolute.
     * <p>
     * Note that the file is not accessed until its
     * <code>getImageData</code> method is called.
     *
     * Scaling, padding, and colorization effects can be added by
     * specifying the corresponing parameters.
     *</p>
     *
     * @param clazz class for resource directory, or <code>null</code>
     * @param filename the name of the file or resource
     * @param version the version number of the resource
     * @param scaling the scaling to be applied or {@code null}
     * @param padding the padding to be applied or {@code null}
     * @param shade the color of the shading or {@code null}
     * @param weight the weight of the shading
     */
    NedImageDescriptor(Class<?> clazz, String filename, int version, Scaling scaling, Padding padding, RGB shade, int weight) {
        this(clazz, filename, version);
        this.scaling = scaling == null ? Scaling.NORMAL : scaling;
        this.padding = padding == null ? Padding.NULL : padding;
        this.colorization = shade;
        this.colorizationWeight = weight;
    }

    /* (non-Javadoc)
     * Method declared on Object.
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof NedImageDescriptor))
            return false;
        NedImageDescriptor other = (NedImageDescriptor) o;
        if (location != null) {
            if (!location.equals(other.location))
                return false;
        }
        else {
            if (other.location != null)
                return false;
        }
        if (colorization != null) {
            if (!colorization.equals(other.colorization))
                return false;
        }
        else {
            if (other.colorization != null)
                return false;
        }
        return name.equals(other.name)
                && version == other.version
                && scaling.equals(other.scaling)
                && padding.equals(other.padding)
                && colorizationWeight == other.colorizationWeight;
    }

    /* (non-Javadoc)
     * Method declared on ImageDesciptor.
     * Returns null if the image data cannot be read.
     */
    @Override
    public ImageData getImageData() {
        InputStream in = getStream();
        ImageData result = null;
        if (in != null) {
            try {
                result = new ImageData(in);
            } catch (SWTException e) {
                if (e.code != SWT.ERROR_INVALID_IMAGE)
                    throw e;
                // fall through otherwise
            } finally {
                try {
                    in.close();
                } catch (IOException e) {
                }
            }
        }
        // add colorization effect to the image data
        result = shadeImageData(result, colorization, colorizationWeight);
        result = rescaleImageData(result);
        return result;
    }

    /**
     * Checks whether the specified resource or file behind the descriptor exists
     * @return <code>true</code> if resource is present <code>false</code> otherwise
     */
    public boolean canCreate() {
        if (location != null) {
            return location.getResourceAsStream(name) != null;
        }
        else {
            // check if the file exists
            File file = new File(name);
            return file.exists() && file.isFile();
        }
    }

    /**
     * Returns a stream on the image contents.  Returns
     * null if a stream could not be opened.
     *
     * @return the buffered stream on the file or <code>null</code>
     * if the file cannot be found
     */
    private InputStream getStream() {
        InputStream is = null;

        if (location != null) {
            is = location.getResourceAsStream(name);

        }
        else {
            try {
                is = new FileInputStream(name);
            } catch (FileNotFoundException e) {
                return null;
            }
        }
        if (is == null)
            return null;
        else
            return new BufferedInputStream(is);
    }


    @Override
    public int hashCode() {
        int code = (37 * name.hashCode() + version);
        if (location != null) {
            code += location.hashCode();
        }
        if (colorization != null) {
            code += colorization.hashCode();
        }
        code += colorizationWeight * 10000 + scaling.hashCode();
        return code;
    }

    /**
     * The <code>NedImageDescriptor</code> implementation of this <code>Object</code> method
     * returns a string representation of this object which is suitable only for debugging.
     */
    @Override
    public String toString() {
        return "NedImageDescriptor(location=" + location + ", name=" + name +
            ", scaling=" + scaling + ", padding" + padding +
            ", colorize=" + colorization + ", colorWight=" + colorizationWeight + ")";
    }

    /**
     * Utility method to shade image data with a weighted color. Modifies the provided image data
     * and returns it for convenience
     * @param data The in-place converted image data
     * @param shade Shading color
     * @param weight The amount of shading 0 - 100
     */
    private static ImageData shadeImageData(ImageData data, RGB shade, int weight) {

        if (data == null || shade == null || weight <= 0)
            return data;

        int width = data.width;
        int height = data.height;
        PaletteData palette = data.palette;
        if (!palette.isDirect) {
            /* Convert the palette entries */
            RGB [] rgbs = palette.getRGBs();
            for (int i = 0; i < rgbs.length; i++) {
                if (data.transparentPixel != i) {
                    RGB color = rgbs [i];
                    int lum = (2126*color.red + 7152*color.green + 722*color.blue)/10000;
                    color.red = determineShading(color.red, shade.red, weight, lum);
                    color.green = determineShading(color.green, shade.green, weight, lum);
                    color.blue = determineShading(color.blue, shade.blue, weight, lum);
                }
            }
            data.palette = new PaletteData(rgbs);
        }
        else {
            /* Convert the pixels. */
            int[] scanline = new int[width];
            int redMask = palette.redMask;
            int greenMask = palette.greenMask;
            int blueMask = palette.blueMask;
            int redShift = palette.redShift;
            int greenShift = palette.greenShift;
            int blueShift = palette.blueShift;
            for (int y = 0; y < height; y++) {
                data.getPixels(0, y, width, scanline, 0);
                for (int x = 0; x < width; x++) {
                    int pixel = scanline[x];
                    int red = pixel & redMask;
                    red = redShift < 0 ? red >>> -redShift : red << redShift;
                    int green = pixel & greenMask;
                    green = greenShift < 0 ? green >>> -greenShift : green << greenShift;
                    int blue = pixel & blueMask;
                    blue = blueShift < 0 ? blue >>> -blueShift : blue << blueShift;

                    int lum = (2126*red + 7152*green + 722*blue)/10000;
                    red = determineShading(red, shade.red, weight, lum);
                    green = determineShading(green, shade.green, weight, lum);
                    blue = determineShading(blue, shade.blue, weight, lum);

                    red = redShift < 0 ? red << -redShift : red >> redShift;
                    red &= redMask;
                    green = greenShift < 0 ? green << -greenShift : green >> greenShift;
                    green &= greenMask;
                    blue = blueShift < 0 ? blue << -blueShift : blue >> blueShift;
                    blue &= blueMask;
                    scanline[x] = red | blue | green;
                }
                data.setPixels(0, y, width, scanline, 0);
            }
        }
        return data;
    }

    // calculate the colorization of a single point
    private static int determineShading(int origColor, int shadeColor, int weight, int lum) {
        int col = (int)((100-weight)*origColor + weight*lum*shadeColor/128.0)/100;
        return col<0 ? 0 : col>255 ? 255 : col;
    }

    /**
     * Utility method for rescaling the specified image.
     * If the size is not changed, and there is no padding, then the original
     * image is returned, otherwise a new ImageData is constructed.
     */
    private ImageData rescaleImageData(ImageData imgData) {
        Dimension size = scaling.computeImageSize(imgData);
        // do not change anything if no re-scale is needed and no padding requested
        if (size == null && padding.equals(Padding.NULL))
            return imgData;

        int newWidth = size == null ? imgData.width : size.width;
        int newHeight = size == null ? imgData.height : size.height;

        // image should not be smaller than the padding itself
        newWidth = Math.max(newWidth, padding.getWidth());
        newHeight = Math.max(newHeight, padding.getHeight());

        return ImageUtils.getResampledImageData(imgData, newWidth, newHeight,
                                    padding.left, padding.top, padding.right, padding.bottom);
    }

}
