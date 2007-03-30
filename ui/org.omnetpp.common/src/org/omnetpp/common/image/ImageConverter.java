package org.omnetpp.common.image;

import java.awt.Color;
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
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.PaletteData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.Rectangle;

public class ImageConverter {

	public static BufferedImage convertToAWT(Image swtImage) {
		return convertToAWT(swtImage.getImageData());
	}

	public static Image convertToSWT(Device device, BufferedImage awtImage) {
		return new Image(device, convertToSWT(awtImage));
	}

	public static Image getResizedImage(Device device, Image swtImage, int width, int height) {
		BufferedImage awtImage = convertToAWT(swtImage);
		BufferedImage scaledAwtImage = getResizedImage(awtImage, width, height);
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
		} else {
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
			} else {
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
			Assert.isTrue(false, "converting from this colormodel not implemented");
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

	public static BufferedImage getResizedImage(BufferedImage img, int width, int height) {
		BufferedImage resultImage = new BufferedImage(width, height, BufferedImage.TYPE_4BYTE_ABGR);

		Graphics2D g2d = resultImage.createGraphics();
		g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
		//g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);

		// fill with transparent color
		g2d.setColor(new Color(255, 255, 255, 0));
		g2d.fillRect(0, 0, width, height);

		// draw image, preserving aspect ratio
		double scalex = (double) width / (double) img.getWidth(null);
		double scaley = (double) height / (double) img.getHeight(null);

		if (scalex < scaley) {
			g2d.drawImage(img, 0, (int)((height - img.getHeight(null) * scalex) / 2.0), width, (int) (img.getHeight(null) * scalex),  null);
		} else {
			g2d.drawImage(img, (int)((width - img.getWidth(null) * scaley) / 2.0), 0, (int) (img.getWidth(null) * scaley), height, null);
		}

		g2d.dispose();

		return resultImage;
	}    

	public static Image getResizedImage(Image image, int width, int height) {


		// calculate scaled image width/height, preserving aspect ratio
		Rectangle imageSize = image.getBounds();
		double scaleX = (double) width / (double) imageSize.width;
		double scaleY = (double) height / (double) imageSize.height;
		int scaledHeight = height, scaledWidth = width;
		if (scaleX < scaleY)
			scaledHeight = (int) (scaleX * (height / scaleY));
		else 
			scaledWidth = (int) (scaleY * (width / scaleX));
		int xoff = (width-scaledWidth)/2;
		int yoff = (height-scaledHeight)/2;
		
		// produce a high-quality resample image (but this won't have alpha channel)
		Image scaledImage = new Image(null, width, height);
		GC gc = new GC(scaledImage);
		gc.setInterpolation(SWT.HIGH);
		gc.drawImage(image, 0, 0, imageSize.width, imageSize.height, xoff, yoff, scaledWidth, scaledHeight);
		gc.dispose();

		// now, produce alpha channel...
		ImageData data = scaledImage.getImageData();
		data.alphaData = new byte[width*height]; // this is all zero == fully transparent
		
		byte[] alphas = image.getImageData().scaledTo(scaledWidth, scaledHeight).alphaData;
		if (alphas==null) 
			System.out.println("bad image!!! "); //XXX

//		for (int y=1; y < scaledHeight-1; y++)
//			for (int x=1; x < scaledWidth-1; x++)
		
		
		for (int y=0; y < scaledHeight; y++)
			for (int x=0; x < scaledWidth; x++)
				data.alphaData[(y+yoff)*width+(x+xoff)] = alphas[y*scaledWidth+x];

		Image resultImage = new Image(null, data);
		scaledImage.dispose();
		return resultImage;
	}    
	
}

