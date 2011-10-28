package org.omnetpp.animation.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.simulation.QueueModel;

public class QueueFigure extends AbstractAnimationFigure {
    public QueueFigure(QueueModel model) {
        super(model);
        setSize(100, 20);
    }

    public QueueModel getQueue() {
        return (QueueModel)model;
    }

    public void refeshAppearance() {
        repaint();
    }

    @Override
    protected void paintClientArea(Graphics graphics) {
        super.paintClientArea(graphics);
        Rectangle clientArea = getClientArea();
        graphics.drawLine(clientArea.getTopLeft(), clientArea.getTopRight());
        graphics.drawLine(clientArea.getBottomLeft().translate(0, -1), clientArea.getBottomRight().translate(0, -1));
        graphics.drawLine(clientArea.getTopLeft(), clientArea.getBottomLeft());
        int count = getQueue().getCount();
        for (int i = 0; i < count; i++) {
            int width = (clientArea.width() - 2 - getQueue().getSize()) / getQueue().getSize();
            int height = clientArea.height - 2;
            graphics.setBackgroundColor(ColorFactory.BLUE2);
            graphics.fillRectangle(clientArea.x + 1 + (width + 1) * i, clientArea.y + 1, width, height);
        }
    }
}
