package org.omnetpp.figures.misc;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PointList;

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
        switch (getAlignment()) {
            case SOURCE:
                return points.getPoint(Point.SINGLETON, 0);
            case TARGET:
                return points.getPoint(Point.SINGLETON, points.size() - 1);
            case MIDDLE:
                if (points.size() % 2 == 0) {
                    int i = points.size() / 2;
                    int j = i - 1;
                    Point p1 = points.getPoint(j);
                    Point p2 = points.getPoint(i);
                    Dimension d = p2.getDifference(p1);

                    int pos = PositionConstants.NONE;
                    if (d.height < 0) pos |=  PositionConstants.EAST;
                    if (d.height > 0) pos |=  PositionConstants.WEST;
                    if (d.width < 0) pos |=  PositionConstants.NORTH;
                    if (d.width > 0) pos |= PositionConstants.SOUTH;
                    setRelativePosition(pos);
                    setGap(2);

                    return Point.SINGLETON.setLocation(p1.x + d.width * 2 / 3,
                                                        p1.y + d.height * 2 / 3);
                }
                int i = (points.size() - 1) / 2;
                return points.getPoint(Point.SINGLETON, i);
            default:
                return new Point();
        }
    }
}
