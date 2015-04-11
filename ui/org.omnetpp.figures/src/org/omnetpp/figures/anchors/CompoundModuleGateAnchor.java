/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.anchors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Gate anchor used by the compound module. It can be locked to a specific point
 * on the edge of the compound module or can be left "floating" along any edge
 * resulting the shortest connection possible to that side of the compound module
 *
 * @author rhornig
 */
public class CompoundModuleGateAnchor extends GateAnchor {
    protected int edgeConstraint = PositionConstants.NSEW;
    protected Point tempRefPoint = new Point(0,0);

    public CompoundModuleGateAnchor(IFigure owner) {
        super(owner);
    }

    /**
     * Gets a Rectangle from {@link #getBox()} and returns the Point where a line from the
     * center of the Rectangle to the Point <i>reference</i> intersects the Rectangle.
     *
     * @param reference The reference point
     * @return The anchor location
     */
    @Override
    public Point getLocation(Point reference) {
        Rectangle box = Rectangle.SINGLETON;
        box.setBounds(getBox());
        box.translate(-1, -1);
        box.resize(1, 1);

        Point p = Point.SINGLETON;

        getOwner().translateToAbsolute(box);

        int distance = Integer.MAX_VALUE;

        if ((edgeConstraint & PositionConstants.NORTH) != 0) {
            int currDistance = Math.abs(box.y - reference.y);
            if(currDistance < distance) {
                distance = currDistance;
                p.x = reference.x;
                p.y = box.y;
            }
        }
        if ((edgeConstraint & PositionConstants.SOUTH) != 0) {
            int currDistance = Math.abs(box.y + box.height - reference.y);
            if(currDistance < distance) {
                distance = currDistance;
                p.x = reference.x;
                p.y = box.y + box.height;
            }
        }
        if ((edgeConstraint & PositionConstants.EAST) != 0) {
            int currDistance = Math.abs(box.x + box.width - reference.x);
            if(currDistance < distance) {
                distance = currDistance;
                p.x = box.x + box.width;
                p.y = reference.y;
            }
        }
        if ((edgeConstraint & PositionConstants.WEST) != 0) {
            int currDistance = Math.abs(box.x - reference.x);
            if(currDistance < distance) {
                distance = currDistance;
                p.x = box.x;
                p.y = reference.y;
            }
        }

        // use this calculated location as an approximate reference point
        tempRefPoint.setLocation(p);
        return new Point(Math.round(p.x), Math.round(p.y));
    }

    /**
     * Returns the anchor's reference point.
     */
    // TODO handle the reference point calculation correctly
    @Override
    public Point getReferencePoint() {
        Point ref = tempRefPoint.getCopy();
        getOwner().translateToAbsolute(ref);
        return ref;
    }

    /**
     * Sets the edge constraint that specifies where the connection anchor is free to move.
     * See PositionConstants for valid values.
     */
    public void setEdgeConstraint(int edgeConstraint) {
        this.edgeConstraint = edgeConstraint;
    }
}
