/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;

/**
 * Helper class for generating images of symbols displayed
 * in the legend and tooltips of charts.
 *
 * @author tomi
 */
public class SymbolImageFactory {

    private static Map<String,Image> imageMap = new HashMap<String,Image>();

    public static Image getImage(String imageName) {
        return imageMap.get(imageName);
    }

    public static Image getOrCreateImage(IPlotSymbol symbol, Color color, boolean drawLine) {
        String imageName = prepareImage(symbol, color, drawLine);
        return getImage(imageName);
    }

    public static String prepareImage(IPlotSymbol symbol, Color color, boolean drawLine) {
        String imageName = makeImageName(symbol, color, drawLine);

        if (!imageMap.containsKey(imageName)) {
            Image image = createImage(symbol, color, drawLine);
            imageMap.put(imageName, image);
        }

        return imageName;
    }

    public static String makeImageName(IPlotSymbol symbol, Color color, boolean drawLine) {
        String symbolName = symbol == null ? "NoneSymbol" : StringUtils.removeStart(symbol.getClass().getName(), symbol.getClass().getPackage().getName()+".");
        String imageName = String.format("%s_%02X%02X%02X%s",
                symbolName, color.getRed(), color.getGreen(), color.getBlue(),
                drawLine ? "_l" : "");

        return imageName;
    }

    private static Image createImage(IPlotSymbol symbol, Color color, boolean drawLine) {
        int size = symbol != null ? symbol.getSizeHint() : 0;
        Image image = null;
        GC gc = null;
        try {
            image = new Image(null, 15, 9);
            gc = new GC(image);
            gc.setAntialias(SWT.ON);
            gc.setForeground(color);
            if (drawLine) {
                gc.setLineWidth(2);
                gc.setLineStyle(SWT.LINE_SOLID);
                gc.drawLine(0, 4, 14, 4);
            }
            if (symbol != null) {
                symbol.setSizeHint(6);
                symbol.drawSymbol(new SWTGraphics(gc), 7, 4);
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
