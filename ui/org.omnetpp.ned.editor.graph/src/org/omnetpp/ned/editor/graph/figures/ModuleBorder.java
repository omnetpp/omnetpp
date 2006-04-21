package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.AbstractBorder;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;

public class ModuleBorder extends AbstractBorder {

    protected static Insets insets = new Insets(8, 36, 8, 6);
    public Insets getInsets(IFigure figure) {
        return insets;
    }

    public void paint(IFigure figure, Graphics g, Insets in) {
        Rectangle r = figure.getBounds().getCropped(in);

        g.setForegroundColor(ColorFactory.logicGreen);
        g.setBackgroundColor(ColorFactory.logicGreen);

        // Draw the sides of the border
        g.fillRectangle(r.x, r.y + 2, r.width, 6);
        g.fillRectangle(r.x, r.bottom() - 8, r.width, 6);
        g.fillRectangle(r.x, r.y + 2, 6, r.height - 4);
        g.fillRectangle(r.right() - 6, r.y + 2, 6, r.height - 4);

        // Outline the border
        g.setForegroundColor(ColorFactory.connectorGreen);
        g.drawLine(r.x, r.y + 2, r.right() - 1, r.y + 2);
        g.drawLine(r.x, r.bottom() - 3, r.right() - 1, r.bottom() - 3);
        g.drawLine(r.x, r.y + 2, r.x, r.bottom() - 3);
        g.drawLine(r.right() - 1, r.bottom() - 3, r.right() - 1, r.y + 2);

        r.crop(new Insets(1, 1, 0, 0));
        r.expand(1, 1);
        r.crop(getInsets(figure));
    }

}