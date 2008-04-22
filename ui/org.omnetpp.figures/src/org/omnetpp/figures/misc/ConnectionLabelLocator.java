package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ConnectionLabelLocator extends ConnectionLocator {

    public ConnectionLabelLocator(Connection connection) {
        super(connection);
    }

    @Override
    protected Point getReferencePoint() {
        return super.getReferencePoint();
    }

    @Override
    public void setAlignment(int align) {
        super.setAlignment(align);
    }

    @Override
    protected Point getLocation(PointList points) {
        Point p1 = points.getPoint(0);
        Point p2 = points.getPoint(points.size() - 1);
        Point refP;
        Dimension d = p2.getDifference(p1);

        switch (getAlignment()) {
            case SOURCE:
                refP = p1;
                break;
            case TARGET:
                refP = p2;
                break;
            case MIDDLE:
                if (points.size() % 2 == 0) {
                    int i = points.size() / 2;
                    int j = i - 1;
                    p1 = points.getPoint(j);
                    p2 = points.getPoint(i);
                    d = p2.getDifference(p1);

                    refP = new Point(p1.x + d.width * 2 / 3, p1.y + d.height * 2 / 3);

                    break;
                }
                int i = (points.size() - 1) / 2;
                refP = points.getPoint(i);
                break;
            default:
                return new Point();
        }

        int pos = PositionConstants.NONE;
        if (d.height < 0) pos |=  PositionConstants.EAST;
        if (d.height > 0) pos |=  PositionConstants.WEST;
        if (d.width < 0) pos |=  PositionConstants.NORTH;
        if (d.width > 0) pos |= PositionConstants.SOUTH;
        setRelativePosition(pos);
        setGap(2);

        return refP;
    }
}
