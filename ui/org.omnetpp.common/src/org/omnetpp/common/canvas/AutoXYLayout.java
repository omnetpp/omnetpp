package org.omnetpp.common.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

public class AutoXYLayout extends XYLayout {
    @SuppressWarnings("unchecked")
    @Override
    public void layout(IFigure container) {
        int spacing = 2;
        List<IFigure> figures = container.getChildren();
        for (IFigure figure : figures) {
            if (getConstraint(figure) == null) {
                Dimension size = figure.getPreferredSize();
                ArrayList<Point> candidates = new ArrayList<Point>();
                candidates.add(new Point(0, 0));
                for (IFigure otherFigure : figures) {
                    if (otherFigure != figure) {
                        Rectangle otherRectangle = (Rectangle)getConstraint(otherFigure);
                        if (otherRectangle != null) {
                            otherRectangle.setSize(otherFigure.getPreferredSize());
                            candidates.add(otherRectangle.getTopLeft().translate(-spacing - size.width, 0));
                            candidates.add(otherRectangle.getTopLeft().translate(0, -spacing - size.height));
                            candidates.add(otherRectangle.getTop().translate(-size.width / 2, -spacing - size.height));
                            candidates.add(otherRectangle.getTopRight().translate(spacing, 0));
                            candidates.add(otherRectangle.getTopRight().translate(-size.width, -spacing - size.height));
                            candidates.add(otherRectangle.getRight().translate(spacing, - size.height / 2));
                            candidates.add(otherRectangle.getBottomRight().translate(-size.width, spacing));
                            candidates.add(otherRectangle.getBottomRight().translate(spacing, -size.height));
                            candidates.add(otherRectangle.getBottom().translate(-size.width / 2, spacing));
                            candidates.add(otherRectangle.getBottomLeft().translate(-spacing - size.width, -size.height));
                            candidates.add(otherRectangle.getBottomLeft().translate(0, spacing));
                            candidates.add(otherRectangle.getLeft().translate(-spacing - size.width, -size.height / 2));
                        }
                    }
                }
                double bestDistance = Double.POSITIVE_INFINITY;
                Point bestPoint = null;
                outer: for (Point point : candidates) {
                    double distance = point.getDistance(new Point(0, 0));
                    Rectangle rectangle = new Rectangle(point, size);
                    for (IFigure otherFigure : figures) {
                        Rectangle otherRectangle = (Rectangle)getConstraint(otherFigure);
                        if (otherRectangle != null) {
                            otherRectangle.setSize(otherFigure.getPreferredSize());
                            if (otherFigure != figure && otherRectangle.intersects(rectangle))
                                continue outer;
                        }
                    }
                    if (point.x >= 0 && point.y >= 0 && distance < bestDistance) {
                        bestPoint = point;
                        bestDistance = distance;
                    }
                }
                setConstraint(figure, new Rectangle(bestPoint.x, bestPoint.y, -1, -1));
            }
        }
        super.layout(container);
    }
}