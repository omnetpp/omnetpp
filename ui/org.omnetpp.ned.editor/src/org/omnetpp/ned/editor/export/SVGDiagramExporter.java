/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.figures.misc.FigureUtils;

public class SVGDiagramExporter implements DiagramExporter
{
    public String getName()
    {
        return "SVG Exporter";
    }

    public String getDescription()
    {
        return "SVG - Scalable Vector Graphics";
    }

    public String getFileExtension()
    {
        return "svg";
    }

    public void exportDiagram(List<IFigure> figures, File file, boolean antialias)
    {
        // Render into a temporary file, and only put it in place if rendering was
        // successful. Otherwise a failed export would leave behind a truncated or
        // empty image file, which the caller has no chance of telling apart from a
        // properly exported one.
        File tmpFile = new File(file.getPath() + ".tmp");
        try {
            DisplayUtils.runNowOrSyncInUIThread(() -> renderIntoFile(figures, tmpFile));

            if (tmpFile.length() == 0)
                throw new RuntimeException("SVG export produced an empty file: " + file.getPath());
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
    protected void renderIntoFile(List<IFigure> figures, File file)
    {
        try (FileOutputStream outputStream = new FileOutputStream(file)) {
            Rectangle bounds = FigureUtils.getBounds(figures);
            SVGGraphics graphics = SVGGraphics.getInstance(bounds.getTranslated(bounds.getLocation().negate()));
            try {
                graphics.translate(bounds.getLocation().negate());

                // Re-scale the font so it will not follow the DPI setting.
                // We do not want bigger fonts in the generated diagrams
                // just because the system used to generate the documentation
                // had a higher than normal DPI setting.
                // Generated documents must be DPI independent.
                Point dpi = graphics.getFont().getDevice().getDPI();
                FontData fdata = graphics.getFont().getFontData()[0];
                fdata.setHeight((int)(fdata.getHeight() * 96.0 / dpi.x + 0.5));  // scale back
                Font scaledFont = new Font(graphics.getFont().getDevice(), fdata);

                try {
                    for (IFigure figure : figures) {
                        figure.setFont(scaledFont);
                        figure.paint(graphics);
                    }

                    graphics.getSVGGraphics2D().stream(new BufferedWriter(new OutputStreamWriter(outputStream)));
                }
                finally {
                    scaledFont.dispose();
                }
            }
            finally {
                graphics.dispose(); // it holds an SWT Image and GC which would leak otherwise
            }

            outputStream.flush();
        }
        catch (IOException e) { // note: SVGGraphics2DIOException is an IOException too
            throw new RuntimeException("Error while exporting to SVG: " + e.getMessage(), e);
        }
    }
}
