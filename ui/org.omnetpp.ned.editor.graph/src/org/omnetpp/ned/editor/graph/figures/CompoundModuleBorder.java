package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.AbstractBorder;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;

public class CompoundModuleBorder extends AbstractBorder {
	
	public final static int HEADER_HEIGHT = 42;

    protected static Insets insets = new Insets(HEADER_HEIGHT, 0, 0, 0);
    public Insets getInsets(IFigure figure) {
        return insets;
    }

    public void paint(IFigure figure, Graphics g, Insets in) {
        Rectangle r = figure.getBounds().getCropped(in);

        g.setForegroundColor(ColorFactory.logicGreen);
        g.setBackgroundColor(ColorFactory.logicGreen);

        // Draw the sides of the border
        g.fillRectangle(r.x, r.y, r.width, insets.getHeight());
    }

}