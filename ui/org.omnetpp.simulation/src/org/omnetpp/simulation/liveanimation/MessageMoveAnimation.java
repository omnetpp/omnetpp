/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.liveanimation;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.simulation.figures.MessageFigure;

/**
 *
 * @author Andras
 */
public class MessageMoveAnimation extends AbstractAnimationPrimitive {
    private IFigure layer;
    private MessageFigure figure;
    private String label;
    private Point startLocation, endLocation;
    private double startTime, endTime;

    public MessageMoveAnimation(IFigure layer, String label, Point startLocation, Point endLocation, double startTime, double endTime) {
        this.layer = layer;
        this.label = label;
        this.startLocation = startLocation;
        this.endLocation = endLocation;
        this.startTime = startTime;
        this.endTime = endTime;
    }

    @Override
    public boolean updateFor(double time) {
        boolean more = time < endTime;
        if (time < startTime || time > endTime) {
            removeFigure();
            return more;
        }

        if (figure == null)
            figure = new MessageFigure(label);

        double alpha = (time - startTime) / (endTime - startTime);
        Point location = startLocation.getTranslated(endLocation.getDifference(startLocation).scale(alpha));
        figure.setCenterLocation(location);

        if (figure.getParent() != layer)
            layer.add(figure);

//        figure.getParent().revalidate();

        if (!more)
            removeFigure();
        return more;
    }

    public void removeFigure() {
        if (figure != null && figure.getParent() != null)
            figure.getParent().remove(figure);
        figure = null;
    }
}
