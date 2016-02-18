/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.figures.misc.FigureUtils;

public class PNGDiagramExporter implements DiagramExporter
{
    public String getName()
    {
        return "PNG Exporter";
    }

    public String getDescription()
    {
        return "PNG - Portable Network Graphics";
    }

    public String getFileExtension()
    {
        return "png";
    }

    public void exportDiagram(List<IFigure> figures, File file, boolean antialias)
    {
        try {
            FileOutputStream outputStream = new FileOutputStream(file);
            Rectangle bounds = FigureUtils.getBounds(figures);
            Image image = new Image(Display.getDefault(), bounds.width, bounds.height);
            for (IFigure figure : figures) {
                GC gc = new GC(image);
                gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
                gc.setTextAntialias(antialias ? SWT.ON : SWT.OFF);
                Graphics graphics = new SWTGraphics(gc);
                graphics.translate(bounds.getLocation().negate());
                figure.paint(graphics);
                graphics.dispose();
                gc.dispose();
            }
            ImageLoader imageLoader = new ImageLoader();
            imageLoader.data = new ImageData[] { image.getImageData() };
            imageLoader.save(outputStream, SWT.IMAGE_PNG);
            image.dispose();
            outputStream.flush();
            outputStream.close();
        }
        catch (IOException e) {
            throw new RuntimeException("Error while exporting to PNG", e);
        }
    }
}
