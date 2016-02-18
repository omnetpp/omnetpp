/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.util.List;

import org.eclipse.draw2d.IFigure;

public interface DiagramExporter
{
    public String getName();

    public String getDescription();

    public String getFileExtension();

    public void exportDiagram(List<IFigure> figures, File file, boolean antialias);
}
