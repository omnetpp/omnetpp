package org.omnetpp.runtimeenv.figures;

import org.eclipse.draw2d.Border;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.SubmoduleFigure;

//XXX draw selection too?
public class SelectionBorder implements Border {
	private int handleSize = 5;

	public int getHandleSize() {
		return handleSize;
	}

	public void setHandleSize(int handleSize) {
		this.handleSize = handleSize;
	}

	@Override
	public Insets getInsets(IFigure figure) {
		return new Insets();
	}

	@Override
	public Dimension getPreferredSize(IFigure figure) {
		return figure.getPreferredSize();
	}

	@Override
	public boolean isOpaque() {
		return false;
	}

	@Override
	public void paint(IFigure figure, Graphics graphics, Insets insets) {
		Rectangle r;
		if (figure instanceof SubmoduleFigure)
			r = ((SubmoduleFigure)figure).getHandleBounds().getCropped(insets);  //XXX use interface
		else
			r = figure.getBounds().getCropped(insets);
		graphics.setBackgroundColor(ColorFactory.BLACK);
		graphics.fillRectangle(r.x, r.y, handleSize, handleSize);
		graphics.fillRectangle(r.x, r.bottom()-handleSize, handleSize, handleSize);
		graphics.fillRectangle(r.right()-handleSize, r.y, handleSize, handleSize);
		graphics.fillRectangle(r.right()-handleSize, r.bottom()-handleSize, handleSize, handleSize);
	}

}
