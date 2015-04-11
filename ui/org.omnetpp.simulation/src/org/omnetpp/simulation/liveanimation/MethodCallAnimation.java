/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.liveanimation;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MidpointLocator;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.omnetpp.common.color.ColorFactory;

/**
 *
 * @author Andras
 */
public class MethodCallAnimation extends AbstractAnimationPrimitive {
    private IFigure layer;
    private PolylineConnection arrowFigure; //TODO use a PolylineConnection; plus see ConnectionFigure.setArrowHeadEnabled() for creating an arrowhead
    private Point startLocation, endLocation;  //TODO connection anchors
    private double creationTime, removalTime;
    private String methodCallText;

    public MethodCallAnimation(IFigure layer, String methodCallText, Point startLocation, Point endLocation, double creationTime) {
        this.layer = layer;
        this.methodCallText = methodCallText;
        this.startLocation = startLocation;
        this.endLocation = endLocation;
        this.creationTime = creationTime;
        // and to set this.removalTime use setRemovalTime()
    }

    // needed because we only learn this later!
    public void setRemovalTime(double removalTime) {
        this.removalTime = removalTime;
    }

    @Override
    public boolean updateFor(double time) {
        if (time < creationTime) {
            return true;
        }
        else if (time >= removalTime) {
            removeFigure();
            return false;
        }
        else {
            if (arrowFigure == null) {
                arrowFigure = new PolylineConnection();
                PolygonDecoration arrowHead = new PolygonDecoration();
                arrowHead.setTemplate(PolygonDecoration.TRIANGLE_TIP);
                arrowFigure.setTargetDecoration(arrowHead);
                arrowFigure.setForegroundColor(ColorFactory.GREEN4);
                Label labelFigure = new Label();
                labelFigure.setText(methodCallText);
                arrowFigure.add(labelFigure, new MidpointLocator(arrowFigure, 0));

                layer.add(arrowFigure);

                arrowFigure.setPoints(new PointList(new int[] {startLocation.x, startLocation.y, endLocation.x, endLocation.y}));
                arrowFigure.getParent().revalidate();
            }
            return true;
        }
    }

    public void removeFigure() {
        if (arrowFigure != null && arrowFigure.getParent() != null)
            arrowFigure.getParent().remove(arrowFigure);
        arrowFigure = null;
    }
}
