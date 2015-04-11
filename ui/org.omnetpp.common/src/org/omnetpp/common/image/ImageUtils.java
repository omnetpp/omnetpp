/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.image;

import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.IndexColorModel;
import java.awt.image.WritableRaster;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Shell;

/**
 * Convert between AWT and SWT image, and supports rescaling
 * @author andras
 */
public class ImageUtils {

    public static BufferedImage convertToAWT(Image swtImage) {
        return convertToAWT(swtImage.getImageData());
    }

    public static Image convertToSWT(Device device, BufferedImage awtImage) {
        return new Image(device, convertToSWT(awtImage));
    }

    public static Image getResizedImage(Device device, Image swtImage, int width, int height) {
        BufferedImage awtImage = convertToAWT(swtImage);
        BufferedImage scaledAwtImage = getResampledAWTImage(awtImage, width, height);
        return convertToSWT(device, scaledAwtImage);
    }

    public static BufferedImage convertToAWT(ImageData data) {
        ColorModel colorModel = null;
        PaletteData palette = data.palette;
        if (palette.isDirect) {
            colorModel = new DirectColorModel(data.depth, palette.redMask, palette.greenMask, palette.blueMask);
            BufferedImage bufferedImage = new BufferedImage(colorModel, colorModel.createCompatibleWritableRaster(data.width, data.height), false, null);
            WritableRaster raster = bufferedImage.getRaster();
            int[] pixelArray = new int[3];
            for (int y = 0; y < data.height; y++) {
                for (int x = 0; x < data.width; x++) {
                    int pixel = data.getPixel(x, y);
                    RGB rgb = palette.getRGB(pixel);
                    pixelArray[0] = rgb.red;
                    pixelArray[1] = rgb.green;
                    pixelArray[2] = rgb.blue;
                    raster.setPixels(x, y, 1, 1, pixelArray);
                }
            }
            return bufferedImage;
        }
        else {
            RGB[] rgbs = palette.getRGBs();
            byte[] red = new byte[rgbs.length];
            byte[] green = new byte[rgbs.length];
            byte[] blue = new byte[rgbs.length];
            for (int i = 0; i < rgbs.length; i++) {
                RGB rgb = rgbs[i];
                red[i] = (byte)rgb.red;
                green[i] = (byte)rgb.green;
                blue[i] = (byte)rgb.blue;
            }
            if (data.transparentPixel != -1) {
                colorModel = new IndexColorModel(data.depth, rgbs.length, red, green, blue, data.transparentPixel);
            }
            else {
                colorModel = new IndexColorModel(data.depth, rgbs.length, red, green, blue);
            }
            BufferedImage bufferedImage = new BufferedImage(colorModel, colorModel.createCompatibleWritableRaster(data.width, data.height), false, null);
            WritableRaster raster = bufferedImage.getRaster();
            int[] pixelArray = new int[1];
            for (int y = 0; y < data.height; y++) {
                for (int x = 0; x < data.width; x++) {
                    int pixel = data.getPixel(x, y);
                    pixelArray[0] = pixel;
                    raster.setPixel(x, y, pixelArray);
                }
            }
            return bufferedImage;
        }
    }

    public static ImageData convertToSWT(BufferedImage bufferedImage) {
        if (bufferedImage.getColorModel() instanceof DirectColorModel) {
            DirectColorModel colorModel = (DirectColorModel)bufferedImage.getColorModel();
            PaletteData palette = new PaletteData(colorModel.getRedMask(), colorModel.getGreenMask(), colorModel.getBlueMask());
            ImageData data = new ImageData(bufferedImage.getWidth(), bufferedImage.getHeight(), colorModel.getPixelSize(), palette);
            WritableRaster raster = bufferedImage.getRaster();
            int[] pixelArray = new int[3];
            for (int y = 0; y < data.height; y++) {
                for (int x = 0; x < data.width; x++) {
                    raster.getPixel(x, y, pixelArray);
                    int pixel = palette.getPixel(new RGB(pixelArray[0], pixelArray[1], pixelArray[2]));
                    data.setPixel(x, y, pixel);
                }
            }
            return data;
        }
        else if (bufferedImage.getColorModel() instanceof ComponentColorModel) {
            Assert.isTrue(false, "converting from this color model not implemented");
            //XXX does not work, to be debugged
            ComponentColorModel colorModel = (ComponentColorModel)bufferedImage.getColorModel();
            int depth = colorModel.getPixelSize();
            PaletteData palette = new PaletteData(0xFF00, 0xFF0000, 0xFF000000);
            ImageData data = new ImageData(bufferedImage.getWidth(), bufferedImage.getHeight(), depth, palette);
            WritableRaster raster = bufferedImage.getRaster();
            int[] pixelArray = new int[4];
            for (int y = 0; y < data.height; y++) {
                for (int x = 0; x < data.width; x++) {
                    raster.getPixel(x, y, pixelArray);
                    int pixel = (pixelArray[3] << 24) | (pixelArray[2] << 16) | (pixelArray[1] << 8);
                    data.setPixel(x, y, pixel);
                    data.setAlpha(x, y, pixelArray[0]);
                }
            }
            return data;
        }
        else if (bufferedImage.getColorModel() instanceof IndexColorModel) {
            IndexColorModel colorModel = (IndexColorModel)bufferedImage.getColorModel();
            int size = colorModel.getMapSize();
            byte[] reds = new byte[size];
            byte[] greens = new byte[size];
            byte[] blues = new byte[size];
            colorModel.getReds(reds);
            colorModel.getGreens(greens);
            colorModel.getBlues(blues);
            RGB[] rgbs = new RGB[size];
            for (int i = 0; i < rgbs.length; i++) {
                rgbs[i] = new RGB(reds[i] & 0xFF, greens[i] & 0xFF, blues[i] & 0xFF);
            }
            PaletteData palette = new PaletteData(rgbs);
            ImageData data = new ImageData(bufferedImage.getWidth(), bufferedImage.getHeight(), colorModel.getPixelSize(), palette);
            data.transparentPixel = colorModel.getTransparentPixel();
            WritableRaster raster = bufferedImage.getRaster();
            int[] pixelArray = new int[1];
            for (int y = 0; y < data.height; y++) {
                for (int x = 0; x < data.width; x++) {
                    raster.getPixel(x, y, pixelArray);
                    data.setPixel(x, y, pixelArray[0]);
                }
            }
            return data;
        }
        else {
            throw new IllegalArgumentException("unhandled AWT color model: "+bufferedImage.getColorModel().getClass());
        }
    }

    public static BufferedImage getResampledAWTImage(BufferedImage img, int width, int height) {
        BufferedImage resultImage = new BufferedImage(width, height, BufferedImage.TYPE_4BYTE_ABGR);

        Graphics2D g2d = resultImage.createGraphics();
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
        //g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);

        // fill with transparent color
        g2d.setColor(new java.awt.Color(255, 255, 255, 0));
        g2d.fillRect(0, 0, width, height);

        // draw image, preserving aspect ratio
        double scalex = (double) width / (double) img.getWidth(null);
        double scaley = (double) height / (double) img.getHeight(null);

        if (scalex < scaley) {
            g2d.drawImage(img, 0, (int)((height - img.getHeight(null) * scalex) / 2.0), width, (int) (img.getHeight(null) * scalex),  null);
        }
        else {
            g2d.drawImage(img, (int)((width - img.getWidth(null) * scaley) / 2.0), 0, (int) (img.getWidth(null) * scaley), height, null);
        }

        g2d.dispose();

        return resultImage;
    }

    /**
     * Invokes getResampledImage() with zero insets.
     *
     * @author Andras
     */
    public static Image getResampledImage(Image image, int width, int height) {
        return getResampledImage(image, width, height, 0, 0, 0, 0);
    }

    /**
     * Resizes an image to the given size, preserving aspect ratio, and leaving
     * some margin (insets). Performs high-quality resampling, and preserves
     * transparency (alpha) information.
     *
     * @author Andras
     */
    public static Image getResampledImage(Image image, int width, int height, int leftInset, int topInset, int rightInset, int bottomInset) {
        if (image==null)
            throw new IllegalArgumentException("image cannot be null");
        return new Image(null, getResampledImageData(image.getImageData(), width, height, leftInset, topInset, rightInset, bottomInset));
    }

    public static ImageData getResampledImageData(ImageData imageData, int width, int height, int leftInset, int topInset, int rightInset, int bottomInset) {
        if (imageData==null)
            throw new IllegalArgumentException("imageData cannot be null");
        if (width <= leftInset+rightInset || height <= topInset+bottomInset)
            throw new IllegalArgumentException("invalid width, height, or insets values");

        // calculate scaled image width/height, preserving aspect ratio
        int scaledHeight = height - topInset - bottomInset;
        int scaledWidth = width - leftInset - rightInset;
        double scaleX = (double) scaledWidth / (double) imageData.width;
        double scaleY = (double) scaledHeight / (double) imageData.height;
        if (scaleX < scaleY)
            scaledHeight = (int) (scaleX * (scaledHeight / scaleY));
        else
            scaledWidth = (int) (scaleY * (scaledWidth / scaleX));
        int xoff = (width-scaledWidth+1)/2;
        int yoff = (height-scaledHeight+1)/2;

        // produce a high-quality re-sampled image (but transparency got lost along the way)
        Image scaledImage = new Image(null, width, height);
        GC gc = new GC(scaledImage);
        Color backgroundColor = new Color(null, 240, 241, 240);
        gc.setBackground(backgroundColor);
        gc.fillRectangle(0, 0, width, height);
        gc.setInterpolation(SWT.HIGH);
        gc.drawImage(new Image(gc.getDevice(),imageData), 0, 0, imageData.width, imageData.height, xoff, yoff, scaledWidth, scaledHeight);
        gc.dispose();

        // now, reproduce transparency
        if (imageData.getTransparencyType()==SWT.TRANSPARENCY_ALPHA) {
            // OK, original image has alpha channel. Trick: we create an
            // alpha mask image (a greyscale image purely from the alpha
            // channel as input), scale it like we did the original image,
            // then copy out the resulting greyscale values as alphas.

            // turn alpha into greyscale
            for (int y=0; y < imageData.height; y++) {
                for (int x=0; x < imageData.width; x++) {
                    int alpha = imageData.getAlpha(x, y);
                    imageData.setPixel(x, y, imageData.palette.getPixel(new RGB(alpha, alpha, alpha)));
                }
            }

            // rescale greyscale image
            Image alphaImage = new Image(null, imageData);
            Image scaledAlphaImage = new Image(null, width, height);
            GC gc2 = new GC(scaledAlphaImage);
            gc2.setBackground(new org.eclipse.swt.graphics.Color(null, 0, 0, 0));
            gc2.fillRectangle(0, 0, width, height);
            gc2.setInterpolation(SWT.HIGH);
            gc2.drawImage(alphaImage, 0, 0, imageData.width, imageData.height, xoff, yoff, scaledWidth, scaledHeight);
            gc2.dispose();

            // take greyscale values as alpha
            ImageData scaledAlphaData = scaledAlphaImage.getImageData();
            ImageData resultData = scaledImage.getImageData();
            for (int y=0; y < height; y++) {
                for (int x=0; x < width; x++) {
                    int alpha = scaledAlphaData.palette.getRGB(scaledAlphaData.getPixel(x, y)).blue;
                    resultData.setAlpha(x, y, alpha);
                }
            }

            Image resultImage = new Image(null, resultData);
            scaledImage.dispose();
            alphaImage.dispose();
            scaledAlphaImage.dispose();
            return resultImage.getImageData();
        }
        else if (imageData.getTransparencyType()==SWT.TRANSPARENCY_MASK || imageData.getTransparencyType()==SWT.TRANSPARENCY_PIXEL) {
            // This is likely a gif image. Just set our background color as the transparent color.
            ImageData resultData = scaledImage.getImageData();
            resultData.transparentPixel = resultData.palette.getPixel(backgroundColor.getRGB());
            scaledImage.dispose();
            Image resultImage = new Image(null, resultData);
            return resultImage.getImageData();
        }
        else {
            // TRANSPARENCY_NONE
            return scaledImage.getImageData();
        }
    }


    // For debugging, uncomment:
    // static {
    //     testImageResizing();
    // }

    protected static void testImageResizing() {
        Shell shell = new Shell((Shell)null, SWT.SHELL_TRIM);
        shell.setSize(750, 500);
        shell.setLayout(new FillLayout());
        Canvas canvas = new Canvas(shell, SWT.NONE);
        shell.layout();
        shell.open();

        int k = 0;
        GC gc = new GC(canvas);
        for (String s : ImageFactory.global().getImageNameList()) {
            Image image = ImageFactory.global().getImage(s);
            Image smallImage = ImageUtils.getResampledImage(image, 16, 16, 2, 2, 2, 2);
            int x = (k%40)*20, y = 20*(k/40);
            gc.drawImage(smallImage, x, y);

            Rectangle r = smallImage.getBounds();
            r.x += x;
            r.y += y;
            gc.setBackground(new Color(null, 0, 0, 0));
            gc.drawRectangle(r);

            k++;
        }
    }

    /**
     * Rescale the provided image to fit to targetWidth/height
     * (keeping the spect ratio), if its size is bigger than the targetSize,
     * otherwise (if it fits) the image is returned unchanged
     */
    public static Image fitImage(Image image, int width, int height) {
        if (image == null)
            return null;

        double ratio = Math.min((double)width / image.getBounds().width,
                                (double)height / image.getBounds().height);
        if (ratio < 1.0 && ratio > 0.0) {
            return new Image(image.getDevice(),
                    image.getImageData().scaledTo((int)(image.getBounds().width*ratio), (int)(image.getBounds().height*ratio)));
        }

        return image;
    }

}

