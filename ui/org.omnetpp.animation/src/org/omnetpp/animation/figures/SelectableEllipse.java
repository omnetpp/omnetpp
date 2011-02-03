package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.misc.ISelectableFigure;

public class SelectableEllipse extends Ellipse implements ISelectableFigure {
    private boolean isSelected;

    public boolean isSelected() {
        return this.isSelected;
    }

    public void setSelected(boolean isSelected) {
        if (isSelected == this.isSelected)
            return;
        else {
            this.isSelected = isSelected;
            repaint();
        }
    }

    @Override
    public void paint(Graphics graphics) {
        super.paint(graphics);
        if (isSelected) {
            graphics.setForegroundColor(ColorFactory.RED);
            Rectangle r = getBounds();
            graphics.drawRectangle(r.x, r.y, r.width - 1, r.height - 1);
        }
    }
}
