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
        try {
            FileOutputStream outputStream = new FileOutputStream(file);
            Rectangle bounds = FigureUtils.getBounds(figures);
            GraphicsSVG graphics = GraphicsSVG.getInstance(bounds.getTranslated(bounds.getLocation().negate()));
            graphics.translate(bounds.getLocation().negate());
            for (IFigure figure : figures)
                figure.paint(graphics);
            graphics.getSVGGraphics2D().stream(new BufferedWriter(new OutputStreamWriter(outputStream)));
            outputStream.flush();
            outputStream.close();
        }
        catch (SVGGraphics2DIOException e) {
            throw new RuntimeException("Error while exporting to SVG", e);
        }
        catch (IOException e) {
            throw new RuntimeException("Error while exporting to SVG", e);
        }
    }
}
