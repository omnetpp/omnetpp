package org.omnetpp.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Draws a '?' or a 'o' sign on the connection based on the conditional or group flags.
 * This figure is not selectable.
 *
 * @author levy
 */
public class ConnectionKindFigure extends Shape {
    protected boolean isConditional;

    protected boolean isGroup;

    public ConnectionKindFigure(boolean isConditional, boolean isGroup) {
        this.isConditional = isConditional;
        this.isGroup = isGroup;
        setFill(false);
        setSize(32, 32);
    }

    // KLUDGE: we don't want to allow selecting a connection by clicking on its visible label
    // NOTE: overriding findFigureAt does not work
    @Override
    public boolean containsPoint(int x, int y) {
        return false;
    };

    @Override
    protected void fillShape(Graphics graphics) {
        graphics.fillOval(getBounds());
    }

    /**
     * Outlines the ellipse.
     * @see org.eclipse.draw2d.Shape#outlineShape(org.eclipse.draw2d.Graphics)
     */
    @Override
    protected void outlineShape(Graphics graphics) {
        Rectangle r = Rectangle.SINGLETON;
        r.setBounds(getBounds());

        if (isGroup)
            graphics.drawOval(r.getCenter().x - 3, r.getCenter().y - 3, 6, 6);

        if (isConditional)
            graphics.drawText("?", new Point(r.getCenter().x + 3, r.getCenter().y));
    }
}
