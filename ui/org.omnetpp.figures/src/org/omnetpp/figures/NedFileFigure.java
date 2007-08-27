package org.omnetpp.figures;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.omnetpp.common.color.ColorFactory;

/**
 * Top level figure corresponding to a NED file
 *
 * @author rhornig
 */
public class NedFileFigure extends Figure {
    // global switch for antialiasing
    public static int antialias = SWT.ON;
    
    protected String problemMessage;

    public NedFileFigure() {
        super();
        ToolbarLayout fl = new ToolbarLayout();
        fl.setStretchMinorAxis(false);
        fl.setSpacing(20);
        setLayoutManager(fl);
        MarginBorder marginBorder = new MarginBorder(10);
		setBorder(marginBorder);
    }

	@Override
	public void paint(Graphics graphics) {
		super.paint(graphics);

		graphics.setBackgroundColor(problemMessage != null ? ColorFactory.RED : getBackgroundColor());
		Rectangle r = getBounds();
		graphics.fillRectangle(r.x, r.y, r.x + 12, r.y + r.height);
	}
	
    public void setProblemMessage(String problemMessage) {
    	this.problemMessage = problemMessage;
		if (problemMessage == null)
			setToolTip(null);
		else {
			TooltipFigure tooltipFigure = new TooltipFigure();
			tooltipFigure.setText(problemMessage);
			setToolTip(tooltipFigure);
		}
		invalidate();
    }
}
