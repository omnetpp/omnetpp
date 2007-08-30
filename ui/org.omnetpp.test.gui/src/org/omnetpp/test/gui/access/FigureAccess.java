package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.widgets.Control;

public class FigureAccess
	extends ClickableAccess
{
	protected IFigure figure;
	
	public FigureAccess(IFigure figure) {
		this.figure = figure;
	}
	
	public void click(int button, Control control) {
		Rectangle r = figure.getBounds().getCopy();
		figure.translateToAbsolute(r);
		click(button, control.toDisplay(getCenter(r)));
	}
}
