package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Shape;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;

public class Completion extends Shape {
    private double completion;

    private boolean isFilled;

    private boolean isOutlined;

    private boolean isHanded;

    public Completion(boolean isFilled, boolean isOutlined, boolean isHanded, double completion, Color color) {
        this.isFilled = isFilled;
        this.isOutlined = isOutlined;
        this.isHanded = isHanded;
        this.completion = completion;
        setForegroundColor(color);
        setBackgroundColor(color);
    }

    public boolean isFilled() {
        return isFilled;
    }

    public void setFilled(boolean isFilled) {
        this.isFilled = isFilled;
    }

    public double getCompletion() {
        return completion;
    }

    public void setCompletion(double completion) {
        this.completion = completion;
        repaint();
    }

    @Override
    protected void fillShape(Graphics graphics) {
    }

    @Override
    protected void outlineShape(Graphics graphics) {
        Rectangle bounds = getBounds();
        if (isFilled)
            graphics.fillArc(bounds, 90, (int)(-360 * completion));
        if (isOutlined)
            graphics.drawArc(bounds.getResized(-1, -1), 0, 360);
        if (isHanded) {
            Point center = bounds.getCenter();
            double rad = 2 * Math.PI * completion - Math.PI / 2;
            graphics.setAlpha(255);
            graphics.drawLine(center.x, center.y, center.x, bounds.y);
            graphics.drawLine(center.x, center.y, (int)(center.x + bounds.width / 2 * Math.cos(rad)), (int)(center.y + bounds.height / 2 * Math.sin(rad)));
            if (getAlpha() != null)
                graphics.setAlpha(getAlpha());
        }
    }
}
