package org.omnetpp.scave.charting;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.collections.keyvalue.MultiKey;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

/**
 * Helper class for generating images of symbols displayed
 * in the legend and tooltips of charts. 
 *
 * @author tomi
 */
public class SymbolImageFactory {
	
	static Map<MultiKey,String> imageFileMap = new HashMap<MultiKey,String>();
	
	/**
	 * Returns the file name of the image containing the given symbol
	 * in the given color.
	 */
	public static String getImageFile(Color color, IChartSymbol symbol, boolean drawLine) {
		MultiKey key = new MultiKey(color, symbol == null ? null : symbol.getClass(), drawLine);
		String fileName = imageFileMap.get(key);
		if (fileName == null) {
			fileName = createImageFile(color, symbol, drawLine);
			imageFileMap.put(key, fileName);
		}
		return fileName;
	}
	
	private static String createImageFile(Color color, IChartSymbol symbol, boolean drawLine) {
		Image image = null;
		try {
			image = createSymbolImage(symbol, color, drawLine);
			String symbolName = symbol == null ? "NoneSymbol" : StringUtils.removeStart(symbol.getClass().getName(), symbol.getClass().getPackage().getName()+".");
			String imageName = String.format("%s_%02X%02X%02X%s",
					symbolName, color.getRed(), color.getGreen(), color.getBlue(),
					drawLine ? "_l" : "");
			return ImageFactory.createTemporaryImageFile(imageName+".png", image, SWT.IMAGE_PNG);
		} catch (Exception e) {
			return null;
		}
		finally {
			if (image != null)
				image.dispose();
		}
	}
	
	private static Image createSymbolImage(IChartSymbol symbol, Color color, boolean drawLine) {
		int size = symbol != null ? symbol.getSizeHint() : 0;
		Image image = null;
		GC gc = null;
		try {
			image = new Image(null, 15, 9);
			gc = new GC(image);
			gc.setAntialias(SWT.ON);
			gc.setForeground(color);
			if (drawLine) {
				gc.setLineWidth(1);
				gc.setLineStyle(SWT.LINE_SOLID);
				gc.drawLine(0, 4, 14, 4);
			}
			if (symbol != null) {
				symbol.setSizeHint(6);
				symbol.drawSymbol(gc, 7, 4);
			}
		}
		finally {
			if (symbol != null)
				symbol.setSizeHint(size);
			if (gc != null)
				gc.dispose();
		}
		return image;
	}
}
