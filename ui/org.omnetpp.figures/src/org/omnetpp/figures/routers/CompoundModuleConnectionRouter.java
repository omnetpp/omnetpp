/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.routers;

import org.eclipse.draw2d.AbstractRouter;
import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.IAnchorBounds;

/**
 * Very basic straight connection router. It handles a CompoundModuleGateAnchor
 * differently so reference point can be calculated correctly.
 *
 * @author rhornig
 */
public class CompoundModuleConnectionRouter extends AbstractRouter
{
    public CompoundModuleConnectionRouter() { }

    /**
     * Routes the given Connection directly between the source and target anchors.
     * Handles CompoundModuleGateAnchors differently, because they cannot provide a
     * reference point without knowing the other end's reference point
     * @param conn the connection to be routed
     */
    public void route(Connection conn) {
        PointList points = conn.getPoints();
        points.removeAllPoints();

        if (!isSelfConnection(conn)) {
            // connection between two different modules (straight line)
            Point start, end;
            conn.translateToRelative(start = getStartPoint(conn));
            conn.translateToRelative(end = getEndPoint(conn));
            points.addPoint(start);
            points.addPoint(end);
        }
        else {
            // self connections (both src and target is the same figure.
            // draw a 3/4 circle in upper right corner for submodules
            // and a 1/4 circle for compound modules
            IAnchorBounds owner = (IAnchorBounds)conn.getSourceAnchor().getOwner(); // can be only a submodule or compound module figure
            Point center = owner.getAnchorBounds().getTopRight();
            conn.getSourceAnchor().getOwner().translateFromParent(center);

            double radius = 14;
            double delta = Math.PI*2/20;
            double angle = Math.PI/2;
            int steps = 16;
            if (owner instanceof CompoundModuleFigure) {
                // compound modules have only a 1/4 circle
                delta = -delta;
                steps = 6;
                radius += 10;
            }
            for (int i=0; i<steps; ++i, angle += delta) {
                points.addPoint((int)(0.5+center.x-Math.sin(angle)*radius),
                                (int)(0.5+center.y+Math.cos(angle)*radius));
            }
        }

        conn.setPoints(points);
    }

    protected boolean isSelfConnection(Connection conn) {
        return conn.getSourceAnchor().getOwner() == conn.getTargetAnchor().getOwner();
    }

    protected Point getEndPoint(Connection conn) {
        Point ref = conn.getSourceAnchor().getReferencePoint();
        // if this is a self connection (src == target) then the connection should end at the right side of the owner figure
        // this require that we use a point to the right from the figure as a reference point
        if (isSelfConnection(conn))
            ref.x += 1000000;
        // if the source anchor is a compound module anchor use the anchor location as reference point
        if (conn.getSourceAnchor() instanceof CompoundModuleGateAnchor)
            ref = conn.getSourceAnchor().getLocation(conn.getTargetAnchor().getReferencePoint());
        return new Point((conn.getTargetAnchor().getLocation(ref)));
    }

    protected Point getStartPoint(Connection conn) {
        Point ref = conn.getTargetAnchor().getReferencePoint();
        // if this is a self connection (src == target) then the connection should start at the top side of the owner figure
        // this require that we use a point above the figure as a reference point
        if (isSelfConnection(conn))
            ref.y -= 1000000;
        // if the target anchor is a compound module anchor use the anchor location as reference point
        if (conn.getTargetAnchor() instanceof CompoundModuleGateAnchor)
            ref = conn.getTargetAnchor().getLocation(conn.getSourceAnchor().getReferencePoint());

        return new Point(conn.getSourceAnchor().getLocation(ref));
    }
}


