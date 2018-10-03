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

import org.apache.batik.svggen.SVGGraphics2DIOException;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gmf.runtime.draw2d.ui.render.awt.internal.svg.export.GraphicsSVG;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.figures.misc.FigureUtils;

@SuppressWarnings("restriction")
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
        Display.getDefault().syncExec(() -> {
            try (FileOutputStream outputStream = new FileOutputStream(file)) {
                    Rectangle bounds = FigureUtils.getBounds(figures);
                    GraphicsSVG graphics = GraphicsSVG.getInstance(bounds.getTranslated(bounds.getLocation().negate()));
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

                    for (IFigure figure : figures) {
                        figure.setFont(scaledFont);
                        figure.paint(graphics);
                    }

                    graphics.getSVGGraphics2D().stream(new BufferedWriter(new OutputStreamWriter(outputStream)));
                    scaledFont.dispose();
                    outputStream.flush();
            }
            catch (SVGGraphics2DIOException e) {
                throw new RuntimeException("Error while exporting to SVG", e);
            }
            catch (IOException e) {
                throw new RuntimeException("Error while exporting to SVG", e);
            }
        });
    }
}
