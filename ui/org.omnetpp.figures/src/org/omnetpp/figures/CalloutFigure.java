/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.RoundedRectangle;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * A figure that resembles a cartoon style callout box. Can show/hide itself using a timer
 *
 * @author rhornig
 */
// you may attach the figure to an other figure at specific anchor points, for example:
// parent.add(new AttachedLayer(attachToFigure, PositionConstants.NORTH,
//                              calloutFigure, PositionConstants.SOUTH_WEST));

// TODO implement background timeout and hiding the expired entries
public class CalloutFigure extends Layer {

    private Layer bubbleLayer = new Layer();
    private Shape mainShape = new RoundedRectangle();

    public CalloutFigure() {
        setLayoutManager(new ToolbarLayout());
        setBackgroundColor(ColorConstants.tooltipBackground);
        setForegroundColor(ColorConstants.tooltipForeground);

        mainShape.setLayoutManager(new ToolbarLayout());

        bubbleLayer.setLayoutManager(new XYLayout());
        bubbleLayer.add(new Ellipse(), new Rectangle(0,20,10,5));
        bubbleLayer.add(new Ellipse(), new Rectangle(10,11,30,10));
        bubbleLayer.add(new Ellipse(), new Rectangle(25,-4,70,15));

        add(mainShape);
        add(bubbleLayer);
    }

    public void addCallout(IFigure fig) {
        setVisible(true);
        fig.setBorder(new MarginBorder(3));
        mainShape.add(fig);
    }

    public void clearCallout() {
        setVisible(false);
        mainShape.removeAll();
    }

}
