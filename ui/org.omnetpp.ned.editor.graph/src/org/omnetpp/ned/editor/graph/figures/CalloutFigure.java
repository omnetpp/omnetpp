package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.*;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * @author rhornig
 * A figure that resembles a cartoon style callout box. Can show/hide itself using a timer
 */
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
