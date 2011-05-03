package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.misc.ISelectableFigure;

public class SelectableImageFigure extends ImageFigure implements ISelectableFigure {
    private boolean isSelected;

    private int alpha = 255;

    public SelectableImageFigure(Image image) {
        super(image);
    }

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

    public void setAlpha(int alpha) {
        this.alpha = alpha;
    }

    @Override
    public void paint(Graphics graphics) {
        int oldAlpha = graphics.getAlpha();
        graphics.setAlpha(alpha);
        super.paint(graphics);
        if (isSelected) {
            graphics.setForegroundColor(ColorFactory.RED);
            Rectangle r = getBounds();
            graphics.drawRectangle(r.x, r.y, r.width - 1, r.height - 1);
        }
        graphics.setAlpha(oldAlpha);
    }
}
