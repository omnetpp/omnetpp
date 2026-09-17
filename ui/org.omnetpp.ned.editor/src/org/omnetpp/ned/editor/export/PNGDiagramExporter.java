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
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.figures.misc.FigureUtils;

public class PNGDiagramExporter implements DiagramExporter
{
    private double scale = 1.0;

    /**
     * Sets the rendering scale. Rendering at a scale larger than 1 produces images that
     * are still crisp on high resolution displays, provided that they are displayed at
     * their logical (i.e. divided by the scale) size.
     */
    public void setScale(double scale) {
        this.scale = scale;
    }

    public double getScale() {
        return scale;
    }

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
        // Render into a temporary file, and only put it in place if rendering was
        // successful. Otherwise a failed export would leave behind a truncated or
        // empty image file, which the caller has no chance of telling apart from a
        // properly exported one.
        File tmpFile = new File(file.getPath() + ".tmp");
        try {
            DisplayUtils.runNowOrSyncInUIThread(() -> renderIntoFile(figures, tmpFile, antialias));

            if (tmpFile.length() == 0)
                throw new RuntimeException("PNG export produced an empty file: " + file.getPath());
            if (file.exists() && !file.delete())
                throw new RuntimeException("Cannot overwrite existing file: " + file.getPath());
            if (!tmpFile.renameTo(file))
                throw new RuntimeException("Cannot rename '" + tmpFile.getPath() + "' to '" + file.getPath() + "'");
        }
        finally {
            tmpFile.delete(); // no-op if it has been successfully renamed
        }
    }

    /**
     * Renders the figures into the given file. Must be called from the UI thread.
     */
    protected void renderIntoFile(List<IFigure> figures, File file, boolean antialias)
    {
        try (FileOutputStream outputStream = new FileOutputStream(file)) {
            Rectangle bounds = FigureUtils.getBounds(figures);
            Image image = new Image(Display.getDefault(),
                    (int)Math.ceil(bounds.width * scale), (int)Math.ceil(bounds.height * scale));
            try {
                for (IFigure figure : figures) {
                    GC gc = new GC(image);
                    try {
                        gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
                        gc.setTextAntialias(antialias ? SWT.ON : SWT.OFF);
                        Graphics graphics = new SWTGraphics(gc);
                        try {
                            if (scale != 1.0)
                                graphics.scale(scale);
                            graphics.translate(bounds.getLocation().negate());
                            figure.paint(graphics);
                        }
                        finally {
                            graphics.dispose();
                        }
                    }
                    finally {
                        gc.dispose();
                    }
                }

                ImageLoader imageLoader = new ImageLoader();
                imageLoader.data = new ImageData[] { image.getImageData() };
                imageLoader.save(outputStream, SWT.IMAGE_PNG);
            }
            finally {
                image.dispose();
            }

            outputStream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException("Error while exporting to PNG: " + e.getMessage(), e);
        }
    }
}
