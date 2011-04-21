package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.geometry.Rectangle;
public class Completion extends Shape {
    private double completion;

    public double getCompletion() {
        return completion;
    }

    public void setCompletion(double completion) {
        this.completion = completion;
    }

    @Override
    protected void fillShape(Graphics graphics) {
    }

    @Override
    protected void outlineShape(Graphics graphics) {
        Rectangle bounds = getBounds();
        //graphics.drawArc(bounds.getResized(-1, -1), 0, 360);
        graphics.fillArc(bounds, 0, (int)(360 * completion));
    }
}
