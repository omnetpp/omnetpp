/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.geometry.PointList;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.ConnectionFigure;

/**
 * Figure for a channel or channel interface NED type.
 *
 * @author andras
 */
public class ConnectionTypeFigure extends NedTypeFigure {
    protected ConnectionFigure connectionFigure = new ConnectionFigure();

    public ConnectionTypeFigure() {
        titleArea.add(connectionFigure, 0); // the 0 index is needed so the problem image will be displayed above connectionFigure (z-ordering)
        connectionFigure.setArrowHeadEnabled(true);
    }

    /**
     * Adjusts the figure properties using a displayString object
     */
    public void setDisplayString(IDisplayString dps, IProject project) {

        // we currently ignore the display strings of interfaces
        int lineWidth = isInterface() ? 1 : dps.getAsInt(IDisplayString.Prop.CONNECTION_WIDTH, 1);
        Color lineColor = isInterface() ? ColorFactory.GREY65 : ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.CONNECTION_COLOR));
        String lineStyle = isInterface() ? "da" : dps.getAsString(IDisplayString.Prop.CONNECTION_STYLE);

        int height = Math.max(10, 4*lineWidth);
        int width = isInnerType() ? 24 : 40;
        if (height > width)  width = height;  // tweak for large line widths

        connectionFigure.setPoints(new PointList(new int[] {0, 3+height/2, width-2*lineWidth/3, 3+height/2}));
        connectionFigure.setStyle(lineColor, lineWidth, lineStyle);

        titleArea.setConstraint(connectionFigure, new Rectangle(0,0,width, height));
        titleArea.setConstraint(nameFigure, new Rectangle(width+4, 2, -1, height));

        invalidateTree();
    }

}
