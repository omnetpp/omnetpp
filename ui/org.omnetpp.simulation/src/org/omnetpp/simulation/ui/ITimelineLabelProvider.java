package org.omnetpp.simulation.ui;

import org.eclipse.swt.graphics.GC;


/**
 * Label provider for the TimelineControl widget.
 * 
 * @author Andras
 */
public interface ITimelineLabelProvider {
    /**
     * Return the label to be displayed for the message.
     */
    String getMessageLabel(Object message);

    /**
     * Render the message using the given GC, centered at point (x,y).
     * Should only render the message symbol (e.g. a dot or icon), as the
     * label will be drawn by the timeline control itself.
     */
    void drawMessageSymbol(Object message, GC gc, int x, int y);
}
